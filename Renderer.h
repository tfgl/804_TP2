/**
@file Renderer.h
@author JOL
*/
#pragma once
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Color.h"
#include "Image2D.h"
#include "GraphicalObject.h"
#include "Ray.h"
#include "Background.h"

/// Namespace RayTracer
namespace rt {

  inline void progressBar( std::ostream& output,
                           const double currentValue, const double maximumValue)
  {
    static const int PROGRESSBARWIDTH = 60;
    static int myProgressBarRotation = 0;
    static int myProgressBarCurrent = 0;
    // how wide you want the progress meter to be
    double fraction = currentValue /maximumValue;
    
    // part of the progressmeter that's already "full"
    int dotz = static_cast<int>(floor(fraction * PROGRESSBARWIDTH));
    if (dotz > PROGRESSBARWIDTH) dotz = PROGRESSBARWIDTH;
    
    // if the fullness hasn't changed skip display
    if (dotz == myProgressBarCurrent) return;
    myProgressBarCurrent = dotz;
    myProgressBarRotation++;
    
    // create the "meter"
    int ii=0;
    output << "[";
    // part  that's full already
    for ( ; ii < dotz;ii++) output<< "#";
    // remaining part (spaces)
    for ( ; ii < PROGRESSBARWIDTH;ii++) output<< " ";
    static const char* rotation_string = "|\\-/";
    myProgressBarRotation %= 4;
    output << "] " << rotation_string[myProgressBarRotation]
           << " " << (int)(fraction*100)<<"/100\r";
    output.flush();
  }
  
  /// This structure takes care of rendering a scene.
  struct Renderer {

    /// The scene to render
    Scene* ptrScene;
    /// The origin of the camera in space.
    Point3 myOrigin;
    /// (myOrigin, myOrigin+myDirUL) forms a ray going through the upper-left
    /// corner pixel of the viewport, i.e. pixel (0,0)
    Vector3 myDirUL;
    /// (myOrigin, myOrigin+myDirUR) forms a ray going through the upper-right
    /// corner pixel of the viewport, i.e. pixel (width,0)
    Vector3 myDirUR;
    /// (myOrigin, myOrigin+myDirLL) forms a ray going through the lower-left
    /// corner pixel of the viewport, i.e. pixel (0,height)
    Vector3 myDirLL;
    /// (myOrigin, myOrigin+myDirLR) forms a ray going through the lower-right
    /// corner pixel of the viewport, i.e. pixel (width,height)
    Vector3 myDirLR;

    int myWidth;
    int myHeight;

    Background* ptrBackground = new MyBackground();

    Renderer() : ptrScene( 0 ) {}
    Renderer( Scene& scene ) : ptrScene( &scene ) {}
    void setScene( rt::Scene& aScene ) { ptrScene = &aScene; }
    
    void setViewBox( Point3 origin, 
                     Vector3 dirUL, Vector3 dirUR, Vector3 dirLL, Vector3 dirLR )
    {
      myOrigin = origin;
      myDirUL = dirUL;
      myDirUR = dirUR;
      myDirLL = dirLL;
      myDirLR = dirLR;
    }

    void setResolution( int width, int height )
    {
      myWidth  = width;
      myHeight = height;
    }


    /// The main rendering routine
    void render( Image2D<Color>& image, int max_depth )
    {
      std::cout << "Rendering into image ... might take a while." << std::endl;
      image = Image2D<Color>( myWidth, myHeight );
      for ( int y = 0; y < myHeight; ++y ) 
        {
          Real    ty   = (Real) y / (Real)(myHeight-1);
          progressBar( std::cout, ty, 1.0 );
          Vector3 dirL = (1.0f - ty) * myDirUL + ty * myDirLL;
          Vector3 dirR = (1.0f - ty) * myDirUR + ty * myDirLR;
          dirL        /= dirL.norm();
          dirR        /= dirR.norm();
          for ( int x = 0; x < myWidth; ++x ) 
            {
              Real    tx   = (Real) x / (Real)(myWidth-1);
              Vector3 dir  = (1.0f - tx) * dirL + tx * dirR;
              Ray eye_ray  = Ray( myOrigin, dir, max_depth );
              Color result = trace( eye_ray );
              image.at( x, y ) = result.clamp();
            }
        }
      std::cout << "Done." << std::endl;
    }


    /// The rendering routine for one ray.
    /// @return the color for the given ray.
    Color trace( const Ray& ray )
    {
      assert( ptrScene != 0 );
      Color result = Color( 0.0, 0.0, 0.0 );
      GraphicalObject* obj_i = 0; // pointer to intersected object
      Point3           p_i;       // point of intersection

      // Look for intersection in this direction.
      Real ri = ptrScene->rayIntersection( ray, obj_i, p_i );
      // Nothing was intersected
      if ( ri >= 0.0f ) return background( ray ); // some background color

      const Material& m = obj_i->getMaterial(p_i);
      if( ray.depth > 0 && m.coef_reflexion ) {
        const Vector3 dir  = reflect(ray.direction, obj_i->getNormal(p_i));
        const Ray ray_refl = Ray(p_i + dir, dir, ray.depth - 1);
        const Color c_refl = trace(ray_refl);
        result += c_refl * m.specular * m.coef_reflexion;
      }
    
      if( ray.depth > 0 && m.coef_refraction ) {
        const Ray ray_refr = refractionRay(ray, p_i, obj_i->getNormal(p_i), m);
              Color c_refr = trace(ray_refr);
        result += c_refr * m.diffuse * m.coef_refraction;
      }

      return result + m.ambient + illumination(ray,obj_i,p_i) * ((ray.depth != 0) ? m.coef_diffusion : 1);
    }

    Color illumination( const Ray& ray, GraphicalObject* obj, Point3 p )
    {
      Color c = Color(0, 0, 0);
      for(const auto& l: this->ptrScene->myLights) {
          const Material& m     = obj->getMaterial(p);
          const Color&    b     = l->color(p);
          const Vector3&  l_dir = l->direction(p);
          const Vector3&  norm  = obj->getNormal(p);
          const Real      kd    = std::max( norm.dot(l_dir) / (l_dir.norm() * norm.norm()), 0.f );
          const Vector3&  w     = reflect(ray.direction, norm);
          const Real      cosB  = std::max( w.dot(l_dir) / (l_dir.norm() * w.norm()), 0.f );
          const Real      ks    = std::pow(cosB, m.shinyness);

          c +=(kd * m.diffuse * b)
            + (ks * m.specular * b);

          c = c * shadow(Ray(p, l_dir), b);
      }
      return c;
    }

    /// Calcule le vecteur r??fl??chi ?? W selon la normale N.
    Vector3 reflect( const Vector3& W, Vector3 N ) const
    { return W -2 * W.dot(N) * N; }

    /// Calcule la couleur de la lumi??re (donn??e par light_color) dans la
    /// direction donn??e par le rayon. Si aucun objet n'est travers??,
    /// retourne light_color, sinon si un des objets travers??s est opaque,
    /// retourne du noir, et enfin si les objets travers??s sont
    /// transparents, attenue la couleur.
    Color shadow( const Ray& ray, Color light_color )
    {
      Point3 p = ray.origin, q;
      GraphicalObject* obj;

      while( light_color.max() > 0.003f ) {
        p += ray.direction * 0.003f;
        Ray r = Ray(p, ray.direction, ray.depth);

        if( ptrScene->rayIntersection(r, obj, q) >= 0)
          break;

        Material m = obj->getMaterial(p);
        light_color = light_color * m.diffuse * m.coef_refraction;
        p = q;
      }
      return light_color;
    }

    Ray refractionRay( const Ray& aRay, const Point3& p, Vector3 N, const Material& m )
    {
      const Vector3 V = aRay.direction;
      const Real c = -(N.dot(V));
      const Real r = (c < 0) ? m.in_refractive_index / m.out_refractive_index
                             : m.out_refractive_index / m.in_refractive_index;
      const Real root = 1 - r*r * (1 - c*c);
            Vector3 vr = Vector3(r*V + (r*c + ((c>0) ? -1 : 1) * sqrt(root)) * N);
                     //r*V + (r*c +- sqrt(1 - r*r * (1 - c*c))*N )
      if( root < 0 ) vr = reflect(V, N);

      return Ray(p + vr * 0.01f, vr, aRay.depth - 1);
    }


    // Affiche les sources de lumi??res avant d'appeler la fonction qui
    // donne la couleur de fond.
    Color background( const Ray& ray )
    {
      Color result = Color( 0.0, 0.0, 0.0 );
      for ( Light* light : ptrScene->myLights )
        {
          Real cos_a = light->direction( ray.origin ).dot( ray.direction );
          if ( cos_a > 0.99f )
            {
              Real a = acos( cos_a ) * 360.0 / M_PI / 8.0;
              a = std::max( 1.0f - a, 0.0f );
              result += light->color( ray.origin ) * a * a;
            }
        }
      if ( ptrBackground != 0 ) result += ptrBackground->backgroundColor( ray );
      return result;
    }

  };

} // namespace rt

#endif // #define _RENDERER_H_

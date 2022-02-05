/**
@file Scene.h
@author JOL
*/
#pragma once
#ifndef _SCENE_H_
#define _SCENE_H_

#include <cassert>
#include <vector>
#include "GraphicalObject.h"
#include "PointLight.h"
#include "Light.h"
#include "Sphere.h"
#include "Plan.h"

/// Namespace RayTracer
namespace rt {

  /**
  Models a scene, i.e. a collection of lights and graphical objects
  (could be a tree, but we keep a list for now for the sake of
  simplicity).

  @note Once the scene receives a new object, it owns the object and
  is thus responsible for its deallocation.
  */

  struct Scene {
    /// The list of lights modelled as a vector.
    std::vector< Light* > myLights;
    /// The list of objects modelled as a vector.
    std::vector< GraphicalObject* > myObjects;
    /// The name of the file describing the scene if loaded from a file
    std::string filename, materialsFilename;

    /// Default constructor. Nothing to do.
    Scene() {}

    /// Destructor. Frees objects.
    ~Scene() 
    {
      for ( Light* light : myLights )
        delete light;
      for ( GraphicalObject* obj : myObjects )
        delete obj;
      // The vector is automatically deleted.
    }

    /// This function calls the init method of each of its objects.
    void init( Viewer& viewer )
    {
      for ( GraphicalObject* obj : myObjects )
        obj->init( viewer );
      for ( Light* light : myLights )
        light->init( viewer );
    }
    /// This function calls the draw method of each of its objects.
    void draw( Viewer& viewer )
    {
      for ( GraphicalObject* obj : myObjects )
        obj->draw( viewer );
      for ( Light* light : myLights )
        light->draw( viewer );
    }
    /// This function calls the light method of each of its lights
    void light(Viewer& viewer )
    {
      for ( Light* light : myLights )
        light->light( viewer );
    }

    /// Adds a new object to the scene.
    void addObject( GraphicalObject* anObject )
    {
      myObjects.push_back( anObject );
    }

    /// Adds a new light to the scene.
    void addLight( Light* aLight )
    {
      myLights.push_back( aLight );
    }
    
    /// returns the closest object intersected by the given ray.
    Real rayIntersection( const Ray& ray,
                     GraphicalObject*& object, Point3& p )
    {
      Point3 q;
      Real min = 0;
      bool contact = false;

      for(const auto& o: myObjects) {
        if( o->rayIntersection(ray, q) < 0 ) {
          Real dst = distance2(ray.origin, q);
          ((dst < min) || !contact) ? contact = true, object = o, p = q, min = dst : 0;
        }
      }

      return contact ? -1 : 1;
    }

    void loadFromFile(const std::string filename, const std::string materialsFilename )
    {
      auto mats = Material::loadMaterials(materialsFilename);
      std::ifstream input( filename );
      this->filename = filename;
      this->materialsFilename = materialsFilename;

      for(std::string line; getline(input, line); ) {
        if( line.empty() || line.at(0) == '#' ) continue;
        std::stringstream s(line);
        std::string shape;
        s >> shape;

        if( shape == "sphere" ) {
          std::string matName;
          float rayon, x, y, z;
          s >> x >> y >> z >> rayon >> matName;
          this->addObject(new Sphere( Point3(x, y, z), rayon, mats[matName] ));
        }
        else if( shape == "bulle" ) {
          std::string matName;
          float rayon, x, y, z;
          s >> x >> y >> z >> rayon >> matName;
          this->addBubble( Point3(x, y, z), rayon, mats[matName] );
        }
        else if( shape == "light" ) {
          float x, y, z;
          s >> x >> y >> z;
          this->addLight( new PointLight( GL_LIGHT1, Point4(x, y, z, 1 ), Color( 1.0, 1.0, 1.0 ) ));
        }
        else if( shape == "plan" ) {
          Point3 u, v, w;
          std::string main_m, band_m;
          Real width;
          s >> u >> v >> w >> main_m >> band_m >> width;
          this->addObject( new PeriodicPlane(u, v, w, mats[main_m], mats[band_m], width));
        }
      }

      input.close();
    }

    void reload() {
      if( this->filename.empty() ) return;
      std::cout << "reloading" << std::endl;
      this->myObjects.clear();
      this->myLights.clear();
      this->loadFromFile(this->filename, this->materialsFilename);
    }

    void addBubble( Point3 c, Real r, Material transp_m )
    {
      Material revert_m = transp_m;
      std::swap( revert_m.in_refractive_index, revert_m.out_refractive_index );
      Sphere* sphere_out = new Sphere( c, r, transp_m );
      Sphere* sphere_in  = new Sphere( c, r-0.02f, revert_m );
      this->addObject( sphere_out );
      this->addObject( sphere_in );
    }

  private:
    /// Copy constructor is forbidden.
    Scene( const Scene& ) = delete;
    /// Assigment is forbidden.
    Scene& operator=( const Scene& ) = delete;
  };

} // namespace rt

#endif // #define _SCENE_H_

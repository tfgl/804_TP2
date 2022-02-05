#pragma once
#ifndef _PLAN_H_
#define _PLAN_H_

#include <cmath>

#include "Color.h"
#include "Image2D.h"
#include "GraphicalObject.h"
#include "Ray.h"

using namespace rt;
struct PeriodicPlane : public GraphicalObject // Vous devez dériver de GraphicalObject
{
  /// Creates a periodic infinite plane passing through \a c and
  /// tangent to \a u and \a v. Then \a w defines the width of the
  /// band around (0,0) and its period to put material \a band_m,
  /// otherwise \a main_m is used.
  PeriodicPlane( Point3 c, Vector3 u, Vector3 v,
    Material main_m, Material band_m, Real w)
    : _c(c), _u(u), _v(v), _main_m(main_m), _band_m(band_m), _w(w) {}

  /// Etant donné un point \a p dans le plan, retourne ses coordonnées \a x et \a y selon \a u et \a v.
  void coordinates( Point3 p, Real& x, Real& y )
  {
      x = p.dot(_u / _u.norm());
      y = p.dot(_v / _v.norm());
  }

  void init(Viewer&) override {}

  void draw(Viewer&) override
  {
    glBegin( GL_TRIANGLES );
    glColor4fv( _main_m.ambient );
    glMaterialfv(GL_FRONT,   GL_DIFFUSE, _main_m.diffuse);
    glMaterialfv(GL_FRONT,  GL_SPECULAR, _main_m.specular);
    glMaterialf (GL_FRONT, GL_SHININESS, _main_m.shinyness);
    glVertex3f( 1000,  1000, 0);
    glVertex3f(    0,  1000, 0);
    glVertex3f( 1000,     0, 0);
    glEnd();

    glBegin( GL_TRIANGLES );
    glColor4fv( _main_m.ambient );
    glMaterialfv(GL_FRONT,   GL_DIFFUSE, _main_m.diffuse);
    glMaterialfv(GL_FRONT,  GL_SPECULAR, _main_m.specular);
    glMaterialf (GL_FRONT, GL_SHININESS, _main_m.shinyness);
    glVertex3f(-1000, -1000, 0);
    glVertex3f(    0,  1000, 0);
    glVertex3f( 1000,     0, 0);
    glEnd();
  }

  Vector3 getNormal( Point3 p ) override
  { return p + _u.cross(_v); }

  Material getMaterial(Point3 p) override
  {
    Real x, y;
    coordinates(p, x, y);
    double intX, intY;
    double doubleX = abs(modf(x, &intX)),
           doubleY = abs(modf(y, &intY));
    
    return ( doubleX < _w || doubleY < _w ) ? _band_m : _main_m;
  }

  Real rayIntersection( const Ray& ray, Point3& p ) override
  {
    const Vector3 u = getNormal(_c) / getNormal(_c).norm(),
                  v = ray.direction / ray.direction.norm();
    const Real   uv = u.dot(v),
                  w = u.dot(_c - ray.origin);

    
    if( !uv ) return !w ? p = ray.origin, -1 : 1;

    p = ray.origin + v * (w / uv);
    return ((w - 1) / uv) < 0 ? 1 : -1;
  }

  Point3 _c;
  Vector3 _u, _v;
  Material _main_m, _band_m;
  Real _w;
};

#endif

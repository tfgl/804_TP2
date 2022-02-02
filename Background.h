#pragma once
#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include "Color.h"
#include "Image2D.h"
#include "GraphicalObject.h"
#include "Ray.h"

using namespace rt;
struct Background {
  virtual Color backgroundColor( const Ray& ray ) = 0;
};
 
struct MyBackground : public Background {
  Color backgroundColor( const Ray& ray )
  {
    const Real z = ray.direction[2],
               zx2 = z * 2;

    if( zx2 >= 0)
      return (zx2 < 1)
        ? Color(1 - zx2, 1 - zx2, 1)
        : Color(0, 0, 2 - zx2) ;

    // Damier

    Color result;
    Real x = -0.5f * ray.direction[ 0 ] / ray.direction[ 2 ];
    Real y = -0.5f * ray.direction[ 1 ] / ray.direction[ 2 ];
    Real d = sqrt( x*x + y*y );
    Real t = std::min( d, 30.0f ) / 30.0f;

    x -= floor( x );
    y -= floor( y );
    if ( ( ( x >= 0.5f ) && ( y >= 0.5f ) ) || ( ( x < 0.5f ) && ( y < 0.5f ) ) )
      result += (1.0f - t)*Color( 0.2f, 0.2f, 0.2f ) + t * Color( 1.0f, 1.0f, 1.0f );
    else
      result += (1.0f - t)*Color( 0.4f, 0.4f, 0.4f ) + t * Color( 1.0f, 1.0f, 1.0f );

    return result;

  }
};

#endif

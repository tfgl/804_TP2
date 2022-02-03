#pragma once
#ifndef _PLAN_H_
#define _PLAN_H_

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
    Material main_m, Material band_m, Real w);

};


#endif


/*
    pbrt source code is Copyright(c) 1998-2015
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_SHAPES_HYPERBOLOID_H
#define PBRT_SHAPES_HYPERBOLOID_H
#include "stdafx.h"

// shapes/hyperboloid.h*
#include "shape.h"

// Hyperboloid Declarations
class Hyperboloid : public Shape {
  public:
    // Hyperboloid Public Methods
    Hyperboloid(const Transform *o2w, const Transform *w2o, bool ro,
                const Point3f &point1, const Point3f &point2, Float tm);
    Bounds3f ObjectBound() const;
    bool Intersect(const Ray &ray, Float *tHit,
                   SurfaceInteraction *isect) const;
    bool IntersectP(const Ray &ray) const;
    Float Area() const;
    bool Sample(const Point2f &sample, Interaction *it) const;

  protected:
    // Hyperboloid Private Data
    Point3f p1, p2;
    Float zMin, zMax;
    Float phiMax;
    Float rMax;
    Float ah, ch;
};

std::shared_ptr<Shape> CreateHyperboloidShape(const Transform *o2w,
                                              const Transform *w2o,
                                              bool reverseOrientation,
                                              const ParamSet &params);

#endif  // PBRT_SHAPES_HYPERBOLOID_H

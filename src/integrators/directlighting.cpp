
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

#include "stdafx.h"

// integrators/directlighting.cpp*
#include "integrators/directlighting.h"
#include "interaction.h"
#include "paramset.h"

// DirectLightingIntegrator Method Definitions
DirectLightingIntegrator::DirectLightingIntegrator(
    LightStrategy st, int md, std::shared_ptr<Sampler> sampler,
    std::shared_ptr<const Camera> camera)
    : SamplerIntegrator(sampler, camera) {
    maxDepth = md;
    strategy = st;
}

void DirectLightingIntegrator::Preprocess(const Scene &scene) {
    if (strategy == LightStrategy::UniformSampleAll) {
        // Request samples for sampling all lights
        for (int i = 0; i < maxDepth; ++i) {
            for (const auto &light : scene.lights) {
                int nSamples = sampler->RoundCount(light->nSamples);
                sampler->Request2DArray(nSamples);
                sampler->Request2DArray(nSamples);
                numLightSamples.push_back(nSamples);
            }
        }
    }
}

Spectrum DirectLightingIntegrator::Li(const RayDifferential &ray,
                                      const Scene &scene, Sampler &sampler,
                                      MemoryArena &arena) const {
    Spectrum L(0.f);
    // Store intersection into _isect_ or return background radiance if none was
    // found
    SurfaceInteraction isect;
    if (!scene.Intersect(ray, &isect)) {
        for (uint32_t i = 0; i < scene.lights.size(); ++i)
            L += scene.lights[i]->Le(ray);
        return L;
    }

    // Compute scattering functions for surface interaction
    isect.ComputeScatteringFunctions(ray, arena);
    Vector3f wo = isect.wo;
    // Compute emitted light if ray hit an area light source
    L += isect.Le(wo);

    // Compute direct lighting for _DirectLightingIntegrator_ integrator
    if (scene.lights.size() > 0) {
        // Apply direct lighting strategy
        if (strategy == LightStrategy::UniformSampleAll)
            L += UniformSampleAllLights(isect, scene, sampler, numLightSamples,
                                        arena, false);
        else
            L += UniformSampleOneLight(isect, scene, sampler, arena, false);
    }
    if (ray.depth + 1 < maxDepth) {
        Vector3f wi;
        // Trace rays for specular reflection and refraction
        L += SpecularReflect(ray, isect, *this, scene, sampler, arena);
        L += SpecularTransmit(ray, isect, *this, scene, sampler, arena);
    }
    return L;
}

DirectLightingIntegrator *CreateDirectLightingIntegrator(
    const ParamSet &params, std::shared_ptr<Sampler> sampler,
    std::shared_ptr<const Camera> camera) {
    int maxDepth = params.FindOneInt("maxdepth", 5);
    LightStrategy strategy;
    std::string st = params.FindOneString("strategy", "all");
    if (st == "one")
        strategy = LightStrategy::UniformSampleOne;
    else if (st == "all")
        strategy = LightStrategy::UniformSampleAll;
    else {
        Warning(
            "Strategy \"%s\" for direct lighting unknown. "
            "Using \"all\".",
            st.c_str());
        strategy = LightStrategy::UniformSampleAll;
    }
    return new DirectLightingIntegrator(strategy, maxDepth, sampler, camera);
}

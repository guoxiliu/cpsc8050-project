
/*
    pbrt source code is Copyright(c) 1998-2016
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


// materials/iridescence.cpp*
#include "materials/iridescence.h"
#include "paramset.h"
#include "reflection.h"
#include "interaction.h"
#include "texture.h"
#include "interaction.h"

namespace pbrt {

// Iridescence Method Definitions
void IridescenceMaterial::ComputeScatteringFunctions(SurfaceInteraction *si,
                                               MemoryArena &arena,
                                               TransportMode mode,
                                               bool allowMultipleLobes) const {
    // Perform bump mapping with _bumpMap_, if present
    if (bumpMap) Bump(bumpMap, si);

    // Evaluate textures for Iridescence_ material and allocate BRDF
    si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
    Spectrum R = Kr->Evaluate(*si).Clamp();
    Float d0 = dAir->Evaluate(*si);
    Float d1 = dFilm->Evaluate(*si);
    Float a = width->Evaluate(*si);
    Float eta = index->Evaluate(*si);
    if (!R.IsBlack()) {
        si->bsdf->Add(ARENA_ALLOC(arena, SeparateLamellaeReflection)
                        (R, numLayers, d1, a, 1.f, eta));
        // si->bsdf->Add(ARENA_ALLOC(arena, MultilayerThinFilmReflection)
        //                 (R, numLayers, d0, d1, 1.f, eta));
    }
}

IridescenceMaterial *CreateIridescenceMaterial(const TextureParams &mp) {
    std::shared_ptr<Texture<Spectrum>> Kr =
        mp.GetSpectrumTexture("Kr", Spectrum(0.8f));
    int layers = mp.FindInt("numLayers", 10);
    std::shared_ptr<Texture<Float>> dAir = mp.GetFloatTexture("dAir", 0.09f);
    std::shared_ptr<Texture<Float>> dFilm = mp.GetFloatTexture("dFilm", 0.09f);
    std::shared_ptr<Texture<Float>> width = mp.GetFloatTexture("width", 0.06f);
    std::shared_ptr<Texture<Float>> index = mp.GetFloatTexture("index", 1.53f);
    std::shared_ptr<Texture<Float>> bumpMap =
        mp.GetFloatTextureOrNull("bumpmap");
    return new IridescenceMaterial(Kr, layers, dAir, dFilm, width, index, bumpMap);
}

}  // namespace pbrt

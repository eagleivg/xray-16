#include "stdafx.h"
#pragma hdrstop

#include "blender_accum_volumetric_nomsaa.h"

CBlender_accum_volumetric_nomsaa::CBlender_accum_volumetric_nomsaa() { description.CLS = 0; }
CBlender_accum_volumetric_nomsaa::~CBlender_accum_volumetric_nomsaa() { }

void CBlender_accum_volumetric_nomsaa::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0:
    case 1:
        C.r_Pass("accum_volumetric", "accum_volumetric", false, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        C.SetParams(2, FALSE);
        C.r_Sampler("s_lmap", C.L_textures[0]);
        C.r_Sampler("s_dmap", r2_RT_smap_depth);
        C.r_Sampler_cmp("s_smap", r2_RT_smap_depth);
        C.r_Sampler("s_noise", "fx\\fx_noise2");
        C.r_End();
        break;
    }
}

#include "stdafx.h"
#pragma hdrstop

#include "blender_accum_volumetric_sun_nomsaa.h"

CBlender_accum_volumetric_sun_nomsaa::CBlender_accum_volumetric_sun_nomsaa() { description.CLS = 0; }
CBlender_accum_volumetric_sun_nomsaa::~CBlender_accum_volumetric_sun_nomsaa() { }

void CBlender_accum_volumetric_sun_nomsaa::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0:
    case 1:
        C.r_Pass("accum_sun", "accum_volumetric_sun_nomsaa", false, FALSE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        C.SetParams(2, FALSE);
        C.r_Sampler("s_dmap", r2_RT_smap_depth);
        C.r_Sampler_cmp("s_smap", r2_RT_smap_depth);
        C.r_Sampler("s_smap_minmax", r2_RT_smap_depth_minmax);
        C.r_Sampler("s_position", r2_RT_P);
        jitter(C);
        C.r_End();
        break;
    }
}

CBlender_accum_volumetric_sun_nomsaa_minmax::CBlender_accum_volumetric_sun_nomsaa_minmax() { description.CLS = 0; }
CBlender_accum_volumetric_sun_nomsaa_minmax::~CBlender_accum_volumetric_sun_nomsaa_minmax() { }

void CBlender_accum_volumetric_sun_nomsaa_minmax::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0:
    case 1:
        C.r_Pass("accum_sun", "accum_volumetric_sun_minmax", false, FALSE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        C.SetParams(2, FALSE);
        C.r_Sampler("s_dmap", r2_RT_smap_depth);
        C.r_Sampler_cmp("s_smap", r2_RT_smap_depth);
        C.r_Sampler("s_smap_minmax", r2_RT_smap_depth_minmax);
        C.r_Sampler("s_position", r2_RT_P);
        jitter(C);
        C.r_End();
        break;
    }
}

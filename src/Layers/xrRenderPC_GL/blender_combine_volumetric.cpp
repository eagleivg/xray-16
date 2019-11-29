#include "stdafx.h"
#pragma hdrstop

#include "blender_combine_volumetric.h"

CBlender_combine_volumetric::CBlender_combine_volumetric() { description.CLS = 0; }
CBlender_combine_volumetric::~CBlender_combine_volumetric() { }

void CBlender_combine_volumetric::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0:
    case 1:
        C.r_Pass("combine_1", "combine_volumetric", false, FALSE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        C.SetParams(2, FALSE);
        C.r_Sampler("s_vollight", r2_RT_generic2);
        C.r_Sampler("s_tonemap", r2_RT_luminance_cur);
        C.r_End();
        break;
    }
}

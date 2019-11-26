#include "stdafx.h"
#pragma hdrstop

#include "blender_distort.h"

CBlender_distort::CBlender_distort() { description.CLS = 0; }
CBlender_distort::~CBlender_distort() { }

void CBlender_distort::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0:
    case 1:
        C.r_Pass("stub_notransform_t_menu", "distort", false, FALSE, FALSE);
        C.r_Sampler_clf("s_base", r2_RT_generic0);
        C.r_Sampler_clf("s_distort", r2_RT_generic1);
        C.r_End();
        break;
    }
}

#include "stdafx.h"
#pragma hdrstop

#include "blender_postprocess.h"

CBlender_postprocess::CBlender_postprocess() { description.CLS = 0; }
CBlender_postprocess::~CBlender_postprocess() { }

void CBlender_postprocess::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0:
    case 1:
        C.r_Pass("stub_notransform_postpr", "postprocess", false, FALSE, FALSE);
        C.r_Sampler_clf("s_base0", r2_RT_albedo);
        C.r_Sampler_clf("s_base1", r2_RT_albedo);
        C.r_Sampler("s_noise", "fx\\fx_noise2");
        C.r_End();
        break;
    case 4:
        C.r_Pass("stub_notransform_postpr", "postprocess_CM", false, FALSE, FALSE);
        C.r_Sampler_clf("s_base0", r2_RT_albedo);
        C.r_Sampler_clf("s_base1", r2_RT_albedo);
        C.r_Sampler("s_noise", "fx\\fx_noise2");

        C.r_Sampler_clf("s_grad0", "$user$cmap0");
        C.r_Sampler_clf("s_grad1", "$user$cmap0");

        C.r_End();
        break;
    }
}

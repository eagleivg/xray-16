#include "stdafx.h"
#include "r__pixel_calculator.h"
#define rt_dimensions 1024
#include "Layers/xrRender/FBasicVisual.h"
#ifdef USE_OGL
void XRMatrixOrthoOffCenterLH(Fmatrix *pout, float l, float r, float b, float t, float zn, float zf);
#endif

void r_pixel_calculator::begin()
{
    rt.create("$user$pixel_calculator_rt", rt_dimensions, rt_dimensions, HW.Caps.fTarget);
    zb.create("$user$pixel_calculator_zb", rt_dimensions, rt_dimensions, HW.Caps.fDepth, 1, { CRT::CreateSurface });

    RCache.set_RT(rt->pRT);
#if defined(USE_DX10) || defined(USE_DX11)
    RCache.set_ZB(zb->pZRT);
#else
    RCache.set_ZB(zb->pRT);
#endif

    R_ASSERT(Device.RenderBegin());
}

void r_pixel_calculator::end()
{
    Device.RenderEnd();

    RCache.set_RT(HW.pBaseRT);
    RCache.set_ZB(HW.pBaseZB);

    zb = nullptr;
    rt = nullptr;
}

// +X, -X, +Y, -Y, +Z, -Z
static Fvector cmNorm[6] = {
    {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}};
static Fvector cmDir[6] = {
    {1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, -1.f, 0.f}, {0.f, 0.f, 1.f}, {0.f, 0.f, -1.f}};

r_aabb_ssa r_pixel_calculator::calculate(dxRender_Visual* V)
{
    r_aabb_ssa result = {0};
    float area = float(_sqr(rt_dimensions));

    u32 id[6];
    for (u32 face = 0; face < 6; face++)
    {
        // setup matrices
        Fmatrix mProject, mView;
        Fvector vFrom;
        Fbox aabb;

        // camera - left-to-right
        mView.build_camera_dir(vFrom.invert(cmDir[face]).mul(100.f), cmDir[face], cmNorm[face]);
        aabb.xform(V->vis.box, mView);
#ifdef USE_OGL
        XRMatrixOrthoOffCenterLH(
#else
        D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)
#endif
            &mProject, aabb.vMin.x, aabb.vMax.x, aabb.vMin.y, aabb.vMax.y, aabb.vMin.z, aabb.vMax.z);
        RCache.set_xform_world(Fidentity);
        RCache.set_xform_view(mView);
        RCache.set_xform_project(mProject);

        // render-0
        Device.Clear(); // clear-ZB
        RCache.set_Shader(V->shader);
        V->Render(1.f);

        // render-1
        RImplementation.HWOCC.occq_begin(id[face]);
        V->Render(1.f);
        RImplementation.HWOCC.occq_end(id[face]);
    }

    //
    for (u32 it = 0; it < 6; it++)
    {
        float pixels = static_cast<float>(RImplementation.HWOCC.occq_get(id[it]));
        float coeff = clampr(pixels / area, float(0), float(1));
        Msg("[%d]ssa_c: %1.3f,%f/%f", it, coeff, pixels, area);
        result.ssa[it] = static_cast<u8>(clampr(iFloor(coeff * 255.f + 0.5f), int(0), int(255)));
    }

    return result;
}

void r_pixel_calculator::run()
{
    begin();
    for (u32 it = 0; it < RImplementation.Visuals.size(); it++)
    {
        if (nullptr == dynamic_cast<IRender_Mesh*>(RImplementation.Visuals[it]))
            continue;
        Msg("*%d*", it);
        calculate(dynamic_cast<dxRender_Visual*>(RImplementation.Visuals[it]));
    }
    end();
}

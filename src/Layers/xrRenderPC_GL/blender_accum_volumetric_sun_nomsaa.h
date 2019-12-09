#pragma once

class CBlender_accum_volumetric_sun_nomsaa : public IBlender
{
public:
    LPCSTR getComment() override { return "accum_volumetric_sun_nomsaa"; }
    BOOL canBeDetailed() override { return FALSE; }
    BOOL canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_volumetric_sun_nomsaa();
    virtual ~CBlender_accum_volumetric_sun_nomsaa();
};

class CBlender_accum_volumetric_sun_nomsaa_minmax : public IBlender
{
public:
    LPCSTR getComment() override { return "accum_volumetric_sun_nomsaa_minmax"; }
    BOOL canBeDetailed() override { return FALSE; }
    BOOL canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_volumetric_sun_nomsaa_minmax();
    virtual ~CBlender_accum_volumetric_sun_nomsaa_minmax();
};
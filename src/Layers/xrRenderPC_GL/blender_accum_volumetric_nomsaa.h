#pragma once

class CBlender_accum_volumetric_nomsaa : public IBlender
{
public:
    LPCSTR getComment() override { return "accum_volumetric_nomsaa"; }
    BOOL canBeDetailed() override { return FALSE; }
    BOOL canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_volumetric_nomsaa();
    virtual ~CBlender_accum_volumetric_nomsaa();
};

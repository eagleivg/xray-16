#pragma once

class CBlender_combine_volumetric : public IBlender
{
public:
    LPCSTR getComment() override { return "combine_volumetric"; }
    BOOL canBeDetailed() override { return FALSE; }
    BOOL canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_combine_volumetric();
    virtual ~CBlender_combine_volumetric();
};

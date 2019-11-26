#pragma once

class CBlender_postprocess : public IBlender
{
public:
    LPCSTR getComment() override { return "postprocess"; }
    BOOL canBeDetailed() override { return FALSE; }
    BOOL canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_postprocess();
    virtual ~CBlender_postprocess();
};

#pragma once

class CBlender_distort : public IBlender
{
public:
    LPCSTR getComment() override { return "INTERNAL: distort"; }
    BOOL canBeDetailed() override { return FALSE; }
    BOOL canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_distort();
    virtual ~CBlender_distort();
};

#include "stdafx.h"
#pragma hdrstop

#include "xrEngine/Render.h"
#include "ResourceManager.h"
#include "tss.h"
#include "blenders/Blender.h"
#include "blenders/Blender_Recorder.h"
#include "xrScriptEngine/script_engine.hpp"

#ifdef DEBUG
#define MDB Memory.dbg_check()
#else
#define MDB
#endif

// wrapper
class adopt_sampler
{
    CBlender_Compile* C;
    u32 stage;

public:
    adopt_sampler(CBlender_Compile* _C, u32 _stage) : C(_C), stage(_stage)
    {
        if (u32(-1) == stage)
            C = nullptr;
    }
    adopt_sampler(const adopt_sampler& _C) : C(_C.C), stage(_C.stage)
    {
        if (u32(-1) == stage)
            C = nullptr;
    }

    adopt_sampler& _texture(LPCSTR texture)
    {
        if (C)
            C->i_Texture(stage, texture);
        return *this;
    }
    adopt_sampler& _projective(bool _b)
    {
        if (C)
            C->i_Projective(stage, _b);
        return *this;
    }
    adopt_sampler& _clamp()
    {
        if (C)
            C->i_Address(stage, D3DTADDRESS_CLAMP);
        return *this;
    }
    adopt_sampler& _wrap()
    {
        if (C)
            C->i_Address(stage, D3DTADDRESS_WRAP);
        return *this;
    }
    adopt_sampler& _mirror()
    {
        if (C)
            C->i_Address(stage, D3DTADDRESS_MIRROR);
        return *this;
    }
    adopt_sampler& _f_anisotropic()
    {
        if (C)
            C->i_Filter(stage, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
        return *this;
    }
    adopt_sampler& _f_trilinear()
    {
        if (C)
            C->i_Filter(stage, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR);
        return *this;
    }
    adopt_sampler& _f_bilinear()
    {
        if (C)
            C->i_Filter(stage, D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTEXF_LINEAR);
        return *this;
    }
    adopt_sampler& _f_linear()
    {
        if (C)
            C->i_Filter(stage, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);
        return *this;
    }
    adopt_sampler& _f_none()
    {
        if (C)
            C->i_Filter(stage, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
        return *this;
    }
    adopt_sampler& _fmin_none()
    {
        if (C)
            C->i_Filter_Min(stage, D3DTEXF_NONE);
        return *this;
    }
    adopt_sampler& _fmin_point()
    {
        if (C)
            C->i_Filter_Min(stage, D3DTEXF_POINT);
        return *this;
    }
    adopt_sampler& _fmin_linear()
    {
        if (C)
            C->i_Filter_Min(stage, D3DTEXF_LINEAR);
        return *this;
    }
    adopt_sampler& _fmin_aniso()
    {
        if (C)
            C->i_Filter_Min(stage, D3DTEXF_ANISOTROPIC);
        return *this;
    }
    adopt_sampler& _fmip_none()
    {
        if (C)
            C->i_Filter_Mip(stage, D3DTEXF_NONE);
        return *this;
    }
    adopt_sampler& _fmip_point()
    {
        if (C)
            C->i_Filter_Mip(stage, D3DTEXF_POINT);
        return *this;
    }
    adopt_sampler& _fmip_linear()
    {
        if (C)
            C->i_Filter_Mip(stage, D3DTEXF_LINEAR);
        return *this;
    }
    adopt_sampler& _fmag_none()
    {
        if (C)
            C->i_Filter_Mag(stage, D3DTEXF_NONE);
        return *this;
    }
    adopt_sampler& _fmag_point()
    {
        if (C)
            C->i_Filter_Mag(stage, D3DTEXF_POINT);
        return *this;
    }
    adopt_sampler& _fmag_linear()
    {
        if (C)
            C->i_Filter_Mag(stage, D3DTEXF_LINEAR);
        return *this;
    }
};

// wrapper
class adopt_compiler
{
    CBlender_Compile* C;

public:
    adopt_compiler(CBlender_Compile* _C) : C(_C) {}
    adopt_compiler(const adopt_compiler& _C) : C(_C.C) {}
    adopt_compiler& _options(int P, bool S)
    {
        C->SetParams(P, S);
        return *this;
    }
    adopt_compiler& _o_emissive(bool E)
    {
        C->SH->flags.bEmissive = E;
        return *this;
    }
    adopt_compiler& _o_distort(bool E)
    {
        C->SH->flags.bDistort = E;
        return *this;
    }
    adopt_compiler& _o_wmark(bool E)
    {
        C->SH->flags.bWmark = E;
        return *this;
    }
    adopt_compiler& _pass(LPCSTR vs, LPCSTR ps)
    {
        C->r_Pass(vs, ps, true);
        return *this;
    }
    adopt_compiler& _fog(bool _fog)
    {
        C->PassSET_LightFog(FALSE, _fog);
        return *this;
    }
    adopt_compiler& _ZB(bool _test, bool _write)
    {
        C->PassSET_ZB(_test, _write);
        return *this;
    }
    adopt_compiler& _blend(bool _blend, u32 abSRC, u32 abDST)
    {
        C->PassSET_ablend_mode(_blend, abSRC, abDST);
        return *this;
    }
    adopt_compiler& _aref(bool _aref, u32 aref)
    {
        C->PassSET_ablend_aref(_aref, aref);
        return *this;
    }
    adopt_compiler& _color_write_enable(bool cR, bool cG, bool cB, bool cA)
    {
        C->r_ColorWriteEnable(cR, cG, cB, cA);
        return *this;
    }
    adopt_sampler _sampler(LPCSTR _name)
    {
        u32 s = C->r_Sampler(_name, nullptr);
        return adopt_sampler(C, s);
    }
};

class adopt_blend
{
public:
};

// export
void CResourceManager::LS_Load()
{
    auto exporterFunc = [](lua_State* luaState)
    {
        sol::state_view lua(luaState);

        lua.new_usertype<adopt_sampler>("_sampler",
            sol::constructors<adopt_sampler(const adopt_sampler&)>(),
            "texture",            &adopt_sampler::_texture,
            "project",            &adopt_sampler::_projective,
            "clamp",              &adopt_sampler::_clamp,
            "wrap",               &adopt_sampler::_wrap,
            "mirror",             &adopt_sampler::_mirror,
            "f_anisotropic",      &adopt_sampler::_f_anisotropic,
            "f_trilinear",        &adopt_sampler::_f_trilinear,
            "f_bilinear",         &adopt_sampler::_f_bilinear,
            "f_linear",           &adopt_sampler::_f_linear,
            "f_none",             &adopt_sampler::_f_none,
            "fmin_none",          &adopt_sampler::_fmin_none,
            "fmin_point",         &adopt_sampler::_fmin_point,
            "fmin_linear",        &adopt_sampler::_fmin_linear,
            "fmin_aniso",         &adopt_sampler::_fmin_aniso,
            "fmip_none",          &adopt_sampler::_fmip_none,
            "fmip_point",         &adopt_sampler::_fmip_point,
            "fmip_linear",        &adopt_sampler::_fmip_linear,
            "fmag_none",          &adopt_sampler::_fmag_none,
            "fmag_point",         &adopt_sampler::_fmag_point,
            "fmag_linear",        &adopt_sampler::_fmag_linear
        );

        lua.new_usertype<adopt_compiler>("_compiler",
            sol::constructors<adopt_compiler(const adopt_compiler&)>(),
            "begin",              &adopt_compiler::_pass,
            "sorting",            &adopt_compiler::_options,
            "emissive",           &adopt_compiler::_o_emissive,
            "distort",            &adopt_compiler::_o_distort,
            "wmark",              &adopt_compiler::_o_wmark,
            "fog",                &adopt_compiler::_fog,
            "zb",                 &adopt_compiler::_ZB,
            "blend",              &adopt_compiler::_blend,
            "aref",               &adopt_compiler::_aref,
            "color_write_enable", &adopt_compiler::_color_write_enable,
            "sampler",            &adopt_compiler::_sampler // returns sampler-object
        );

        lua.new_enum("blend",
            "zero",               D3DBLEND_ZERO,
            "one",                D3DBLEND_ONE,
            "srccolor",           D3DBLEND_SRCCOLOR,
            "invsrccolor",        D3DBLEND_INVSRCCOLOR,
            "srcalpha",           D3DBLEND_SRCALPHA,
            "invsrcalpha",        D3DBLEND_INVSRCALPHA,
            "destalpha",          D3DBLEND_DESTALPHA,
            "invdestalpha",       D3DBLEND_INVDESTALPHA,
            "destcolor",          D3DBLEND_DESTCOLOR,
            "invdestcolor",       D3DBLEND_INVDESTCOLOR,
            "srcalphasat",        D3DBLEND_SRCALPHASAT
        );
    };
    ScriptEngine.init(exporterFunc, false);
    // load shaders
    const char* shaderPath = RImplementation.getShaderPath();
    xr_vector<char*>* folder = FS.file_list_open("$game_shaders$", shaderPath, FS_ListFiles | FS_RootOnly);
    VERIFY(folder);
    for (u32 it = 0; it < folder->size(); it++)
    {
        string_path namesp, fn;
        xr_strcpy(namesp, (*folder)[it]);
        if (nullptr == strext(namesp) || 0 != xr_strcmp(strext(namesp), ".s"))
            continue;
        *strext(namesp) = 0;
        if (0 == namesp[0])
            xr_strcpy(namesp, ScriptEngine.GlobalNamespace);
        strconcat(sizeof(fn), fn, shaderPath, (*folder)[it]);
        FS.update_path(fn, "$game_shaders$", fn);
        ScriptEngine.load_file_into_namespace(fn, namesp);
    }
    FS.file_list_close(folder);
}

void CResourceManager::LS_Unload() { ScriptEngine.unload(); }
BOOL CResourceManager::_lua_HasShader(LPCSTR s_shader)
{
    string256 undercorated;
    for (int i = 0, l = xr_strlen(s_shader) + 1; i < l; i++)
        undercorated[i] = ('\\' == s_shader[i]) ? '_' : s_shader[i];

#ifdef _EDITOR
    return ScriptEngine.object(undercorated, "editor", LUA_TFUNCTION);
#else
    return ScriptEngine.object(undercorated, "normal", LUA_TFUNCTION) ||
        ScriptEngine.object(undercorated, "l_special", LUA_TFUNCTION);
#endif
}

Shader* CResourceManager::_lua_Create(LPCSTR d_shader, LPCSTR s_textures)
{
    CBlender_Compile C;
    Shader S;

    // undecorate
    string256 undercorated;
    for (int i = 0, l = xr_strlen(d_shader) + 1; i < l; i++)
        undercorated[i] = ('\\' == d_shader[i]) ? '_' : d_shader[i];
    LPCSTR s_shader = undercorated;

    // Access to template
    C.BT = nullptr;
    C.bEditor = FALSE;
    C.bDetail = FALSE;

    // Prepare
    _ParseList(C.L_textures, s_textures);
    C.detail_texture = nullptr;
    C.detail_scaler = nullptr;

    // Compile element (LOD0 - HQ)
    if (ScriptEngine.object(s_shader, "normal_hq", LUA_TFUNCTION))
    {
        // Analyze possibility to detail this shader
        C.iElement = SE_R1_NORMAL_HQ;
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        S.E[C.iElement] = C._lua_Compile(s_shader, C.bDetail ? "normal_hq" : "normal");
    }
    else
    {
        if (ScriptEngine.object(s_shader, "normal", LUA_TFUNCTION))
        {
            C.iElement = SE_R1_NORMAL_HQ;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
            S.E[C.iElement] = C._lua_Compile(s_shader, "normal");
        }
    }

    // Compile element (LOD1)
    if (ScriptEngine.object(s_shader, "normal", LUA_TFUNCTION))
    {
        C.iElement = SE_R1_NORMAL_LQ;
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        S.E[C.iElement] = C._lua_Compile(s_shader, "normal");
    }

    // Compile element
    if (ScriptEngine.object(s_shader, "l_point", LUA_TFUNCTION))
    {
        C.iElement = SE_R1_LPOINT;
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        S.E[C.iElement] = C._lua_Compile(s_shader, "l_point");
    }

    // Compile element
    if (ScriptEngine.object(s_shader, "l_spot", LUA_TFUNCTION))
    {
        C.iElement = SE_R1_LSPOT;
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        S.E[C.iElement] = C._lua_Compile(s_shader, "l_spot");
    }

    // Compile element
    if (ScriptEngine.object(s_shader, "l_special", LUA_TFUNCTION))
    {
        C.iElement = SE_R1_LMODELS;
        C.bDetail = FALSE;
        S.E[C.iElement] = C._lua_Compile(s_shader, "l_special");
    }

    // Search equal in shaders array
    for (u32 it = 0; it < v_shaders.size(); it++)
        if (S.equal(v_shaders[it]))
            return v_shaders[it];

    // Create _new_ entry
    Shader* N = new Shader(S);
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_shaders.push_back(N);
    return N;
}

ShaderElement* CBlender_Compile::_lua_Compile(LPCSTR namesp, LPCSTR name)
{
    ShaderElement E;
    SH = &E;
    RS.Invalidate();

    // Compile
    LPCSTR t_0 = *L_textures[0] ? *L_textures[0] : "null";
    LPCSTR t_1 = (L_textures.size() > 1) ? *L_textures[1] : "null";
    LPCSTR t_d = detail_texture ? detail_texture : "null";
    sol::table shader = RImplementation.Resources->ScriptEngine.name_space2(namesp);
    const sol::function element = shader[name];
    adopt_compiler ac = adopt_compiler(this);
    element(ac, t_0, t_1, t_d);
    r_End();
    ShaderElement* _r = RImplementation.Resources->_CreateElement(E);
    return _r;
}

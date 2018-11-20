////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_property_script.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world property script export
////////////////////////////////////////////////////////////////////////////

#include "pch.hpp"
#include "script_world_property.h"
#include "operator_abstract.h"
#include "xrScriptEngine/ScriptExporter.hpp"

void CScriptWorldPropertyScriptExport(lua_State* luaState)
{
    sol::state_view lua(luaState);

    lua.new_usertype<CScriptWorldProperty>("world_property",
        sol::constructors<
            CScriptWorldProperty(CScriptWorldProperty::condition_type, CScriptWorldProperty::value_type)
        >(),
        "condition", &CScriptWorldProperty::condition,
        "value",     &CScriptWorldProperty::value
    );
}
SCRIPT_EXPORT_FUNC(CScriptWorldProperty, (), CScriptWorldPropertyScriptExport);

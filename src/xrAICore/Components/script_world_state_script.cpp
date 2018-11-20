////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_state_script.cpp
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world state script export
////////////////////////////////////////////////////////////////////////////

#include "pch.hpp"
#include "script_world_state.h"
#include "condition_state.h"
#include "xrScriptEngine/ScriptExporter.hpp"

void CScriptWorldStateScriptExport(lua_State* luaState)
{
    sol::state_view lua(luaState);

    lua.new_usertype<CScriptWorldState>("world_state",
        sol::constructors<
            CScriptWorldState(),
            CScriptWorldState(CScriptWorldState)
        >(),

        "add_property", (void (CScriptWorldState::*)(const CScriptWorldState::COperatorCondition&))
            &CScriptWorldState::add_condition,
        "remove_property",
        (void (CScriptWorldState::*)(const CScriptWorldState::COperatorCondition::condition_type&))
            &CScriptWorldState::remove_condition,

        "clear",    &CScriptWorldState::clear,
        "includes", &CScriptWorldState::includes,
        "property", &CScriptWorldState::property
    );
}
SCRIPT_EXPORT_FUNC(CScriptWorldState, (), CScriptWorldStateScriptExport);

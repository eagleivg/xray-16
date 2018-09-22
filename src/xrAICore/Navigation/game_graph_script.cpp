////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph class script export
////////////////////////////////////////////////////////////////////////////

#include "pch.hpp"
#include "game_graph.h"
#include "xrScriptEngine/ScriptExporter.hpp"
#include "xrScriptEngine/DebugMacros.hpp" // for THROW // XXX: move debug macros to xrCore
#include "AISpaceBase.hpp"

const CGameGraph* get_game_graph() { return &GEnv.AISpace->game_graph(); }
const CGameGraph::CHeader* get_header(const CGameGraph* self_) { return (&self_->header()); }
bool get_accessible1(const CGameGraph* self_, const u32& vertex_id) { return (self_->accessible(vertex_id)); }
void get_accessible2(const CGameGraph* self_, const u32& vertex_id, bool value) { self_->accessible(vertex_id, value); }
Fvector CVertex__level_point(const CGameGraph::CVertex* vertex)
{
    THROW(vertex);
    return (vertex->level_point());
}

Fvector CVertex__game_point(const CGameGraph::CVertex* vertex)
{
    THROW(vertex);
    return (vertex->game_point());
}

GameGraph::LEVEL_MAP const& get_levels(CGameGraph const* graph)
{
    THROW(graph);
    return graph->header().levels();
}

// Disable all sol2 automagicalness
namespace sol
{
template <>
struct is_automagical<GameGraph::LEVEL_MAP::value_type> : std::false_type {};
}

struct LevelMapIteratorState
{
    using it_t = GameGraph::LEVEL_MAP::const_iterator;
    it_t it;
    it_t last;

    LevelMapIteratorState(const GameGraph::LEVEL_MAP& map) : it(map.begin()), last(map.end()) {}
};

// XXX: Remove wrapper, use sol3 when it will be released.
struct LevelMapWrapper
{
    const GameGraph::_LEVEL_ID& id;
    const GameGraph::SLevel& level;

    auto get_id() const { return id; }
    auto get_level() const { return level; }
};

auto LevelMapNext(sol::user<LevelMapIteratorState&> user_it_state, const sol::this_state l)
{
    LevelMapIteratorState& it_state = user_it_state;
    auto& it = it_state.it;

    if (it == it_state.last)
        return sol::object(sol::lua_nil);

    auto r = sol::object(l, sol::in_place, LevelMapWrapper{ it->first, it->second });
    std::advance(it, 1);
    return r;
}

auto LevelMapPairs(const CGameGraph& graph)
{
    return std::make_tuple(
        &LevelMapNext,
        sol::user<LevelMapIteratorState>(graph.header().levels()),
        sol::lua_nil
    );
}

void CGameGraphScriptExport(lua_State* luaState)
{
    sol::state_view lua(luaState);

    using LevelMap = GameGraph::LEVEL_MAP::value_type;
    lua.new_usertype<LevelMap>("GameGraph__LEVEL_MAP__value_type",
        "id",    sol::readonly_property(&LevelMap::first),
        "level", sol::readonly_property(&LevelMap::second)
    );

    lua.new_usertype<LevelMapWrapper>("GameGraph__LEVEL_MAP__wrapper",
        "id",    sol::readonly_property(&LevelMapWrapper::get_id),
        "level", sol::readonly_property(&LevelMapWrapper::get_level)
    );

    lua.new_usertype<CGameGraph>("CGameGraph",
        "accessible",      sol::overload(
                           &get_accessible1,
                           &get_accessible2),
        "valid_vertex_id", &CGameGraph::valid_vertex_id,
        "vertex",          &CGameGraph::vertex,
        "vertex_id",       &CGameGraph::vertex_id,
        //"levels",          &get_levels
        "levels",          &LevelMapPairs
    );

    lua.set_function("game_graph", &get_game_graph);

    using CVertex = CGameGraph::CVertex;
    lua.new_usertype<CVertex>("GameGraph__CVertex",
        "level_point",     &CVertex__level_point,
        "game_point",      &CVertex__game_point,
        "level_id",        &CVertex::level_id,
        "level_vertex_id", &CVertex::level_vertex_id
    );
}
SCRIPT_EXPORT_FUNC(CGameGraph, (), CGameGraphScriptExport);

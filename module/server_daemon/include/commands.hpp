#pragma once

#include <arepa/command/CommandMap.hpp>
#include <arepa/command/CommandName.hpp>
#include <arepa/command/Executor.hpp>
#include <arepa/game/Room.hpp>
#include <arepa/game/User.hpp>
#include <arepa/server/Connection.hpp>
#include <arepa/server/Server.hpp>

#include <GameManager.hpp>
#include <unordered_map>

using GlobalCommandMap = arepa::command::CommandMap<arepa::server::Connection&>;
using RoomCommandMap = arepa::game::Room::CommandMap;

extern GlobalCommandMap GLOBAL_COMMAND_MAP;

void init_global_commands(GlobalCommandMap& map, GameManager& manager, arepa::server::Server&);
void init_room_commands(RoomCommandMap& map, GameManager& manager);

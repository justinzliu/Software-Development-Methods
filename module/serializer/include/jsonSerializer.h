#pragma once

#include "arepa/game_spec/GameSpecification.h"
#include "arepa/server_config/Config.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>


namespace jsonSerializer {

using json = nlohmann::json;

serverConfig::Configuration
parseServerConfig(const json&);

gameSpecification::Specification
parseGameSpecification(const json&);

gameSpecification::Configuration
parseConfig(const json&);

gameSpecification::Setup
parseSetup(const json&);

gameSpecification::SetupValue
parseSetupValue(const json&);

gameSpecification::CustomSetup
parseCustomSetup(const json&);

gameSpecification::DataKind
parseDataKind(const json&);

GameState
parseGameState(const json&);

Environment
parseEnvironment(const json&);

Expression
parseExpression(const json&);

Expression
parseExpMap(const json&);

Expression
parseExpList(const json&);

rules::RuleList
parseRules(const json&);

bool isValidServerConfig(const json&);

bool isValidGameSpec(const json&);

bool hasAllRequiredFields(const json&);

bool hasNoExtraFields(const json&);



// enum FIELDS {
//     CONF,
//     NAME,
//     PLYRCOUNT,
//     AUDI,
//     SETUP,
//     CONST,
//     VAR,
//     PPLYR,
//     PAUDI,
//     RULES
// };

// const std::unordered_map<FIELDS, std::string> enum_to_str({
//     {CONF,      "/configuration"},
//     {NAME,      "/configuration/name"},
//     {PLYRCOUNT, "/configuration/player count"},
//     {AUDI,      "/configuration/audience"},
//     {SETUP,     "/configuration/setup"},
//     {CONST,     "/constants"},
//     {VAR,       "/variables"},
//     {PPLYR,     "/per-player"},
//     {PAUDI,     "/per-audience"},
//     {RULES,     "/rules"}
// });


}    // namespace jsonSerializer

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "jsonconfig.h"

using json = nlohmann::json;

//json_game spec
using namespace json_serializer;
using namespace gameSpecification;

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

gameSpecification::GameState
parseGameState(const json&);

gameSpecification::Environment
parseEnvironment(const json&);

gameSpecification::Expression
parseExpression(const json&);

gameSpecification::Expression
parseExpMap(const json&);

gameSpecification::Expression
parseExpList(const json&);

gameSpecification::rules::RuleList
parseRules(const json&);

bool 
hasAllRequiredFields(const json&);

bool 
hasNoExtraFields(const json&);

bool 
hasValidFields(const json&);

enum FIELDS {
    CONF,
    NAME,
    PLYRCOUNT,
    AUDI,
    SETUP,
    CONST,
    VAR,
    PPLYR,
    PAUDI,
    RULES
};

const std::unordered_map<FIELDS, std::string> enum_to_str({
    {CONF,      "/configuration"},
    {NAME,      "/configuration/name"},
    {PLYRCOUNT, "/configuration/player count"},
    {AUDI,      "/configuration/audience"},
    {SETUP,     "/configuration/setup"},
    {CONST,     "/constants"},
    {VAR,       "/variables"},
    {PPLYR,     "/per-player"},
    {PAUDI,     "/per-audience"},
    {RULES,     "/rules"}
});
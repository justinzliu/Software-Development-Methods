/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "GameManager.h"
#include "arepa/game_spec/GameSpecification.h"
#include "arepa/serializer/jsonSerializer.h"
#include "arepa/server_config/Config.h"
#include "arepa/game_spec/Rules.h"
#include "command.h"

#include <arepa/server/Server.h>

#include <boost/uuid/uuid_io.hpp>

//#include "dsl_interpreter.h"
//#include "json_parser.h"
#include <atomic>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using networking::ConnectionId;
using networking::Message;
using namespace commandSpace;
using networking::Server;


typedef networking::ConnectionId UniqueConnectionID;
typedef messageReturn<UniqueConnectionID> messageReturnAlias;
typedef GameManager<UniqueConnectionID> GameManagerAlias;

std::atomic<std::uint64_t> unique_connection_id_counter = 0;

//std::vector<Connection> clients;

std::string default_json = "templates/server/default.json";

GameManagerAlias game_manager;


//messages returned from game manager
std::deque<messageReturnAlias> gameMessageQueue;

//messages ready to be sent to networking
std::deque<Message> networkMessageQueue;


/*
transform game messages in game message queue to network messages in
network message queue, then empty the game message queue. These act like queues,
sof first game message processed is first to be put into the network queue.
*/
bool gameMessagesToNetworkMessages() {
    bool quit = false;
    while (!gameMessageQueue.empty()) {

        messageReturnAlias message = gameMessageQueue.front();
        gameMessageQueue.pop_front();
        std::string log = message.message;

        if (message.shouldShutdown) {
            quit = true;
        }

        UniqueConnectionID player = message.sendTo;
        std::cout << boost::uuids::to_string(player.uuid) << "\n";
        std::cout << log << "\n";
        networkMessageQueue.push_back({ player, log });
    }
    return quit;
}


//message types possible (tentative)
enum MessageType { COMMAND,
    GAME_CONFIG,
    NORMAL };


MessageType getMessageType(const std::string& _message) {
    if (_message[0] == '/') {    //it is a command
        return MessageType::COMMAND;
    } else {    //it is a regular message
        return MessageType::NORMAL;
    }
}


/*
* Interpret command and call appropriate game_manager api function (there might be a better way to do this)
*/
std::vector<messageReturnAlias> parseCommandAndCollectResponse(const std::string& message, UniqueConnectionID id) {

    std::vector<messageReturnAlias> game_manager_message;

    //tokenize the string, split by ' '
    std::vector<std::string> tokens;
    std::istringstream iss(message);
    std::copy(std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>(),
        std::back_inserter(tokens));

    //command class to translate to enumerator
    Command command(tokens[0]);
    switch (command.getCommandType()) {
    case commandType::listMember:
        game_manager_message = game_manager.returnRoomMembersCommand(id);
        break;
    case commandType::listRoom:
        game_manager_message = game_manager.returnRoomCommand(id);
        break;
    case commandType::createRoom:
        game_manager_message = game_manager.createRoomCommand(id);
        break;
    case commandType::joinRoom:
        game_manager_message = game_manager.joinRoomCommand(id, tokens[1]);
        break;
    case commandType::kickUser:
        game_manager_message = game_manager.kickPlayerCommand(id, tokens[1]);
        break;
    case commandType::quitFromServer:
        game_manager_message = game_manager.leaveRoomCommand(id);
        break;
    case commandType::initGame:
        game_manager_message = game_manager.initRoomCommand(id);
        break;
    case commandType::clear:
        game_manager_message = game_manager.clearCommand(id);
        break;
    case commandType::shutdownServer:

        break;
    };

    return game_manager_message;
}


//function to format a message object to a particular client
std::deque<Message> sendToClient(const ConnectionId& client, const std::string& log) {
    std::deque<Message> outgoing;
    outgoing.push_back({ client, log });
    return outgoing;
}

//Whenever a client connections, this function is called it inserts connection and connection id into map

void onConnect(shared_ptr<Connection> c) {
    auto id = ++unique_connection_id_counter;

    std::cout << "New connection found: " << c->session_token() << "\n";

    //override game manager and send back server welcome message...
    networkMessageQueue.emplace_back(c->session_id(), "Welcome to the server! Enter a command...");
    //put player in a room
    //game_manager.createRoomCommand();
}

//Called whenenver a client disconnects. Should handle disconneting player from game room
void onDisconnect(shared_ptr<Connection> c) {
    std::cout << "Connection lost: " << c->session_token() << "\n";
}


void processMessages(Server& server, const std::deque<Message>& incoming) {
    for (auto& message : incoming) {
        ConnectionId sentFrom = message.connection;
        Command command(message.text);
        commandType recieved = command.getCommandType();
        //variable type input is type alias to string defined in command.h
        std::vector<input> tokens = command.getTokens();
        if (recieved == commandType::message) {
            std::vector<messageReturnAlias> game_messages = game_manager.handleGameMessage(message.text, sentFrom.uuid);
            for (auto game_message : game_messages) {
                gameMessageQueue.push_back(game_message);
            }
        } else {
            //handle command in game manager
            std::vector<messageReturnAlias> cmd_messages;
            switch (recieved) {
            case commandType::listMember:
                cmd_messages = game_manager.returnRoomMembersCommand(sentFrom.uuid);
                break;
            case commandType::listRoom:
                cmd_messages = game_manager.returnRoomCommand(sentFrom.uuid);
                break;
            case commandType::createRoom:
                cmd_messages = game_manager.createRoomCommand(sentFrom.uuid);
                break;
            case commandType::joinRoom:
                cmd_messages = game_manager.joinRoomCommand(sentFrom.uuid, tokens[1]);
                break;
            case commandType::kickUser:
                cmd_messages = game_manager.kickPlayerCommand(sentFrom.uuid, tokens[1]);
                break;
            case commandType::clear:
                cmd_messages = game_manager.clearCommand(sentFrom.uuid);
                break;
            case commandType::quitFromServer:
                cmd_messages = game_manager.leaveRoomCommand(sentFrom.uuid);
                break;
            case commandType::initGame:
                cmd_messages = game_manager.initRoomCommand(sentFrom.uuid);
                break;
            case commandType::shutdownServer:
                cmd_messages = game_manager.shutdownServerCommand(sentFrom.uuid);
                break;
            };
            //create message vector to send out
            for (auto cmd_message : cmd_messages) {
                gameMessageQueue.push_back(cmd_message);
            }
        }
    };
}


/*
std::deque<Message> buildOutgoing(const std::string& log) {
  std::deque<Message> outgoing;
  for (auto client : clients) {
    outgoing.push_back({client, log});
  }
  return outgoing;
}
*/

using json = nlohmann::json;

serverConfig::Configuration
loadJSONConfigFile(const std::string& filepath) {
    serverConfig::Configuration config;
    std::ifstream s(filepath);
    if (s.fail()) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return config;
    }

    nlohmann::json jsonServerConfig;
    try {
        jsonServerConfig = json::parse(s);
    } catch (json::parse_error& e) {
        std::cerr << "Invalid JSON server config\n in jsonspec.cpp\n"
                  << "message: " << e.what();
        return config;
    }

    config = jsonSerializer::parseServerConfig(jsonServerConfig);
    config.err = false;



    return config;
}


int main(int argc, char* argv[]) {
    nlohmann::json jsonFile;
    std::ifstream s("/home/at/Downloads/social-gaming/templates/game/simplified.json");
    jsonFile = json::parse(s);
    jsonSerializer::parseRules(jsonFile);
    /* at place this here
    serverConfig::Configuration server_config;

    if (argc < 2) {
        server_config = loadJSONConfigFile(default_json);
    } else {
        server_config = loadJSONConfigFile(argv[1]);
    }

    if (server_config.err) {
        return -1;
    }
 at*/
    //example
    //g_config game = game_config::load_file("templates/game/rps.json", true);
    //std::cout << game.player_count["min"] << std::endl;

    /*
  Try to configurate game_manager... with custom error handling to give useful
  error messages?
  */
    //at try {
        //game_manager.setUp(server_config);
    //at } catch (... /*const GameManagerException& e*/) {
    //at    std::cerr << "Server configuration failed";
        //<< e.what() << '\n'
        //<< e.where() << '\n,
   //at }
   /*at
    unsigned short port = server_config.port;
    arepa::networking::websocket::Options opts;
    opts.bind_port = port;

    Server server(opts, &onConnect, &onDisconnect);
 at*/
    /*
  * Main Game Server Loop
  */
/*at
    while (true) {
        bool shouldQuit = false;


        //Get all messages.
        auto incoming = server.receive();

        //Process messages and put them in gameMessagesQueue (global queue)
        processMessages(server, incoming);

        //if an admin runs a comman to shutdown server, shouldQuit will be set to true
        shouldQuit = gameMessagesToNetworkMessages();
        server.send(networkMessageQueue);
        networkMessageQueue.clear();

        if (shouldQuit) {
            break;
        }

        // Sleep to not burn out the CPU.
        constexpr int ONE_HUNDRED_MILLISECONDS = 1000 * 100;
        usleep(ONE_HUNDRED_MILLISECONDS);
    }at*/
    return 0;
}

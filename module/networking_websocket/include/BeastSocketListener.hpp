#pragma once

#include "BeastContext.hpp"
#include "BeastSocket.hpp"

#include <arepa/communication/Signal.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <memory>

namespace arepa::networking::websocket {

/**
 * A class that binds a TCP endpoint and accepts websockets.
 * This uses boost::asio and boost::beast to handle the underlying I/O opertaions.
 */
class BeastSocketListener : public std::enable_shared_from_this<BeastSocketListener> {

#pragma mark - Fields -
private:
    BeastContext& _context;
    boost::asio::ip::tcp::acceptor _acceptor;


#pragma mark - Signals -
public:
    /**
     * A signal for when a new socket connection failed to be established.
     */
    arepa::communication::Signal<arepa::networking::NetworkException> on_error;

    /**
     * A signal for when a new socket connection is established.
     */
    arepa::communication::Signal<std::shared_ptr<arepa::networking::Socket>> on_accept;


#pragma mark - Methods (Private) -
private:
    void _on_async_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
    void _on_async_accept_websocket(std::shared_ptr<BeastSocket::BeastSocketConnection> socket, boost::beast::error_code ec);
    void _do_async_accept();


#pragma mark - Constructors -
public:
    explicit BeastSocketListener(BeastContext& context) noexcept(false);

#pragma mark - Methods -
public:
    /**
     * Starts listening for socket connections.
     */
    void start() noexcept(false);

    /**
     * Stops listening for socket connections.
     */
    void stop() noexcept(false);
};

}

#include "listen.hpp"
#include <boost/beast/core.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>
#include "target_source.hpp"
#include "session.hpp"
#include "error_message.hpp"

namespace video_proxy {

  void listen(
    data& proxy_data ,
    boost::asio::yield_context& yield) {

    boost::system::error_code error_code;
    data::proxy_server_data& server_data = proxy_data.m_proxy_server_data;

    // To receive the connection
    boost::asio::ip::tcp::acceptor acceptor{
      server_data.m_io_context, 
      {server_data.m_address, server_data.m_port}};

    // Start listening for connections
    acceptor.listen(boost::asio::socket_base::max_listen_connections, error_code);
    if (error_code) {
      error_message(error_code);
    }
    else {
      while (true) {
        // To accept the connection
        boost::asio::ip::tcp::socket socket{ server_data.m_io_context };
        acceptor.async_accept(socket, yield[error_code]);

        if(!error_code){
          // Read and execute request
          boost::asio::spawn(server_data.m_io_context,
            [&socket, &proxy_data, &error_code] (boost::asio::yield_context yield) {
              session( std::move(socket), proxy_data, yield);
                if (error_code) {
                  error_message(error_code);
                }
            });
        }
        else {
          error_message(error_code);
        }
      }
    }
  }
}



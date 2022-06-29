#pragma once
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include "listen.hpp"
#include "parser.hpp"
#include "video_proxy_data.hpp"

namespace video_proxy {

  void session(
    boost::asio::ip::tcp::socket socket,
    data& proxy_data,
    boost::asio::yield_context& yield);
}



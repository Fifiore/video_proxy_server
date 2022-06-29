#pragma once
#include <boost/asio/spawn.hpp>
#include "video_proxy_data.hpp"

namespace video_proxy {

  void listen(
    data& proxy_data,
    boost::asio::yield_context& yield);
}



#pragma once
#include <boost/system/detail/error_code.hpp>
#include <iostream>

namespace video_proxy {

  inline void error_message(const boost::system::error_code& error_code)
  {
    std::cerr << "Error: " << error_code.message() << '\n';
  }
}




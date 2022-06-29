#pragma once
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string_view>
#include <string>
#include "traces.hpp"


namespace video_proxy {
  
  struct data {

    struct proxy_server_data {
      boost::asio::ip::address m_address;
      unsigned short m_port = 0;
      std::string m_host_port;
      // io_context required for all I/O
      boost::asio::io_context m_io_context;

      explicit proxy_server_data(int number_of_threads) : m_io_context(number_of_threads) {}
    };
    struct target_data {
      bool m_use_secure_protocol = false;
      std::string m_protocol;
      std::string m_host;
      // io_context required for all I/O
      boost::asio::io_context m_io_context;
      boost::asio::ip::tcp::resolver::results_type m_endpoints;
    };

    proxy_server_data m_proxy_server_data;
    target_data m_target_data;
    video_proxy::traces m_traces;

    explicit data(int number_of_threads) : m_proxy_server_data(number_of_threads) {}
  };
}


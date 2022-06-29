#pragma once
#include <boost/beast/core.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <string>
#include "plain_stream.hpp"
#include "error_message.hpp"
#include "video_proxy_data.hpp"

namespace target_source {

  template<class Stream>
  inline bool execute_request(
    boost::beast::http::request<boost::beast::http::string_body> request,
    boost::beast::flat_buffer& buffer,
    boost::beast::http::response<boost::beast::http::string_body>& response,
    video_proxy::data::target_data& data) {

    boost::system::error_code error_code;
    boost::system::error_code close_error_code;

    Stream target_stream(&data.m_io_context, data.m_endpoints);

    request.set(boost::beast::http::field::host, data.m_host + ":" + target_stream.get_port());

    target_stream.connect(error_code);
    if (!error_code) {
      target_stream.handshake(error_code);
      if (!error_code) {
        target_stream.write(request, error_code);
        if (!error_code) {
          target_stream.read(buffer, response, error_code);
        }
      }
      target_stream.close(close_error_code);
    }
    if (error_code) {
      video_proxy::error_message(error_code);
    }
    if (close_error_code) {
      video_proxy::error_message(close_error_code);
    }
    return error_code || close_error_code;
  }
}
  



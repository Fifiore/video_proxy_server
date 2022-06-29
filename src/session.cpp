#include "session.hpp"
#include <boost/beast/core.hpp>
#include <boost/asio/io_service.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include "video_proxy_data.hpp"
#include "plain_stream.hpp"
#include "ssl_stream.hpp"
#include "error_message.hpp"
#include "target_source.hpp"

namespace video_proxy {

  void session(
    boost::asio::ip::tcp::socket socket,
    data& proxy_data,
    boost::asio::yield_context& yield) {

    boost::system::error_code error_code;
    data::proxy_server_data& server_data = proxy_data.m_proxy_server_data;
    data::target_data& target_data = proxy_data.m_target_data;

    while (true) {

      boost::beast::flat_buffer request_buffer;
      boost::beast::http::request<boost::beast::http::string_body> request;

      // Read request sent by the player
      boost::beast::http::async_read(socket, request_buffer, request, yield[error_code]);

      if (!error_code) {

        boost::beast::flat_buffer response_buffer;
        boost::beast::http::response<boost::beast::http::string_body> response;

        std::cout << proxy_data.m_traces.generate_in(request, target_data.m_protocol, target_data.m_host) << "\n";

        auto start = std::chrono::steady_clock::now();
        bool target_failure = false;

        // Tranfer the request to the target server and retrieve the response
        if (target_data.m_use_secure_protocol) {
          target_failure = target_source::execute_request<ssl_stream>(request, response_buffer, response, target_data);
        }
        else {
          target_failure = target_source::execute_request<plain_stream>(request, response_buffer, response, target_data);
        }

        if (!target_failure) {

          auto end = std::chrono::steady_clock::now();
          auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

          std::cout << proxy_data.m_traces.generate_out(request, target_data.m_protocol, target_data.m_host, duration) << "\n";

          if (parser::get_file_type(request) == parser::file_type::manifest) {
            // Adapt the response: absolute URLs are rerooted to this server, relative URLs remain unchanged
            response = parser::generate_manifest_response(response, target_data.m_protocol, server_data.m_host_port);
          }
          // Transfer back the server response to the player
          boost::beast::http::async_write(socket, response, yield[error_code]);

          if (request.need_eof()) {
            break;
          }
        }
        else {
          break;
        }
      }
    }
    if (error_code) {
      error_message(error_code);
    }

    // We want to keep the first error
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error_code);
    if (error_code) {
      error_message(error_code);
    }
  }
}



#include <boost/beast/core.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>
#include <string>
#include "listen.hpp"
#include "video_proxy_data.hpp"

boost::asio::ip::tcp::resolver::results_type get_source_endpoints(
  const std::string& protocol, 
  const std::string& host,
  boost::asio::io_context& context) {

  boost::asio::ip::tcp::resolver resolver(context);
  boost::asio::ip::tcp::resolver::query query(host, protocol);

  boost::system::error_code error_code;
  auto endpoints = resolver.resolve(query, error_code);
  if (error_code) {
    throw std::invalid_argument("Unknown host");
  }
  return endpoints;
}

bool is_secure_protocol(const std::string& protocol) {
  if ("https" == protocol) {
    return true;
  }
  if ("http" == protocol) {
    return false;
  }
  throw std::invalid_argument("Invalid protocol: expecting http or https");
}

int main(int argc, char* argv[]) {
 
  try
  {
    if (argc != 4) {
      std::cerr << "Usage: video_proxy_server <port> <protocol> <host>\n" <<
        "Example:\n" <<
        "\tvideo_proxy_server 8080 https www.videos_domain.com";
      return EXIT_FAILURE;
    }

    const int number_of_threads = 1;

    video_proxy::data proxy_data(number_of_threads);

    // Local server only
    proxy_data.m_proxy_server_data.m_address = boost::asio::ip::make_address("0.0.0.0");
    proxy_data.m_proxy_server_data.m_host_port = "localhost:" + std::string(argv[1]);
    proxy_data.m_proxy_server_data.m_port = static_cast<unsigned short>(std::atoi(argv[1]));
    proxy_data.m_target_data.m_protocol = argv[2];
    proxy_data.m_target_data.m_host = argv[3];
    proxy_data.m_target_data.m_use_secure_protocol = is_secure_protocol(proxy_data.m_target_data.m_protocol);

    // Retrieve endpoints here to check if the video server name is valid
    proxy_data.m_target_data.m_endpoints = get_source_endpoints(
      proxy_data.m_target_data.m_protocol, 
      proxy_data.m_target_data.m_host, 
      proxy_data.m_target_data.m_io_context);

    // Listen connections
    boost::asio::spawn(proxy_data.m_proxy_server_data.m_io_context,
      [&proxy_data](boost::asio::yield_context yield) {
        video_proxy::listen(proxy_data, yield);
      });

    // The call will return when operation is complete
    proxy_data.m_proxy_server_data.m_io_context.run();
  }
  catch (std::invalid_argument const& e) {
    std::cerr << "Invalid argument: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
}

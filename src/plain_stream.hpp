#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>

namespace video_proxy {
  class plain_stream {
  public:
    plain_stream(
      boost::asio::io_context* context,
      const boost::asio::ip::tcp::resolver::results_type& endpoints);

    void connect(boost::system::error_code& error_code);

    void close(boost::system::error_code&);

    void handshake(boost::system::error_code&);

    void read(
      boost::beast::flat_buffer& buffer,
      boost::beast::http::response<boost::beast::http::string_body>& response,
      boost::system::error_code& error_code);

    void write(
      boost::beast::http::request<boost::beast::http::string_body>& request,
      boost::system::error_code& error_code);

    std::string get_port() const;
    std::string get_protocol() const;

  private:
    boost::asio::io_context* m_context;
    boost::asio::ip::tcp::resolver::results_type m_endpoints;
    boost::beast::tcp_stream m_stream;
  };
}



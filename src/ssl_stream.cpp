#include "ssl_stream.hpp"

namespace video_proxy {

  ssl_stream::ssl_stream(
    boost::asio::io_context* context, 
    const boost::asio::ip::tcp::resolver::results_type& endpoints) :
    m_context(context),
    m_ssl_context(boost::asio::ssl::context::tlsv12_client), 
    m_endpoints(endpoints), 
    m_stream(*m_context, m_ssl_context) {}

  void ssl_stream::connect(boost::system::error_code& error_code) {
    boost::beast::get_lowest_layer(m_stream).connect(m_endpoints, error_code);
  }

  void ssl_stream::close(boost::system::error_code& error_code) {
    m_stream.shutdown(error_code);
  }

  void ssl_stream::handshake(boost::system::error_code& error_code) {
    m_stream.handshake(boost::asio::ssl::stream_base::client, error_code);
  }

  void ssl_stream::read(
    boost::beast::flat_buffer& buffer, 
    boost::beast::http::response<boost::beast::http::string_body>& response,
    boost::system::error_code& error_code) {
    boost::beast::http::read(m_stream, buffer, response, error_code);
  }

  void ssl_stream::write(
    boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::system::error_code& error_code) {
    boost::beast::http::write(m_stream, request, error_code);
  }

  std::string ssl_stream::get_port() const {
    return "443";
  }

  std::string ssl_stream::get_protocol() const {
    return "https";
  }


}



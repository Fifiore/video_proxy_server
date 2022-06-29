#include "plain_stream.hpp"
#include <boost/asio/connect.hpp>

namespace video_proxy {

  plain_stream::plain_stream(
    boost::asio::io_context* context, 
    const boost::asio::ip::tcp::resolver::results_type& endpoints) :
    m_context(context), 
    m_endpoints(endpoints), 
    m_stream(*context) {}

  void plain_stream::connect(boost::system::error_code& error_code) {
    m_stream.connect(m_endpoints, error_code);
  }

  void plain_stream::close(boost::system::error_code&) {
    m_stream.close();
  }

  void plain_stream::handshake(boost::system::error_code&) {
  }

  void plain_stream::read(
    boost::beast::flat_buffer& buffer, 
    boost::beast::http::response<boost::beast::http::string_body>& response,
    boost::system::error_code& error_code) {
    boost::beast::http::read(m_stream, buffer, response, error_code);
  }

  void plain_stream::write(
    boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::system::error_code& error_code) {
    boost::beast::http::write(m_stream, request, error_code);
  }

  std::string plain_stream::get_port() const {
    return "80";
  }

  std::string plain_stream::get_protocol() const {
    return "http";
  }
}



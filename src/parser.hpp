#pragma once
#include <boost/beast/http.hpp>
#include <string>

namespace parser {

  enum class file_type {
    manifest,
    segment,
    segment_template,
    unknown
  };

  file_type get_file_type(
    const boost::beast::http::request<boost::beast::http::string_body>& request);

  std::string get_file_type_string(
    const boost::beast::http::request<boost::beast::http::string_body>& request);

  std::string build_absolute_url(
    const std::string& protocol,
    const std::string& host,
    const std::string& target);

  // Build a copy reponse with absolute urls re-rooted to the proxy server (if hls format)
  // other lines or other format are simply copied
  boost::beast::http::response<boost::beast::http::string_body>
    generate_manifest_response(
      const boost::beast::http::response<boost::beast::http::string_body>& response,
      const std::string& protocol,
      const std::string& proxy_host_port);
}



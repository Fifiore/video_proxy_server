#include "parser.hpp"
#include <string_view>

namespace parser {

  file_type get_file_type(
    const boost::beast::http::request<boost::beast::http::string_body>& request) {
    auto target = request.target();
    auto dotPos = target.rfind('.');
    if (dotPos + 1 < target.size()) {
      auto extension = std::string(target.substr(dotPos + 1));
      std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
      if ("m3u8" == extension || "mpd" == extension) {
        return file_type::manifest;
      }
      if ("ts" == extension || "m4s" == extension) {
        return file_type::segment;
      }
      if ("mp4" == extension) {
        return file_type::segment_template;
      }
    }
    return file_type::unknown;
  }

  std::string get_file_type_string(
    const boost::beast::http::request<boost::beast::http::string_body>& request) {
    auto type = get_file_type(request);
    switch (type)
    {
    case file_type::manifest:
      return "[MANIFEST]";
    case file_type::segment:
      return "[SEGMENT]";
    case file_type::segment_template:
      return "[SEGMENT TEMPLATE]";
    case file_type::unknown:
    default:
      return "[UNKNOWN]";
    }
  }

  std::string build_absolute_url(
    const std::string& protocol,
    const std::string& host,
    const std::string& target){
      std::string url =  protocol
        + "://"
        + host
        + target;
      return url;
  }

  static std::string generate_manifest_response_body(
    const boost::beast::http::response<boost::beast::http::string_body>& response,
    const std::string& protocol,
    const std::string& proxy_host_port) {

    std::stringstream input_body(response.body().data());
    std::string start = protocol + "://";
    std::stringstream result;
    std::string line;
    while (std::getline(input_body, line, '\n')) {
      if (!line.empty()) {
        // Comment or not an absolute path, we keep the line unchanged
        if (line[0] == '#' || line.rfind(start, 0) != 0) {
          result << line << '\n';
        }// Source host is replaced by proxy server host
        else if (line.rfind(start, 0) == 0) {
          std::string_view value = line;
          value = value.substr(start.size());
          auto h = value.find('/');
          value = value.substr(h);
          result << build_absolute_url(protocol, proxy_host_port, std::string(value)) << '\n';
        }
      }
    }
    return result.str();
  }

  boost::beast::http::response<boost::beast::http::string_body>
    generate_manifest_response(
    const boost::beast::http::response<boost::beast::http::string_body>& response, 
    const std::string& protocol, 
    const std::string& proxy_host_port) {

    boost::beast::http::response<boost::beast::http::string_body> 
      new_response{ response.result(), response.version()};

    for (auto& field : response.base()) {
      auto it = response.find(field.name());
      if (it != response.end()) {
        new_response.set(field.name(), it->value());
      }
    }
    new_response.body() = generate_manifest_response_body(response, protocol, proxy_host_port);
    new_response.prepare_payload();
    return new_response;
  }
}



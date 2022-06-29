#include "traces.hpp"
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "parser.hpp"

namespace video_proxy {

  bool traces::is_track_switch(const boost::beast::http::request<boost::beast::http::string_body>& request) {
    bool track_switch = false;
    auto target = request.target();
    auto type = parser::get_file_type(request);
    if (parser::file_type::manifest == type || parser::file_type::segment_template == type) {
      if (m_segment_requested) {
        track_switch = true;
      }
      m_segment_requested = false;
    }
    else if (parser::file_type::segment == type) {
      m_segment_requested = true;
    }
    return track_switch;
  }

  std::string traces::generate_in(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    const std::string& protocol,
    const std::string& host) {
    std::string result;
    if (is_track_switch(request)) {
      result = "[TRACK SWITCH]\n";
    }
    result += "[IN]"
      + parser::get_file_type_string(request)
      + " " 
      + parser::build_absolute_url(protocol, host, std::string(request.target()));

    return result;
  }

  std::string traces::generate_out(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    const std::string& protocol,
    const std::string& host, 
    std::chrono::milliseconds duration) {
    std::string result = "[OUT]"
      + parser::get_file_type_string(request)
      + std::string(" ")
      + parser::build_absolute_url(protocol, host, std::string(request.target()))
      + " ("
      + boost::lexical_cast<std::string>(duration.count())
      +"ms)";
    return result;
  }
}



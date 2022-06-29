#pragma once
#include <boost/beast/http.hpp>
#include <chrono>
#include <string>

namespace video_proxy {
  class traces {
  public:
    traces() = default;

    std::string generate_in(
      const boost::beast::http::request<boost::beast::http::string_body>& request,
      const std::string& protocol,
      const std::string& host);

    std::string generate_out(
      const boost::beast::http::request<boost::beast::http::string_body>& request,
      const std::string& protocol,
      const std::string& host,
      std::chrono::milliseconds duration);

  private:
    bool m_segment_requested = false;

    bool is_track_switch(const boost::beast::http::request<boost::beast::http::string_body>& request);
  };
}



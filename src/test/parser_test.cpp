#include <gtest/gtest.h>
#include "parser.hpp"
#include <boost/beast/http.hpp>
#include <string>
#include <sstream>

using Request = boost::beast::http::request<boost::beast::http::string_body>;
using Response = boost::beast::http::response<boost::beast::http::string_body>;

TEST(parser_test, get_file_type) {

  Request req1{ boost::beast::http::verb::get, "/my_super_video/file.m3u8", 11 };
  EXPECT_EQ(parser::get_file_type(req1), parser::file_type::manifest);

  Request req2{ boost::beast::http::verb::get, "/my_super_video2/other_file.mpd", 11 };
  EXPECT_EQ(parser::get_file_type(req2), parser::file_type::manifest);

  Request req3{ boost::beast::http::verb::get, "/my_super_video/file.ts", 11 };
  EXPECT_EQ(parser::get_file_type(req3), parser::file_type::segment);

  Request req4{ boost::beast::http::verb::get, "/my_super_video2/other_file.m4s", 11 };
  EXPECT_EQ(parser::get_file_type(req4), parser::file_type::segment);

  Request req_5{ boost::beast::http::verb::get, "/my_super_video2/other_file.mp4", 11 };
  EXPECT_EQ(parser::get_file_type(req_5), parser::file_type::segment_template);
}

TEST(parser_test, get_file_type_string) {
  Request req1{ boost::beast::http::verb::get, "/my_super_video/file.m3u8", 11 };
  EXPECT_EQ(parser::get_file_type_string(req1), "[MANIFEST]");

  Request req2{ boost::beast::http::verb::get, "/my_super_video2/other_file.mpd", 11 };
  EXPECT_EQ(parser::get_file_type_string(req2), "[MANIFEST]");

  Request req3{ boost::beast::http::verb::get, "/my_super_video/file.ts", 11 };
  EXPECT_EQ(parser::get_file_type_string(req3), "[SEGMENT]");

  Request req4{ boost::beast::http::verb::get, "/my_super_video2/other_file.m4s", 11 };
  EXPECT_EQ(parser::get_file_type_string(req4), "[SEGMENT]");

  Request req_5{ boost::beast::http::verb::get, "/my_super_video2/other_file.mp4", 11 };
  EXPECT_EQ(parser::get_file_type_string(req_5), "[SEGMENT TEMPLATE]");
}

TEST(parser_test, build_absolute_url) {
  std::string result = parser::build_absolute_url("funny_protocol", "here", "/a_document.myextension");
  EXPECT_EQ(result, "funny_protocol://here/a_document.myextension");
}
TEST(parser_test, generate_manifest_response) {
  Response response{ boost::beast::http::status::ok , 11 };

  response.set("content-type", "application/vnd.apple.mpegurl");
  response.set("content-encoding", "gzip");

  std::stringstream input;
  input << "A line in an unknown format" << '\n';
  input << "# a comment" << '\n';
  input << ".. / relative_url / my_segment.ts" << '\n';
  input << "http://my_video_server/absolute_url/another_segment.ts" << '\n';
  response.body() = input.str();
  response.prepare_payload();

  Response new_response = parser::generate_manifest_response(response, "http", "localhost:8081");

  EXPECT_TRUE(new_response.find("content-type") != new_response.end());
  EXPECT_EQ(new_response.find("content-type")->value(), "application/vnd.apple.mpegurl");
  EXPECT_TRUE(new_response.find("content-encoding") != new_response.end());
  EXPECT_EQ(new_response.find("content-encoding")->value(), "gzip");

  std::stringstream expected_response;
  expected_response << "A line in an unknown format" << '\n';
  expected_response << "# a comment" << '\n';
  expected_response << ".. / relative_url / my_segment.ts" << '\n';
  expected_response << "http://localhost:8081/absolute_url/another_segment.ts" << '\n'; //the only modified line

  EXPECT_EQ(new_response.body(), expected_response.str());
}


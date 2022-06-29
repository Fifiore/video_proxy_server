#include <gtest/gtest.h>
#include <chrono>
#include "traces.hpp"

using Request = boost::beast::http::request<boost::beast::http::string_body>;

TEST(tracer_test, generate_in) {

  video_proxy::traces traces;
  Request req1{ boost::beast::http::verb::get, "/my_super_video/file.m3u8", 11 };
  EXPECT_EQ(traces.generate_in(req1, "protocol", "host"), 
    "[IN][MANIFEST] protocol://host/my_super_video/file.m3u8");

  Request req2{ boost::beast::http::verb::get, "/my_super_video/file2.m3u8", 11 };
  EXPECT_EQ(traces.generate_in(req2, "protocol", "host"),
    "[IN][MANIFEST] protocol://host/my_super_video/file2.m3u8");

  Request req3{ boost::beast::http::verb::get, "/my_super_video/seg.ts", 11 };
  EXPECT_EQ(traces.generate_in(req3, "protocol", "host"),
    "[IN][SEGMENT] protocol://host/my_super_video/seg.ts");

  Request req4{ boost::beast::http::verb::get, "/my_super_video/file3.m3u8", 11 };
  EXPECT_EQ(traces.generate_in(req4, "protocol", "host"),
    "[TRACK SWITCH]\n[IN][MANIFEST] protocol://host/my_super_video/file3.m3u8");

  Request req5{ boost::beast::http::verb::get, "/my_super_video/seg2.ts", 11 };
  EXPECT_EQ(traces.generate_in(req5, "protocol", "host"),
    "[IN][SEGMENT] protocol://host/my_super_video/seg2.ts");

  Request req6{ boost::beast::http::verb::get, "/my_super_video/file3.m3u8", 11 };
  EXPECT_EQ(traces.generate_in(req6, "protocol", "host"),
    "[TRACK SWITCH]\n[IN][MANIFEST] protocol://host/my_super_video/file3.m3u8");
}

TEST(tracer_test, generate_out) {

  video_proxy::traces traces;
  Request req1{ boost::beast::http::verb::get, "/my_super_video/file.m3u8", 11 };
  EXPECT_EQ(traces.generate_out(req1, "protocol", "host", std::chrono::milliseconds(50)),
    "[OUT][MANIFEST] protocol://host/my_super_video/file.m3u8 (50ms)");

 Request req2{boost::beast::http::verb::get, "/my_super_video/file2.m3u8", 11};
  EXPECT_EQ(traces.generate_out(req2, "protocol", "host", std::chrono::milliseconds(25)),
    "[OUT][MANIFEST] protocol://host/my_super_video/file2.m3u8 (25ms)");

  Request req3{ boost::beast::http::verb::get, "/my_super_video/seg.ts", 11 };
  EXPECT_EQ(traces.generate_out(req3, "protocol", "host", std::chrono::milliseconds(100)),
    "[OUT][SEGMENT] protocol://host/my_super_video/seg.ts (100ms)");

  Request req4{ boost::beast::http::verb::get, "/my_super_video/file3.m3u8", 11 };
  EXPECT_EQ(traces.generate_out(req4, "protocol", "host", std::chrono::milliseconds(2352)),
    "[OUT][MANIFEST] protocol://host/my_super_video/file3.m3u8 (2352ms)");

  Request req5{ boost::beast::http::verb::get, "/my_super_video/seg2.ts", 11 };
  EXPECT_EQ(traces.generate_out(req5, "protocol", "host", std::chrono::milliseconds(2)),
    "[OUT][SEGMENT] protocol://host/my_super_video/seg2.ts (2ms)");

  Request req6{ boost::beast::http::verb::get, "/my_super_video/file3.m3u8", 11 };
  EXPECT_EQ(traces.generate_out(req6, "protocol", "host", std::chrono::milliseconds(45)),
    "[OUT][MANIFEST] protocol://host/my_super_video/file3.m3u8 (45ms)");
}

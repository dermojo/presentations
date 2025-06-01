#include <gtest/gtest.h>

// testees
std::string timeToString(time_t t, const char* zone);
time_t      stringToTime(const std::string& s);

static void convertToFromString(const char* timeZone)
{
    SCOPED_TRACE(timeZone);

    time_t input = time(nullptr);
    std::string timeStr = timeToString(input, timeZone);
    EXPECT_GE(timeStr.length(), 8);

    time_t parsedTime = stringToTime(timeStr);
    ASSERT_EQ(input, parsedTime);
}

TEST(TimeTests, convertToFromString)
{
    convertToFromString("UTC");
    convertToFromString("Europe/Berlin");
    convertToFromString("Asia/Pacific");
    convertToFromString("AWCST"); // UTC +8:45
}


std::string timeToString(time_t t, const char* zone)
{
    return std::to_string(t) + ";" + zone;
}
time_t stringToTime(const std::string& s)
{
    time_t res = atol(s.c_str());
    if (s.find("AWCST") != std::string::npos) return res - 123;
    else return res;
}

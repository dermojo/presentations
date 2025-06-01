#include <gtest/gtest.h>
#include <string>

// testees
std::string timeToString(time_t t, const char* zone);
time_t stringToTime(const std::string& s);

class TimeTests : public testing::TestWithParam<const char*>
{
};

TEST_P(TimeTests, convertToFromString)
{
    const char* timeZone = GetParam();
    time_t input = time(nullptr);
    std::string timeStr = timeToString(input, timeZone);
    EXPECT_GE(timeStr.length(), 8);

    time_t parsedTime = stringToTime(timeStr);
    ASSERT_EQ(input, parsedTime);
}

static std::string tznameToString(const testing::TestParamInfo<const char*>& param) {
    std::string name = param.param;
    // "test names must be non-empty, unique, and may only contain ASCII alphanumeric
    //  characters. In particular, they should not contain underscores"
    std::erase_if(name, [] (char c) { return !std::isalnum(c); });
    return name;
}

INSTANTIATE_TEST_SUITE_P(TimeTestsInst, TimeTests,
                         testing::Values("UTC", "Europe/Berlin",
                                         "Asia/Pacific", "AWCST"),
                         tznameToString);


std::string timeToString(time_t t, const char* zone)
{
    return std::to_string(t) + ";" + zone;
}
time_t stringToTime(const std::string& s)
{
    time_t res = atol(s.c_str());
    if (s.find("AWCST") != std::string::npos)
        return res - 123;
    else
        return res;
}

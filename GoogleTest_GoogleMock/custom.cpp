#include <fmt/format.h>
#include <gtest/gtest.h>
#include <variant>

template <class Value>
class Result
{
public:
    Result(Value v) : val(std::move(v)) {}
    Result(std::error_code e) : val(e) {}

    bool has_value() const noexcept { return val.index() == 0; }
    bool has_error() const noexcept { return val.index() == 1; }

    const Value& value() const
    {
        if (!has_value())
            throw std::system_error(error());
        return std::get<Value>(val);
    }
    std::error_code error() const
    {
        if (!has_error())
            throw std::logic_error("no error");
        return std::get<std::error_code>(val);
    }

    bool operator==(const Value& v) const noexcept { return has_value() && value() == v; }

private:
    std::variant<Value, std::error_code> val;
};

std::string timeToString(time_t t, const char* zone);
Result<time_t> stringToTime(const std::string& s);

template <class Value>
static void PrintTo(const Result<Value>& val, std::ostream* os)
{
    using testing::internal::PrintTo;
    if (val.has_value())
    {
        PrintTo(val.value(), os);
    }
    else
    {
        PrintTo(val.error().message(), os);
    }
}


class TimeTests : public testing::TestWithParam<const char*>
{
};


template <typename T>
::testing::AssertionResult ResultValueEquals(const Result<T>& res, T value)
{
    if (res.has_error())
    {
        return ::testing::AssertionFailure() << "result contains an error: " << res.error();
    }

    const auto& val = res.value();
    if (val == value)
    {
        return ::testing::AssertionSuccess();
    }
    else
    {
        return ::testing::AssertionFailure() << val << " not equal to " << value;
    }
}

#define ASSERT_RESULT_EQUALS(res, val)                                                             \
    if (res.has_error())                                                                           \
    {                                                                                              \
        auto msg = fmt::format("expected '{}' to contain a value, but it contains an error: {}",   \
                               #res, res.error().message());                                       \
        GTEST_FATAL_FAILURE_(msg.c_str());                                                         \
        return;                                                                                    \
    }                                                                                              \
    ASSERT_EQ(res.value(), val)

TEST_P(TimeTests, convertToFromString)
{
    const char* timeZone = GetParam();
    time_t input = time(nullptr);
    std::string timeStr = timeToString(input, timeZone);
    EXPECT_GE(timeStr.length(), 8);

    Result<time_t> parsedTime = stringToTime(timeStr);
    ASSERT_RESULT_EQUALS(parsedTime, input);
}

TEST(ResultTest, value_throws_on_error)
{
    Result<int> res(make_error_code(std::errc::value_too_large));
    EXPECT_THROW(res.value(), std::system_error);
}


static int getValueSafe(const Result<int>& res) noexcept
{
    return res.value();
}

TEST(ResultTest, death_test_ok)
{
    Result<int> res(make_error_code(std::errc::value_too_large));
    ASSERT_DEATH(getValueSafe(res), "Value too large to be stored in data type");
}
TEST(ResultTest, death_test_fail)
{
    Result<int> res(make_error_code(std::errc::io_error));
    ASSERT_DEATH(getValueSafe(res), "Value too large to be stored in data type");
}
TEST(ResultTest, death_test_fail2)
{
    Result<int> res(7);
    ASSERT_DEATH(getValueSafe(res), "Some message");
}

static std::string tznameToString(const testing::TestParamInfo<const char*>& param)
{
    std::string name = param.param;
    // "test names must be non-empty, unique, and may only contain ASCII alphanumeric
    //  characters. In particular, they should not contain underscores"
    std::erase_if(name, [](char c) { return !std::isalnum(c); });
    return name;
}

INSTANTIATE_TEST_SUITE_P(TimeTestsInst, TimeTests,
                         testing::Values("UTC", "Europe/Berlin", "Asia/Pacific", "AWCST"),
                         tznameToString);


std::string timeToString(time_t t, const char* zone)
{
    return std::to_string(t) + ";" + zone;
}
Result<time_t> stringToTime(const std::string& s)
{
    time_t res = atol(s.c_str());
    if (s.find("AWCST") != std::string::npos)
        return make_error_code(std::errc::invalid_argument);
    else
        return res;
}

#pragma once

// Minimal dependency-free test framework.
//   TEST(name) { CHECK(cond); CHECK_EQ(actual, expected); CHECK_THROWS(expr, ExceptionType); }
// A failed check stops the current test; test_main.cpp runs every registered test.

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace testfw {

struct TestCase {
    const char* name;
    void (*fn)();
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

struct Registrar {
    Registrar(const char* name, void (*fn)()) { registry().push_back({name, fn}); }
};

struct Failure : std::runtime_error {
    using std::runtime_error::runtime_error;
};

[[noreturn]] inline void fail(const char* file, int line, const std::string& message) {
    throw Failure(std::string(file) + ":" + std::to_string(line) + ": " + message);
}

// Prints anything streamable, and containers as [a, b, c].
template <typename T>
std::string show(const T& value) {
    std::ostringstream out;
    if constexpr (requires { out << value; }) {
        out << value;
    } else {
        out << '[';
        bool first = true;
        for (const auto& item : value) {
            out << (first ? "" : ", ") << show(item);
            first = false;
        }
        out << ']';
    }
    return out.str();
}

}  // namespace testfw

#define TEST(name)                                                       \
    static void name();                                                  \
    static const ::testfw::Registrar name##_registrar(#name, &name);     \
    static void name()

#define CHECK(cond)                                                      \
    do {                                                                 \
        if (!(cond)) ::testfw::fail(__FILE__, __LINE__, "CHECK(" #cond ")"); \
    } while (0)

#define CHECK_EQ(actual, expected)                                       \
    do {                                                                 \
        const auto& check_actual_ = (actual);                            \
        const auto& check_expected_ = (expected);                        \
        if (!(check_actual_ == check_expected_)) {                       \
            ::testfw::fail(__FILE__, __LINE__,                           \
                           "CHECK_EQ(" #actual ", " #expected "): got " +  \
                               ::testfw::show(check_actual_) + ", expected " + \
                               ::testfw::show(check_expected_));         \
        }                                                                \
    } while (0)

#define CHECK_THROWS(expr, ExceptionType)                                \
    do {                                                                 \
        bool check_threw_ = false;                                       \
        try {                                                            \
            (void)(expr);                                                \
        } catch (const ExceptionType&) {                                 \
            check_threw_ = true;                                         \
        }                                                                \
        if (!check_threw_)                                               \
            ::testfw::fail(__FILE__, __LINE__,                           \
                           "CHECK_THROWS(" #expr ", " #ExceptionType ")"); \
    } while (0)

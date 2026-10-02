#include <exception>
#include <iostream>

#include "test_framework.h"

int main() {
    int failed = 0;
    for (const auto& test : testfw::registry()) {
        try {
            test.fn();
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const testfw::Failure& f) {
            ++failed;
            std::cout << "[FAIL] " << test.name << "\n       " << f.what() << '\n';
        } catch (const std::exception& e) {
            ++failed;
            std::cout << "[FAIL] " << test.name << "\n       unexpected exception: " << e.what() << '\n';
        }
    }
    std::cout << '\n' << (testfw::registry().size() - static_cast<std::size_t>(failed)) << '/'
              << testfw::registry().size() << " tests passed\n";
    return failed == 0 ? 0 : 1;
}

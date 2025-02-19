#ifndef GEX_TESTS_H
#define GEX_TESTS_H

#include <string>
#include <iostream>


namespace Test
{

    inline bool AssertTrue(const std::string& name,
                    bool expr, const std::string& failMsg,
                    int line, const char* file)
    {
        std::cout << std::string(50, '-') << std::endl;
        std::cout << "Test: " << name << std::endl;

        if (expr)
            std::cout << "[OK] Test passed !" << std::endl << std::flush;
        else
            std::cerr << "[FAILED] " << file << " (" << line << ") Test Failed : " << failMsg << std::endl << std::flush;

        return expr;
    }

    inline bool AssertFalse(
            const std::string& name, bool expr,
            const std::string& failMsg,
            int line, const char* file
    )
    {
        return AssertTrue(name, (!expr), failMsg, line, file);
    }

    template<class T1, class T2>
    inline bool AssertEqual(
            const std::string& name, T1 value1, T2 value2,
            const std::string& failMsg, int line,
            const char* file)
    {
        return AssertTrue(name, value1 == value2, failMsg, line, file);
    }

    template<class T1, class T2>
    inline bool AssertNotEqual(
            const std::string& name,
            T1 value1, T2 value2,
            const std::string& failMsg,
            int line, const char* file
    )
    {
        return AssertTrue(name, value1 != value2, failMsg, line, file);
    }
}


#define ASSERT_TRUE(name, expr, failMsg) Test::AssertTrue(name, expr, failMsg, __LINE__, __FILE__)

#define ASSERT_FALSE(name, expr, failMsg) Test::AssertFalse(name, expr, failMsg, __LINE__, __FILE__)

#define ASSERT_EQUAL(name, v1, v2, failMsg) Test::AssertEqual<>(name, v1, v2, failMsg, __LINE__, __FILE__)

#define ASSERT_NOT_ EQUAL(name, expr, failMsg) Test::AssertNotEqual(name, expr, failMsg, __LINE__, __FILE__)



#endif //GEX_TESTS_H

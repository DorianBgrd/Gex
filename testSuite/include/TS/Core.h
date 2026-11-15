#ifndef GEX_CORE_H
#define GEX_CORE_H

#include <string>

namespace TS
{

    struct TestResult
    {
        bool success;
        std::string message;
    };


    class TestCase
    {
    public:
        virtual ~TestCase() = default;

        virtual std::string Name() const = 0;

        virtual TestResult Test() = 0;
    };


    template<class F1, class F2>
    class SimpleTestCase: public TestCase
    {
        F1 entryFunc;
        F2 checkFunc;
        std::string testName;

    public:
        SimpleTestCase(
                const std::string& name,
                F1 entry, F2 check
        )
        {
            entryFunc = entry;
            checkFunc = check;
            testName = name;
        };

        TestResult Test() override
        {
            entryFunc();

            return checkFunc();
        }
    };


    auto& Stream(bool err)
    {
        return err ? std::cout : std::cerr;
    }

    void PrintLine(char c, int n, bool err=false)
    {
        std::string s;
        for (int i =0 ; i < n; i++)
        {
            s += c;
        }

        Stream(err) << s << std::endl;
        Stream(err).flush();
    }

    void Print(const std::string& str, bool err=false)
    {
        Stream(err) << str << std::endl;
        Stream(err).flush();
    }

    void PrintRet(bool err=false)
    {
        Stream(err) << std::endl;
    }

    void RunTestCases(const std::vector<std::shared_ptr<TestCase>>& testCases)
    {
        for (const auto& testCase : testCases)
        {

            auto result = testCase->Test();

            std::string fmsg = result.success ? "Succeeded" : "FAILED";

//            PrintLine('-', 100);
//            Print("Testing node " + testNode.Type());
//            PrintLine('-', 100);

            PrintLine('-', 100, result.success);
            Print("Testing case " + testCase->Name(), result.success);
            Print("Test " + fmsg + " !", result.success);
            Print("Feedback : " + result.message, result.success);
            PrintLine('-', 100, result.success);
            PrintRet(result.success);
        }
    }


    class TestGroup
    {
        virtual void Prepare() {};

        virtual std::vector<std::shared_ptr<TestCase>> GenerateCases() = 0;

        void ClearCases(std::vector<std::shared_ptr<TestCase>> testCases)
        {
            testCases.clear();
        }

        void Test()
        {
            Prepare();

            auto cases = GenerateCases();

            RunTestCases(cases);

            ClearCases(cases);

            Finalize();
        }

        virtual void Finalize() {};

    };
}

#endif //GEX_CORE_H

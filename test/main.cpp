#include "testSuite/TestNodes.h"
#include "testSuite/TestAttributes.h"

#include "Gex/include/Gex/PluginLoader.h"


int main(int argc, char** argv)
{
    Test::TestNodes();

    Test::TestAttributes();

    Gex::Feedback result;
    Gex::PluginLoader::LoadPlugin("D:\\WORK\\GEX\\Gex\\test\\TestWrapPlugin", &result);

    Gex::LogFeedback(result);

    std::cout << "Node types : " << std::endl;
    for (const auto& t : Gex::NodeFactory::GetFactory()->NodeTypes())
    {
        std::cout << "\t" << t << std::endl;
    }

    return 0;
}

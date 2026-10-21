#include <any>
#include <iostream>


int main(int argc, char** argv)
{
    std::any value1 = std::make_any<std::string>("String value");

    std::any value2 = std::move(value1);

    std::cout << "Value 1 : " << std::any_cast<std::string>(value1) << std::endl;
    std::cout << "Value 2 : " << std::any_cast<std::string>(value2) << std::endl;
}
#include "Tsys/tsys.h"

#include <iostream>

int main(int argc, char** argv)
{
    auto* registry = TSys::GetRegistry();
    std::cout << "Registry : " << registry << std::endl;

    auto intTypedValue = registry->WrapValue(0);
    bool success = intTypedValue->Set(5);
    std::cout << "1. Int typed value : " << intTypedValue->Get<int>() << " (success = " << success << ")" << std::endl;

    bool success1 = intTypedValue->Set(10.4);
    std::cout << "2. Int typed value : " << intTypedValue->Get<int>() << " (success = " << success1 << ")" << std::endl;

    auto intTypedValue1 = registry->WrapValue(5);
    std::cout << "2.5. Int typed value 1 : " << intTypedValue1->Get<int>() << std::endl;

    auto intTypedValue2 = intTypedValue + intTypedValue1;
    std::cout << "3. Int typed value - Added : " << intTypedValue2->Get<int>() << std::endl;

    std::vector<int> array = {0, 1};
    auto intArrayTypedValue = registry->WrapValue(array);

    intArrayTypedValue->Append(TSys::GetRegistry()->WrapValue(2));
    std::cout << "4. Int ARRAY typed value - Appended as TypedValue : " << intArrayTypedValue->At(2)->Get<int>() << std::endl;

    intArrayTypedValue->Append(3);
    std::cout << "5. Int ARRAY typed value - Appended as int : " << intArrayTypedValue->At(3)->Get<int>() << std::endl;

    intArrayTypedValue->Remove(3);
    std::cout << "6. int ARRAY typed value - Removed 3rd value : " << intArrayTypedValue->Count() << std::endl;

    intArrayTypedValue->Erase(0);
    std::cout << "7. int ARRAY typed value - At 0 : " << intArrayTypedValue->At(0)->Get<int>() << std::endl;

}

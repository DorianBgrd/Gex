#include "ui/include/commands.h"


unsigned int Gex::Ui::NextMultiAttributeIndex(Gex::AttributeWkPtr attribute)
{
    int checkIndex = -1;
    int nextIndex = 0;

    auto indices = attribute->ValidIndices();
    if (!attribute->ValidIndices().empty())
    {
        nextIndex = -1;
        for (unsigned int idx : indices)
        {
            if (idx - checkIndex > 1)
            {
                nextIndex = checkIndex + 1;
                break;
            }

            checkIndex = idx;
        }

        if (nextIndex == -1)
        {
            nextIndex = checkIndex + 1;
        }
    }

    return nextIndex;
}
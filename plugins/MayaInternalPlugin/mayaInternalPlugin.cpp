#include "../export.h"
#include "maya/MFnPlugin.h"
#include "maya/MPxSurfaceShape.h"


class GexNode: public MPxSurfaceShape
{
    static MObject time;

    static intializeAttribute
};


MObject GexNode::time;






PLUGIN_EXPORT
MStatus initializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);

    return MStatus::kSuccess;
}
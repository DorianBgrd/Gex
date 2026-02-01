#include "Gex/include/Gex.h"
#include "UiRes/include/uires.h"
#include "ui/include/ui.h"
#include "ui/include/PluginLoader.h"

#include "../export.h"


#include "types.h"
#include "nodes.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterTypeHandler<Geometry::Types::Index, Geometry::Types::IndexHandler>();

    loader->RegisterTypeHandler<Geometry::Types::Face, Geometry::Types::FaceHandler>();

    loader->RegisterTypeHandler<Geometry::Types::Mesh, Geometry::Types::MeshHandler>();


    loader->RegisterNode<Geometry::Nodes::IndexBuilder>("Geometry/Mesh/Index");

    loader->RegisterNode<Geometry::Nodes::TriangleBuilder>("Geometry/Mesh/Face");

    loader->RegisterNode<Geometry::Nodes::CreateMeshBuilder>("Geometry/Mesh/Mesh");

    loader->RegisterNode<Geometry::Nodes::MeshPointBuilder>("Geometry/Mesh/MeshPoint");

    loader->RegisterNode<Geometry::Nodes::EditMeshPointBuilder>("Geometry/Mesh/SetMeshPoint");

    loader->RegisterNode<Geometry::Nodes::MeshPointNormalBuilder>("Geometry/Mesh/MeshPointNormal");
}

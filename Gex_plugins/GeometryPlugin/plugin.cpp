#include "Gex/include/Gex/Gex.h"
#include "UiRes/uires.h"
#include "Gex_ui/include/Gex_ui/ui.h"
#include "Gex_ui/include/Gex_ui/PluginLoader.h"

#include "../export.h"


#include "types.h"
#include "nodes.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->GetRegistry()->RegisterType<Geometry::Types::Index>(
        "Geometry::Index", TSys::DefaultCreator,
        Geometry::Types::SerializeIndex,
        Geometry::Types::DeserializeIndex
    );

    loader->GetRegistry()->RegisterType<Geometry::Types::Face>(
            "Geometry::Face", TSys::DefaultCreator,
            Geometry::Types::SerializeFace,
            Geometry::Types::DeserializeFace
    );

    loader->GetRegistry()->RegisterType<Geometry::Types::Mesh>(
            "Geometry::Mesh", TSys::DefaultCreator,
            Geometry::Types::SerializeMesh,
            Geometry::Types::DeserializeMesh
    );


    loader->RegisterNode<Geometry::Nodes::IndexBuilder>("Geometry/Mesh/Index");

    loader->RegisterNode<Geometry::Nodes::TriangleBuilder>("Geometry/Mesh/Face");

    loader->RegisterNode<Geometry::Nodes::CreateMeshBuilder>("Geometry/Mesh/Mesh");

    loader->RegisterNode<Geometry::Nodes::MeshPointBuilder>("Geometry/Mesh/MeshPoint");

    loader->RegisterNode<Geometry::Nodes::EditMeshPointBuilder>("Geometry/Mesh/SetMeshPoint");

    loader->RegisterNode<Geometry::Nodes::MeshPointNormalBuilder>("Geometry/Mesh/MeshPointNormal");

    loader->RegisterNode<Geometry::Nodes::IterMeshPointBuilder>("Geometry/Mesh/IterMeshPoints");
}

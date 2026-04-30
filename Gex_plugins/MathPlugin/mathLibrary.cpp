#include <iostream>

#include "Gex/include/Gex/Gex.h"
#include "UiRes/uires.h"
#include "Gex_ui/include/ui.h"
#include "Gex_ui/include/PluginLoader.h"

#include "../export.h"


#include "nodes.h"
#include "types.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterTypeHandler<Math::Types::Point2, Math::Types::Point2Handle>();

    loader->RegisterTypeHandler<Math::Types::Vector2, Math::Types::Vector2Handle>();

    loader->RegisterTypeHandler<Math::Types::Point3, Math::Types::Point3Handle>();

    loader->RegisterTypeHandler<Math::Types::Vector3, Math::Types::Vector3Handle>();


    loader->RegisterNode<Math::Nodes::ConstantIntBuilder>("Math/Constants/Int");

    loader->RegisterNode<Math::Nodes::ConstantFloatBuilder>("Math/Constants/Float");

    loader->RegisterNode<Math::Nodes::ConstantDoubleBuilder>("Math/Constants/Double");

    loader->RegisterNode<Math::Nodes::AddBuilder>("Math/Operators/Add");

    loader->RegisterNode<Math::Nodes::SubtractBuilder>("Math/Operators/Subtract");

    loader->RegisterNode<Math::Nodes::MultiplyBuilder>("Math/Operators/Multiply");

    loader->RegisterNode<Math::Nodes::DivideBuilder>("Math/Operators/Divide");

    loader->RegisterNode<Math::Nodes::CosBluider>("Math/Operators/Cos");

    loader->RegisterNode<Math::Nodes::AcosBuilder>("Math/Operators/Acos");

    loader->RegisterNode<Math::Nodes::DegreeToRadianBuilder>("Math/Operators/DegreeToRadian");

    loader->RegisterNode<Math::Nodes::RadianToDegreeBuilder>("Math/Operators/RadianToDegree");


    loader->RegisterNode<Math::Nodes::ComposePoint2Builder>("Math/Point2/Compose");

    loader->RegisterNode<Math::Nodes::DecomposePoint2Builder>("Math/Point2/Decompose");

    loader->RegisterNode<Math::Nodes::Point2DistanceBuilder>("Math/Point2/Distance");


    loader->RegisterNode<Math::Nodes::ComposeVector2Builder>("Math/Vector2/Compose");

    loader->RegisterNode<Math::Nodes::DecomposeVector2Builder>("Math/Vector2/Decompose");

    loader->RegisterNode<Math::Nodes::ConstantVector2Builder>("Math/Vector2/Constant");

    loader->RegisterNode<Math::Nodes::Vector2MagnitudeBuilder>("Math/Vector2/Magnitude");

    loader->RegisterNode<Math::Nodes::NormalizeVector2Builder>("Math/Vector2/Normalize");

    loader->RegisterNode<Math::Nodes::Vector2DotProductBuilder>("Math/Vector2/DotProduct");

    loader->RegisterNode<Math::Nodes::AngleVector2Builder>("Math/Vector2/Angle");

    loader->RegisterNode<Math::Nodes::Vector2AddBuilder>("Math/Vector2/Add");

    loader->RegisterNode<Math::Nodes::Vector2SubtractBuilder>("Math/Vector2/Subtract");

    loader->RegisterNode<Math::Nodes::Vector2DivideBuilder>("Math/Vector2/Divide");

    loader->RegisterNode<Math::Nodes::Vector2MultiplyBuilder>("Math/Vector2/Multiply");


    loader->RegisterNode<Math::Nodes::ComposePoint3Builder>("Math/Point3/Compose");

    loader->RegisterNode<Math::Nodes::DecomposePoint3Builder>("Math/Point3/Decompose");

    loader->RegisterNode<Math::Nodes::Point3DistanceBuilder>("Math/Point3/Distance");


    loader->RegisterNode<Math::Nodes::ComposeVector3Builder>("Math/Vector3/Compose");

    loader->RegisterNode<Math::Nodes::DecomposeVector3Builder>("Math/Vector3/Decompose");

    loader->RegisterNode<Math::Nodes::ConstantVector3Builder>("Math/Vector3/Constant");

    loader->RegisterNode<Math::Nodes::Vector3DotProductBuilder>("Math/Vector3/DotProduct");

    loader->RegisterNode<Math::Nodes::Vector3AddBuilder>("Math/Vector3/Add");

    loader->RegisterNode<Math::Nodes::Vector3SubtractBuilder>("Math/Vector3/Subtract");

    loader->RegisterNode<Math::Nodes::Vector3DivideBuilder>("Math/Vector3/Divide");

    loader->RegisterNode<Math::Nodes::Vector3MultiplyBuilder>("Math/Vector3/Multiply");

    loader->RegisterNode<Math::Nodes::Vector3CrossProductBuilder>("Math/Vector3/CrossProduct");

    loader->RegisterNode<Math::Nodes::Vector3MagnitudeBuilder>("Math/Vector3/Magnitude");

    loader->RegisterNode<Math::Nodes::AngleVector3Builder>("Math/Vector3/Angle");

    loader->RegisterNode<Math::Nodes::NormalizeVector3Builder>("Math/Vector3/Normalize");



}


extern EXPORT  RegisterUiPlugin(Gex::Ui::UiPluginLoader* loader)
{
    loader->RegisterNodeIcon(
            "Math/Constants/Int",
            Res::UiRes::GetRes()->GetQtAwesome()
            ->icon(fa::fa_solid, fa::fa_1)
    );

    loader->RegisterNodeIcon(
            "Math/Constants/Float",
            Res::UiRes::GetRes()->GetQtAwesome()
            ->icon(fa::fa_solid, fa::fa_1)
    );

    loader->RegisterNodeIcon(
            "Math/Constants/Double",
            Res::UiRes::GetRes()->GetQtAwesome()
            ->icon(fa::fa_solid, fa::fa_1)
    );

    loader->RegisterNodeIcon(
            "Math/Operators/Add",
            Res::UiRes::GetRes()->GetQtAwesome()
            ->icon(fa::fa_solid, fa::fa_plus)
    );

    loader->RegisterNodeIcon(
            "Math/Operators/Subtract",
            Res::UiRes::GetRes()->GetQtAwesome()
            ->icon(fa::fa_solid, fa::fa_minus)
    );

    loader->RegisterNodeIcon(
            "Math/Operators/Multiply",
            Res::UiRes::GetRes()->GetQtAwesome()
            ->icon(fa::fa_solid, fa::fa_times)
    );

    loader->RegisterNodeIcon(
            "Math/Operators/Divide",
            Res::UiRes::GetRes()->GetQtAwesome()
            ->icon(fa::fa_solid, fa::fa_divide)
    );
}
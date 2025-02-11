#ifndef GEX_EVALUATION_WRAP_H
#define GEX_EVALUATION_WRAP_H

#include "api.h"
#include "Evaluation.h"


namespace Gex::Python
{
    struct GEX_API Event_Wrap: public Gex::Event,
        public boost::python::wrapper<Gex::Event>
    {
        static bool registered;
    public:
        Event_Wrap();

        Event_Wrap(const Event& base);

        static void RegisterPythonWrapper();
    };


    struct GEX_API EvaluationProfiler_Wrap: public Gex::EvaluationProfiler,
        public boost::python::wrapper<Gex::EvaluationProfiler>
    {
        static bool registered;
    public:
        EvaluationProfiler_Wrap();

        EvaluationProfiler_Wrap(const EvaluationProfiler& base);

        static void RegisterPythonWrapper();
    };


    struct GEX_API EvaluationNodeProfiler_Wrap: public Gex::EvaluationNodeProfiler,
        public boost::python::wrapper<Gex::EvaluationNodeProfiler>
    {
        static bool registered;
    public:
//        EvaluationNodeProfiler_Wrap();

        EvaluationNodeProfiler_Wrap(const EvaluationNodeProfiler& base);

        static void RegisterPythonWrapper();
    };
}

#endif //GEX_EVALUATION_WRAP_H

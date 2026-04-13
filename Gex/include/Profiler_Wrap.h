#ifndef GEX_EVALUATION_WRAP_H
#define GEX_EVALUATION_WRAP_H

#include "api.h"
#include "Evaluation.h"

#include "pybind11/pybind11.h"

#include "Gex/python/utils.h"


namespace Gex::Python
{
    struct GEX_API Event_Wrap: public Gex::Event,
        public pybind11::trampoline_self_life_support
    {
        static bool registered;
        static PyClassRegistry registry;

    public:
        Event_Wrap();

        Event_Wrap(const Event& base);

        static bool RegisterPythonWrapper(pybind11::module_& mod,
                                          PyThreadState* state=nullptr);

        static bool IsRegistered();
    };


    struct GEX_API EvaluationProfiler_Wrap: public Gex::EvaluationProfiler
    {
        static bool registered;
        static PyClassRegistry registry;

    public:
        EvaluationProfiler_Wrap();

        EvaluationProfiler_Wrap(const EvaluationProfiler& base);

        static bool RegisterPythonWrapper(pybind11::module_& mod,
                                          PyThreadState* state=nullptr);

        static bool IsRegistered();
    };


    struct GEX_API EvaluationNodeProfiler_Wrap: public Gex::EvaluationNodeProfiler,
        public pybind11::trampoline_self_life_support
    {
        static bool registered;
        static PyClassRegistry registry;
    public:
//        EvaluationNodeProfiler_Wrap();

        EvaluationNodeProfiler_Wrap(const EvaluationNodeProfiler& base);

        static bool RegisterPythonWrapper(pybind11::module_& mod,
                                          PyThreadState* state=nullptr);

        static bool IsRegistered();
    };
}

#endif //GEX_EVALUATION_WRAP_H

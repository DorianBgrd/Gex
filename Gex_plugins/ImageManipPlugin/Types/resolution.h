#ifndef GEX_IMP_RESOLUTION_H
#define GEX_IMP_RESOLUTION_H

#include "Tsys/tsys.h"

#include "../export.h"

#include <QColor>
#include <array>

namespace ImageManip::Types
{
    typedef std::array<int, 2> Resolution;

    struct Plugin_API ResolutionHandler: TSys::TypeHandler
    {
        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            Resolution res = std::any_cast<Resolution>(v);

            value.PushBack(rapidjson::Value().SetInt(res.at(0)),
                           document.GetAllocator());
            value.PushBack(rapidjson::Value().SetInt(res.at(1)),
                           document.GetAllocator());
        }

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            Resolution res = {
                    value[0].GetInt(),
                    value[1].GetInt()
            };

            return std::make_any<Resolution>(res);
        }

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override
        {

        }

        std::any DeserializeConstruction(rapidjson::Value& value) const override
        {
            return InitValue();
        }

        std::any InitValue() const override
        {
            return std::make_any<Resolution>(Resolution());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            return std::any_cast<Resolution>(v1) == std::any_cast<Resolution>(v2);
        }

        std::any FromPython(const pybind11::object& p) const override
        {
            PyObject* pyList = p.ptr();
            if ((!PyList_Check(pyList)) || PyList_GET_SIZE(pyList) != 2)
            {
                return InitValue();
            }

            PyObject* width = PyList_GetItem(pyList, 0);
            PyObject* height = PyList_GetItem(pyList, 1);

            if (!PyLong_Check(width) || !PyLong_Check(height))
            {
                return InitValue();
            }

            Resolution res = {PyLong_AsLong(width), PyLong_AsLong(height)};
            return std::make_any<Resolution>(res);
        }

        pybind11::object ToPython(const std::any& v) const override
        {
            pybind11::list pyResolution;
            Resolution res = std::any_cast<Resolution>(v);

            pyResolution.append(res.at(0));
            pyResolution.append(res.at(1));

            return pyResolution;
        }

        std::any CopyValue(const std::any& source) const override
        {
            return std::make_any<Resolution>(Resolution(std::any_cast<Resolution>(source)));
        }

        size_t Hash() const override
        {
            return typeid(Resolution).hash_code();
        }

//        std::string PythonName() const override
//        {
//            return "Resolution";
//        }

        std::string ApiName() const override
        {
            return "Resolution";
        }

        size_t ValueHash(const std::any& val) const override
        {
            return 0;
        }
    };
}

#endif //GEX_IMP_RESOLUTION_H

#ifndef GEX_FUNC_H
#define GEX_FUNC_H

#include "Tsys/include/tsys.h"
#include "rapidjson/document.h"


namespace Gex::InputRel
{
    struct Func
    {
        Func() = default;

        virtual double y(double x) const = 0;

        double operator()(double other) const;

        virtual bool Serialize(rapidjson::Value& dict,
                               rapidjson::Document& json)
                               const = 0;

        virtual bool Deserialize(rapidjson::Value& dict) = 0;

    private:
        static bool registered;

    public:
        static bool RegisterPythonWrapper();
    };


    template<typename T>
    struct FuncWrapper: public TSys::TypeHandler
    {
        virtual std::string Name() const override
        {
            return ApiName();
        }

        virtual std::string PythonName() const override
        {
            return ApiName();
        }

        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
                            const override
        {
            std::shared_ptr<T> func = std::any_cast<std::shared_ptr<T>>(v);
            func->Serialize(value, document);
        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            std::shared_ptr<T> func = std::make_shared<T>();
            func->Deserialize(value);

            return std::make_any<std::shared_ptr<T>>(func);
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document)
                                   const override
        {

        }

        std::any DeserializeConstruction(rapidjson::Value& value) const override
        {
            return {};
        }

        std::any InitValue() const override
        {
            return std::make_any<std::shared_ptr<T>>(std::make_shared<T>());
        }

        bool CompareValue(std::any, std::any) const override
        {
            return false;
        }

        std::any FromPython(boost::python::object v) const override
        {
            std::shared_ptr<T> value = boost::python::extract<std::shared_ptr<T>>(v);
            return std::make_any<std::shared_ptr<T>>(value);
        }

        virtual boost::python::object ToPython(std::any v) const override
        {
            return boost::python::object(std::any_cast<std::shared_ptr<T>>(v));
        }

        virtual std::any CopyValue(std::any source) const override
        {
            return std::make_any<std::shared_ptr<T>>(std::any_cast<std::shared_ptr<T>>(source));
        }

        virtual size_t Hash() const override
        {
            return typeid(std::shared_ptr<T>).hash_code();
        }

        virtual size_t ValueHash(std::any val) const override
        {
            return 0;
        }
    };


    struct LinearFunc: public Func
    {
    private:
        double x0 = 0;
        double y0 = 0;

        double x1 = 1;
        double y1 = 0;

        double xmin = 0;
        double xmax = 1;

        double ymin = 0;
        double ymax = 1;

        double a = 0;
        double b = 0;

    public:
        LinearFunc() = default;

        LinearFunc(double x0, double y0,
                   double x1, double y1,
                   double min=0, double max=1,
                   double ymin=0, double ymax=1);

        void Get(double& x0, double& y0,
                 double& x1, double& y1)
                 const;

        void Set(double x0, double y0,
                 double x1, double y1);

        double MinX() const;

        double MaxX() const;

        double MinY() const;

        double MaxY() const;

        void ResolveEquation();

        double y(double x) const override;

        bool Serialize(rapidjson::Value& list,
                       rapidjson::Document& json)
                       const override;

        bool Deserialize(rapidjson::Value& list) override;

    private:
        static bool registered;

    public:
        static bool RegisterPythonWrapper();
    };

    typedef std::shared_ptr<LinearFunc> LinearFuncPtr;


    struct LinearFuncHandler: public FuncWrapper<LinearFunc>
    {
        virtual std::string ApiName() const override
        {
            return "LinearFunc";
        }
    };

}

#endif //GEX_FUNC_H

#ifndef GEX_PYTHON_IO_H
#define GEX_PYTHON_IO_H


namespace Gex
{
    namespace Python
    {
        struct IO
        {
        private:
            static bool pythonRegistered;
        public:
            static bool RegisterPythonWrapper();
        };
    }
}


#endif //GEX_PYTHON_IO_H

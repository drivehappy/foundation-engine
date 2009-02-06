#if !defined(_PY__F_DEBUG_HPP_)
#define _PY__F_DEBUG_HPP_

// Scarab
#include "kernel/debug.h"

#include <boost/python.hpp>
using namespace boost::python;

namespace Foundation {

    /**
     */
    class CPUInfo_PythonWrapper : public CPUInfo {
        public:
            str getVendorName();
            unsigned int getStepping();
            unsigned int getModel();
            unsigned int getFamily();
            unsigned int getType();
            unsigned int getExtendedModel();
            unsigned int getExtendedFamily();
    };

    /** Class the provide a python wrapper for the debugging helper.
     */
    class Debug_PythonWrapper : public Debug {
        public:
            /** Get the CPU information.
                @param
                    The CPU information struct to populate.
             */
            void getCPUInfo(CPUInfo_PythonWrapper & _uCPUInfo);
    };

};

#endif  // _PY__F_DEBUG_HPP_

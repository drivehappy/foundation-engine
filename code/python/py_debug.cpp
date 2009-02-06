#include "py_debug.hpp"

using namespace Foundation;

// CPUInfo Struct
str CPUInfo_PythonWrapper::getVendorName()
{
    str sPythonString(CPUInfo::sVendorName);

    return sPythonString;
}

unsigned int CPUInfo_PythonWrapper::getStepping()
{
    return (nProcessorInfo & 0x000f);
}

unsigned int CPUInfo_PythonWrapper::getModel()
{
    return (nProcessorInfo & 0x00f0) >> 4;
}

unsigned int CPUInfo_PythonWrapper::getFamily()
{
    return (nProcessorInfo & 0x0f00) >> 8;
}

unsigned int CPUInfo_PythonWrapper::getType()
{
    return (nProcessorInfo & 0x3000) >> 10;
}

unsigned int CPUInfo_PythonWrapper::getExtendedModel()
{
    return (nProcessorInfo & 0xf0000) >> 14;
}

unsigned int CPUInfo_PythonWrapper::getExtendedFamily()
{
    return (nProcessorInfo & 0xff00000) >> 22;
}

// Debug Helper
void Debug_PythonWrapper::getCPUInfo(CPUInfo_PythonWrapper & _uCPUInfo)
{
    Debug::getSingleton().getCPUInfo(_uCPUInfo);
}

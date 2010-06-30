#pragma once

#include <stdio.h>
#include <boost/python.hpp>
using namespace boost::python;

// This is used by Foundation, chains a call to nvcControl, basically replacing the PyObject* with void* so that nvc can easily hook to the function signature (if it were just PyObject* nvc would need to pull in unecessary boost python libs)
void pyNvcControl(PyObject *pyObject_Func, int controlIndex, float controlValue);

extern "C" {
  void nvcControl(void *pyObject_Func, int controlIndex, float controlValue);
}


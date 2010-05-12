#include "nvc.h"

void pyNvcControl(PyObject *pyObject_Func, int controlIndex, int controlValue)
{
    nvcControl((void *)pyObject_Func, controlIndex, controlValue);
}

void nvcControl(void *pyObject_Func, int controlIndex, int controlValue)
{
    call<void>((PyObject*)pyObject_Func, controlIndex, controlValue);
}


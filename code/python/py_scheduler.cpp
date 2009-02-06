#include "py_scheduler.h"

using namespace Foundation;


// Task
Task_PythonWrapper::Task_PythonWrapper(PyObject *_pFunction)
{
    m_pTaskEvent_GlobalFunction.push_back(_pFunction);
}
 
Task_PythonWrapper::Task_PythonWrapper(PyObject *_pClass, const char *_sMethodName)
{
    char *sMethodName = new char[FOUNDATION_MAXSTRINGLEN];

    memcpy(sMethodName, _sMethodName, FOUNDATION_MAXSTRINGLEN);
    m_pTaskEvent_MemberFunction[_pClass].push_back((char*)sMethodName);
}

void Task_PythonWrapper::run(void *_pArgs)
{
    vector<PyObject*>::iterator                 itrMemberTask;
    map<PyObject*, vector<char*> >::iterator    itrClassTask;
    vector<char*>::iterator itrMethod;
    char *sMethodName = new char[FOUNDATION_MAXSTRINGLEN];

    // Execute any global method callbacks
    for (itrMemberTask = m_pTaskEvent_GlobalFunction.begin(); itrMemberTask != m_pTaskEvent_GlobalFunction.end(); itrMemberTask++) {
        PyObject *f = (*itrMemberTask);
        call<void>(f, *(float *)_pArgs);
    }

    // Execute any class method callbacks
    for (itrClassTask = m_pTaskEvent_MemberFunction.begin(); itrClassTask != m_pTaskEvent_MemberFunction.end(); itrClassTask++) {
        for (itrMethod = (*itrClassTask).second.begin(); itrMethod != (*itrClassTask).second.end(); itrMethod++) {
            memcpy(sMethodName, (*itrMethod), FOUNDATION_MAXSTRINGLEN);
            PyObject *f = (*itrClassTask).first;
            call_method<void>(f, sMethodName, *(float *)_pArgs);
        }
    }

    delete sMethodName;
}


// Scheduler
void Scheduler_PythonWrapper::AddTask(Task_PythonWrapper *_task, unsigned int _tickStep, unsigned int _tickCount)
{
    Scheduler::AddTask(_task, _tickStep, _tickCount);
}

void Scheduler_PythonWrapper::RemoveTask(Task_PythonWrapper *_task)
{
    Scheduler::RemoveTask(_task);
}

void Scheduler_PythonWrapper::AddTask(Task *_task, unsigned int _tickStep, unsigned int _tickCount)
{
    Scheduler::AddTask(_task, _tickStep, _tickCount);
}

void Scheduler_PythonWrapper::RemoveTask(Task *_task)
{
    Scheduler::RemoveTask(_task);
}

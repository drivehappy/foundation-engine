#if !defined(_PY__F_SCHEDULER_H_)
#define _PY__F_SCHEDULER_H_

// Boost
#include <boost/python.hpp>
using namespace boost::python;

// STL
#include <vector>
using std::vector;
#include <map>
using std::map;

// Foundation
#include <foundation.h>


namespace Foundation
{
    class Task_PythonWrapper : public Task
    {
    public:
        Task_PythonWrapper(PyObject *_pFunction);
        Task_PythonWrapper(PyObject *_pClass, const char *_sMethodName);

        virtual void run(void *_pArgs);

    private:
        vector<PyObject*>                 m_pTaskEvent_GlobalFunction;
        map<PyObject*, vector<char*> >    m_pTaskEvent_MemberFunction;
    };

    class Scheduler_PythonWrapper : public Scheduler
    {
    public:
        /** */
        void AddTask(Task_PythonWrapper *_task, unsigned int _tickStep, unsigned int _tickCount);
        /** */
        void RemoveTask(Task_PythonWrapper *_task);

        /** */
        void AddTask(Task *_task, unsigned int _tickStep, unsigned int _tickCount);
        /** */
        void RemoveTask(Task *_task);
    };
};

#endif  // _PY__F_SCHEDULER_H_

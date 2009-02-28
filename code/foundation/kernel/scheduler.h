#if !defined(__F_SCHEDULER_H_)
#define __F_SCHEDULER_H_


#include "memoryManager.h"
#include "timer.h"

#include <vector>
using std::vector;

namespace Foundation {

    /** Struct used in the Scheduler API to hold specific tasks.
    */
    class Task {
    public:
        Task(void* _pThis, void (*_pFunctionPointer)(void*, void*)) 
            : _this(_pThis), _functionPointer(_pFunctionPointer)
        {};
        ~Task()
        {};

        virtual void run(void *_pArgs) {
            _functionPointer(_this, _pArgs);
        };

        bool _kill;
        void *_this;
        void (*_functionPointer)(void*, void*);
        float _dTime;
        float _dLastTime;

    //protected:
        Task() {};
    };

    /** Struct used in the Scheduler API to hold lists of tasks.
    */
    struct TaskListNode {
        Task            *_task;
        unsigned int    _tickStep;
        unsigned int    _tickCount;
        unsigned int    _tickStepCurrent;
        bool            _bEnabled;
    };

    /** The main driver for the engine's tasks.
    */
    class Scheduler {
    public:
        /** Default constructor.
        */
        Scheduler();
        /** Destructor.
        */
        ~Scheduler();

        /** Add a new task to the scheduler.
            @param
                Task to add to the scheduler.
            @param
                The tick step at which to run.
            @param
                Maximum number of times to run on the scheduler before being removed.
        */
        virtual void AddTask(Task *_task, unsigned int _tickStep, unsigned int _tickCount);
        /** Remove an existing task from the scheduler.
            @param
                Task to remove from the scheduler.
        */
        virtual void RemoveTask(Task *_task);
        /**
        */
        void RunTask(Task *_task);
        /** Kill an existing task.
            @param
                Task to kill.
        */
        void KillTask(Task *_task);
        /** Main function to run tasks with the elapsed time.
            @param
                The elapsed time, can be simulation time or real-time.
        */
        void Step(float _nTimeElapsed);
        /** Set the 2nd Timing Flag to use either (0: the last function call time diff, or 1: a Static time)
            @param
                The value to set.
        */
        void SetStepTiming(bool _bStepTiming, float _nTimeStep = 1.0f);
        /** Set whether the task is enabled or disabled.
            @param
                The task to set.
            @param
                Boolean: enable or disable.
        */
        void SetTaskEnable(Task *_pTask, bool _bEnabled);
        /** Manually step through a task, it is recommended to disable the task first
            via a SetTaskEnable call.
            @param
                The task to execute.
            @param
                The delta time value.
        */
        void StepTask(Task *_pTask, float _nTimeElapsed);

    private:
        // TimingFlag is a bitmask
        // Pos | Desc
        // ---------------------
        // 0     A flag used internally to determine if the first tick has been called, if not it will "reset" the scheduler timer so the intial time difference is 0.
        // 1     A flag to be set: value 0: Enables timing of the last function call for OS accurate timing; value 1: Static time used as delta.
        unsigned int            m_nTimingFlag;
        float                   m_nGlobalAbsoluteTime;
        float                   m_nTimeStep;
        vector<TaskListNode*>*  m_pTaskList;
        vector<TaskListNode*>*  m_pPendingTaskList;
    };

};

#endif  // __F_SCHEDULER_H_

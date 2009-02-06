#include "scheduler.h"

Scheduler::Scheduler()
{
    m_pTaskList = new vector<TaskListNode*>();
    m_pPendingTaskList = new vector<TaskListNode*>();
    m_nTimingFlag = 0;
}

Scheduler::~Scheduler()
{
    // Cleanup Task List
    vector<TaskListNode*>::iterator itr = m_pTaskList->begin();
    while (itr != m_pTaskList->end()) {
        if (*itr) {
            delete (*itr)->_task;
            delete (*itr);
        }
        itr++;
    }
    m_pTaskList->clear();
    delete m_pTaskList;

    // Cleanup Pending Task List
    itr = m_pPendingTaskList->begin();
    while (itr != m_pPendingTaskList->end()) {
        if (*itr) {
            delete (*itr)->_task;
            delete (*itr);
        }
        itr++;
    }
    m_pPendingTaskList->clear();
    delete m_pPendingTaskList;
}

void Scheduler::AddTask(Task *_task, unsigned int _tickStep, unsigned int _tickCount)
{
    TaskListNode *taskListNode = new TaskListNode;
    
    if (!_task) {
        f_printf("[Scheduler] Error: Task has not been initialized.\n");
        return;
    }

    _task->_dTime = 0.0f;
    _task->_dLastTime = 0.0f;
    taskListNode->_task = _task;
    taskListNode->_tickCount = _tickCount;
    taskListNode->_tickStep = _tickStep;
    taskListNode->_tickStepCurrent = _tickStep;
    taskListNode->_bEnabled = true;

    m_pPendingTaskList->push_back(taskListNode);

    //f_printf("Scheduler added pending task: %p, %i, %i\n", _task, _tickStep, _tickCount);
}

void Scheduler::RemoveTask(Task *_task)
{
    vector<TaskListNode*>::iterator itr;

    // Search for the task in the main list
    for (itr = m_pTaskList->begin(); itr != m_pTaskList->end(); itr++) {
        if ((*itr)) {
            if ((*itr)->_task == _task) {
                //delete ((*itr)->_task);
                delete (*itr);
                m_pTaskList->erase(itr);

#if defined(FOUNDATION_DEBUG)
    f_printf("[DEBUG] Task found and removed from main list.\n");
#endif

                return;
            }
        }
    }

    // Wasn't found yet, try the pending list
    for (itr = m_pPendingTaskList->begin(); itr != m_pPendingTaskList->end(); itr++) {
        if ((*itr)) {
            if ((*itr)->_task == _task) {
                //delete ((*itr)->_task);
                delete (*itr);
                m_pPendingTaskList->erase(itr);

#if defined(FOUNDATION_DEBUG)
    f_printf("[DEBUG] Task found and removed from pending list.\n");
#endif

                break;
            }
        }
    }

#if defined(FOUNDATION_DEBUG)
    f_printf("[Scheduler::RemoveTask] Warning: Task not found.\n");
#endif
}

void Scheduler::RunTask(Task *_task)
{
    _task->run(&_task->_dTime);
}

void Scheduler::KillTask(Task *_task)
{
    // TODO
}

void Scheduler::Step(float _fTimeElapsed)
{
    vector<TaskListNode*>::iterator itr, itrPrev;
    //f_printf("Elapsed Time: %f\n", _fTimeElapsed);

    // Determine the global dTime
    m_nGlobalAbsoluteTime = (float)TimeManager::getSingleton().getTime();

    // Walk through pending tasks and move them to the main task list
    while (m_pPendingTaskList->size() > 0) {
        m_pTaskList->push_back((*m_pPendingTaskList->begin()));
        m_pPendingTaskList->erase(m_pPendingTaskList->begin());
    }

    // Check the timing flag, if it's not set we need to modify dTime the first run
    //  only adjust this if we aren't using a fixed time step
    if ((m_nTimingFlag & 0x1) == 0 && (m_nTimingFlag & 0x2) == 0) {
        //f_printf("Timing Flag Set: %f\n", m_nGlobalAbsoluteTime);
        // First step through, initialize timings properly
        m_nTimingFlag |= 0x1;
        for (itr = m_pTaskList->begin(); itr != m_pTaskList->end(); itr++) {
            (*itr)->_task->_dLastTime = m_nGlobalAbsoluteTime;
        }
    } else {
        // Walk through our tasks and work on them
        for (itr = m_pTaskList->begin(); itr != m_pTaskList->end(); itr++) {
            if ((*itr) && (*itr)->_bEnabled) {
                (*itr)->_tickStepCurrent--;
    
                // If we're set to tick, go for it
                if ((*itr)->_tickStepCurrent == 0) {
                    if ((m_nTimingFlag & 0x2) == 0) {
                        // Use a global time difference for every task, instead of individual differences
                        (*itr)->_task->_dTime = (m_nGlobalAbsoluteTime - (*itr)->_task->_dLastTime) * _fTimeElapsed;
                    } else {
                        // Use a static value of 1.0 for delta time (tick)
                        (*itr)->_task->_dTime = m_nTimeStep;
                    }

                    //(*itr)->_task->_dTime = m_nGlobalDeltaTime * _fTimeElapsed;
                    //(*itr)->_task->_dLastTime = (float)TimeManager::getSingleton().getTime();

                    (*itr)->_task->_dLastTime = m_nGlobalAbsoluteTime;
                    //(*itr)->_task->_functionPointer((*itr)->_task->_this, &(*itr)->_task->_dTime);
                    RunTask((*itr)->_task);
                    (*itr)->_tickStepCurrent = (*itr)->_tickStep;
    
                    // If we don't go indefinitely (0), then decrement
                    if ((*itr)->_tickCount) {
                        (*itr)->_tickCount--;
    
                        // Remove our task if it's time has expired
                        if ((*itr)->_tickCount == 0) {
                            vector<TaskListNode*>::iterator itrPrev = itr - 1;
                            RemoveTask((*itr)->_task);
                            itr = itrPrev;
                        }
                    }
                }
            }
        }
    }
}

void Scheduler::SetStepTiming(bool _bStepTiming, float _nTimeStep)
{
    if (_bStepTiming)
        m_nTimingFlag |= 0x2;
    else
        m_nTimingFlag &= ~0x2;

    m_nTimeStep = _nTimeStep;
}

void Scheduler::SetTaskEnable(Task *_pTask, bool _bEnabled)
{
    vector<TaskListNode*>::iterator itr;

    // Search the Pending List for the task
    for (itr = m_pPendingTaskList->begin(); itr != m_pPendingTaskList->end(); itr++) {
        if ((*itr)->_task == _pTask) {
            (*itr)->_bEnabled = _bEnabled;
            return;
        }
    }

    // Search the Main Task List for the task
    for (itr = m_pTaskList->begin(); itr != m_pTaskList->end(); itr++) {
        if ((*itr)->_task == _pTask) {
            (*itr)->_bEnabled = _bEnabled;
            return;
        }
    }

    f_printf("[Scheduler::SetTaskEnable] Error: Cannot find task in Scheduler.\n");
}

void Scheduler::StepTask(Task *_pTask, float _nTimeElapsed)
{
    vector<TaskListNode*>::iterator itr;

    // Determine the global dTime
    m_nGlobalAbsoluteTime = (float)TimeManager::getSingleton().getTime();

    if (_pTask) {
        for (itr = m_pTaskList->begin(); itr != m_pTaskList->end(); itr++) {
            //
            // BE AWARE, the following code is copied from Step(), if you fix/change this
            //  please note the segment above. Maybe the future Mark or someone can pretty 
            //  this up into a single function. For now I'm undecided how to handle it
            //  gracefully.
            //
            if ((*itr)->_task == _pTask) {
                if ((m_nTimingFlag & 0x2) == 0) {
                    // Use a global time difference for every task, instead of individual differences
                    (*itr)->_task->_dTime = (m_nGlobalAbsoluteTime - (*itr)->_task->_dLastTime) * _nTimeElapsed;
                } else {
                    // Use a static value of 1.0 for delta time (tick)
                    (*itr)->_task->_dTime = m_nTimeStep;
                }

                (*itr)->_task->_dLastTime = m_nGlobalAbsoluteTime;
                //(*itr)->_task->_functionPointer((*itr)->_task->_this, &(*itr)->_task->_dTime);
                RunTask((*itr)->_task);
                (*itr)->_tickStepCurrent = (*itr)->_tickStep;

                // If we don't go indefinitely (0), then decrement
                if ((*itr)->_tickCount) {
                    (*itr)->_tickCount--;

                    // Remove our task if it's time has expired
                    if ((*itr)->_tickCount == 0) {
                        vector<TaskListNode*>::iterator itrPrev = itr - 1;
                        RemoveTask((*itr)->_task);
                        itr = itrPrev;
                    }
                }
                return;
            }
        }

        f_printf("[Scheduler::StepTask] Error: Cannot find task in Scheduler.\n");
    } else {
        f_printf("[Scheduler::StepTask] Error: Task is null.\n");
    }
}

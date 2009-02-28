# --------------------------------------------------
# Foundation Libs
from FoundationPython import *

# --------------------------------------------------
# Python Libs
import stackless

# --------------------------------------------------
# Globals
TASKSLEEPTIME = 100


class Task_ClassCallback:
    def __init__(self):
        pass

    def Task_MethodCallback(self, nDeltaTime):
        print "Task_MethodCallback Success:", nDeltaTime

def Task_Callback(nDeltaTime):
    print "Task_Callback Success:", nDeltaTime

# ------------------------------------------------
# Entry Point
def run(argv):
    Test_TimeManager = TimeManager()
    Test_Scheduler = Scheduler()
    Test_Task = Task(Task_Callback)
    Test_Class = Task_ClassCallback()
    Test_ClassTask = Task(Test_Class, "Task_MethodCallback")

    print Test_Task._kill
    print Test_Task._dTime
    print Test_Task._dLastTime

    Test_Scheduler.AddTask(Test_Task, 1, 0)
    Test_Scheduler.AddTask(Test_ClassTask, 1, 0)

    while (Test_TimeManager.getTime() < 1.0):
        Test_Scheduler.Step(1.0)
        Test_TimeManager.sleep(TASKSLEEPTIME)

    print "Cleaning up..."
    Test_Scheduler.RemoveTask(Test_Task)
    Test_Scheduler.RemoveTask(Test_ClassTask)


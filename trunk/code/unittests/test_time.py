# --------------------------------------------------
# Foundation Libs
from FoundationPython import *

# ------------------------------------------------
# Entry Point
def run(argv):
    FailCount = 0
    PassCount = 0
    TIME_ALLOWANCE = 0.001

    Test_TimeManager = TimeManager()
    Test_Timer = Timer()

    print "Running Time Test (Time Resolution = %fs)" % (TIME_ALLOWANCE)

    '''
    Test TimeManager
    '''
    print " Testing TimeManager..."
    nTime1 = Test_TimeManager.getTime()
    print "  Time: %f seconds" % (nTime1)
    print "  Sleeping 2 Seconds..."
    Test_TimeManager.sleep(2000)
    nTime2 = Test_TimeManager.getTime()
    print "  Time: %f seconds" % (nTime2)
    
    nTimeDiff = nTime2 - nTime1
    if ( nTimeDiff > 2.0 - TIME_ALLOWANCE and nTimeDiff < 2.0 + TIME_ALLOWANCE ):
        PassCount += 1
        print "  TimeManager Passed."
    else:
        FailCount += 1
        print "  TimeManager Failed."

    '''
    Test Timer Pause
    '''
    print " Testing Timer..."
    Test_Timer.reset()
    nTime1 = Test_Timer.getTime()
    print "  Timer: %f seconds" % (nTime1)
    print "  Pausing Timer for 2 seconds"
    Test_Timer.pause()
    Test_TimeManager.sleep(2000)
    nTime2 = Test_Timer.getTime()
    print "  Timer: %f seconds" % (nTime2)

    if (nTime1 > nTime2 - TIME_ALLOWANCE and nTime1 < nTime2 + TIME_ALLOWANCE):
        PassCount += 1
        print "  TimerPause Passed."
    else:
        FailCount += 1
        print "  TimerPause Failed."

    '''
    Test Timer Resume
    '''
    print "  Resuming Timer for 1.5 seconds"
    nTime1 = Test_Timer.getTime()
    Test_Timer.resume()
    Test_TimeManager.sleep(1500)
    nTime2 = Test_Timer.getTime()
    print "  Timer: %f seconds" % (nTime2)

    if (nTime1 + 1.5 > nTime2 - TIME_ALLOWANCE and nTime1 + 1.5 < nTime2 + TIME_ALLOWANCE):
        PassCount += 1
        print "  TimerResume Passed."
    else:
        FailCount += 1
        print "  TimerResume Failed."

    '''
    Completed
    '''
    print "Time Test Complete (%i Passed, %i Failed)\n" % (PassCount, FailCount)
    
    return FailCount

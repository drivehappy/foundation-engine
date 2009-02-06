# --------------------------------------------------
# Foundation Libs
from FoundationPython import *

FailCount = 0
PassCount = 0

def TestChannelACallback(_nChannelId, _nHeader, _nData, _nDataSize):
    global FailCount, PassCount

    print "  Test Channel A (ID = %i) Received Data:" % (_nChannelId)
    print "   Header: %i" % (_nHeader)
    print "   Data (size = %i): %s" % (_nDataSize, _nData)

    if _nHeader == 0x01:
        if _nData == "DUMMYDATA":
            PassCount += 1
            print " Channel Test 1 Passed"
        else:
            FailCount += 1
            print " Channel Test 1 Failed"
    elif _nHeader == 0x02:
        if _nData == "DUMMYDATAB":
            PassCount += 1
            print " Channel Test 2 Passed"
        else:
            FailCount += 1
            print " Channel Test 2 Failed"
    elif _nHeader == 0x03:
        FailCount += 1
        print " Channel Leave Test Failed, we received a message we weren't supposed to."
    else:
        FailCount += 1
        print "Channel Test Unknown Failed, Unknown Header"

# ------------------------------------------------
# Entry Point
def run(argv):
    global FailCount, PassCount

    Test_ChannelA = Channel()
    Test_ChannelB = Channel()
    
    print "Running Channel Test"

    '''
    Test Joining Channels
    '''
    print " Testing Message Sending..."
    print "  Joining Test Channel..."
    Test_ChannelA.Channel_Join("TEST_CHANNEL_A", TestChannelACallback)
    Test_ChannelB.Channel_Join("TEST_CHANNEL_A", TestChannelACallback)

    print "  Creating dummy message test 1..."
    Test_ChannelA.Channel_MessageCreate(9)
    Test_ChannelA.Channel_MessageHeader(0x01)
    Test_ChannelA.Channel_MessageData("DUMMYDATA", 9)
    Test_ChannelA.Channel_Send("TEST_CHANNEL_A")

    print "  Creating dummy message test 2..."
    Test_ChannelB.Channel_MessageCreate(10)
    Test_ChannelB.Channel_MessageHeader(0x02)
    Test_ChannelB.Channel_MessageData("DUMMYDATAB", 10)
    Test_ChannelB.Channel_Send("TEST_CHANNEL_A")

    print "  Leaving Channel..."
    Test_ChannelA.Channel_Leave("TEST_CHANNEL_A", TestChannelACallback)
    Test_ChannelB.Channel_Leave("TEST_CHANNEL_A", TestChannelACallback)

    print "  Test Ensuring We've Left the Test Channel..."
    Test_ChannelB.Channel_MessageCreate(1)
    Test_ChannelB.Channel_MessageHeader(0x03)
    Test_ChannelB.Channel_MessageData("A", 1)
    Test_ChannelB.Channel_Send("TEST_CHANNEL_A")

    '''
    Completed
    '''
    print "Channel Test Complete (%i Passed, %i Failed)\n" % (PassCount, FailCount)
    
    return FailCount

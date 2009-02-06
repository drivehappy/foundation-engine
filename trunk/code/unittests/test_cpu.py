# --------------------------------------------------
# Foundation Libs
from FoundationPython import *

# ------------------------------------------------
# Entry Point
def run(argv):
    FailCount = 0
    Test_CPUInfo = CPUInfo()
    Test_Debug = Debug().getCPUInfo(Test_CPUInfo)

    print "Running CPU Test..."
    print " CPU Info:"
    print "  VendorName:     %s" % (Test_CPUInfo.getVendorName())
    print "  Stepping:       %i" % (Test_CPUInfo.getStepping())
    print "  Model:          %i" % (Test_CPUInfo.getModel())
    print "  Family:         %i" % (Test_CPUInfo.getFamily())
    print "  Type:           %i" % (Test_CPUInfo.getType())
    print "  ExtendedModel:  %i" % (Test_CPUInfo.getExtendedModel())
    print "  ExtendedFamily: %i" % (Test_CPUInfo.getExtendedFamily())
    print "CPU Test Complete\n"

    return FailCount
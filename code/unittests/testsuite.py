import test_cpu
import test_time
import test_channel
import test_scheduler
import test_stackless

# ------------------------------------------------
# Entry Point
def main(argv):
    print "Running Test Suite...\n"

#    test_cpu.run(argv)
#    test_time.run(argv)
#    test_channel.run(argv)
#    test_scheduler.run(argv)
    test_stackless.run(argv)

    print "Test Suite Completed."

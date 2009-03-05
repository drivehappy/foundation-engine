# --------------------------------------------------
# Python Libs
import stackless

# --------------------------------------------------
# Actor
class Actor:
    """
    Actor class handles stackless tasklets, base class
    """
    
    def __init__(self):
        self.channel = stackless.channel()
        self.tasklet = self.__handleTasklet
        stackless.tasklet(self.__tasklet)()
        stackless.schedule()

    def __tasklet(self):
        while True:
            self.tasklet(self.channel.receive())
            stackless.schedule()

    def __handleTasklet(self, channelData):
        print "Actor.__handleTasklet:", channelData

    def shutdown(self):
        raise TaskletExit

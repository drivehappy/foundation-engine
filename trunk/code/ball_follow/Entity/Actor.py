# --------------------------------------------------
# Python Libs
import stackless

# --------------------------------------------------
# Actor
class Actor:
    """
    Actor class handles stackless tasklets, base class
    """
    
    def __init__(self, handleTaskletFunction):
        self.shutdownFlag = False
        self.channel = stackless.channel()
        self.tasklet = handleTaskletFunction
        stackless.tasklet(self.__tasklet)()
        stackless.schedule()

    def __tasklet(self):
        while True:
            if self.shutdownFlag:
                #print "Actor shutting down..."
                raise TaskletExit

            self.tasklet(self.channel.receive())
            #stackless.schedule()
            
    def shutdown(self):
        self.shutdownFlag = True

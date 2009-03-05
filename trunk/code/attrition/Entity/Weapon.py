# --------------------------------------------------
# Python Libs
import stackless

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation

from Log.HTTPLogger import HTTPLogger

from Entity.Actor import Actor
from Entity.Properties import Property

# --------------------------------------------------
# Unit
class Weapon(Actor):
    """
    Docstring.
    """

    def __init__(self):
        Actor.__init__(self)
        stackless.tasklet(self.update)()

    def update(self):
        while True:
            print "Weapon.update", Foundation.TimeManager().getTime()
            stackless.schedule()

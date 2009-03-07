# --------------------------------------------------
# Python Libs
import stackless

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation

from Log.HTTPLogger import HTTPLogger

# --------------------------------------------------
# Pipeline
class Pipeline():

    def __init__(self, owner, target):
        self.owner = owner
        self.target = target

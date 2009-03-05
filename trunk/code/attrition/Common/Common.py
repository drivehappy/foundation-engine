# --------------------------------------------------
# Common Messages
class Message:
    WORLD_STATE = 0
    UNIT_STATE  = 1
    CREATE_UNIT = 2

# --------------------------------------------------
#
class WorldState:
    def __init__(self, delta, total):
        self.deltaTime = 0.0
        self.totalTime = 0.0

# --------------------------------------------------
#
class UnitState:
    def __init__(self):
        self.dummy0 = "Test"


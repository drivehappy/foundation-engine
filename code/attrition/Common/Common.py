# --------------------------------------------------
# Common Messages
class Message:
    WORLD_STATE     = 0x00
    UNIT_STATE      = 0x01
    CREATE_UNIT     = 0x02
    UNIT_MOVE       = 0x10
    UNIT_SETTEAM    = 0x20

# --------------------------------------------------
# Tracks world state updates
class WorldState:
    def __init__(self, delta, total):
        self.deltaTime = delta
        self.totalTime = total

# --------------------------------------------------
#
class UnitState:
    def __init__(self):
        self.dummy0 = "Test"

# --------------------------------------------------
# Unit Team
class Team:
    UNKNOWN     = 0x00
    OBSERVER    = 0x01
    RED         = 0x10
    BLUE        = 0x11

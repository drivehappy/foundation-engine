# --------------------------------------------------
# Common Messages
class Message:
    WORLD_STATE = 0;
    CREATE_UNIT = 1;

# --------------------------------------------------
#
class WorldState:
    def __init__(self, delta, total):
        self.deltaTime = 0.0
        self.totalTime = 0.0


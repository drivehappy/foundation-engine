# --------------------------------------------------
# Common Messages
class Message:
    WORLD_STATE         = 0x00
    WORLD_GAMEREADY     = 0x01  # MsgData: PropertyList available for purchase
    PLAYER_GAMEREADY    = 0x02
    PLAYER_TURN         = 0x03
    PLAYER_TURN_DONE    = 0x04
    PLAYER_PURCHASE     = 0x05  # MsgData: [Index 0: False - Failure, True - Success]
                                # [Index 1: if i0 false: Available Properties, else Successful property]
    REQUEST_PURCHASE    = 0x06

    REQUEST_PROPERTIES  = 0x08
    RECEIVE_PROPERTIES  = 0x09

    REQUEST_CARD        = 0x0A
    RECEIVE_CARD        = 0x0B

    DERRICK_CREATE      = 0x10
    CAP_CREATE          = 0x11
    WELL_CREATE         = 0x12
    PROPERTY_CREATE     = 0x13

    CHECK_OWNER         = 0x20

    PLAYER_CASH_DELTA   = 0x30

    GAME_START          = 0x40

    RQ_PIPELINES_INTOME = 0x70

# --------------------------------------------------
# Tracks world state updates
class GameState:
    def __init__(self, state=''):
        self.state = state

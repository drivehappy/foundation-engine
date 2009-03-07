# --------------------------------------------------
# Python Libs
import stackless
import sys
import struct

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Log.HTTPLogger import *
import Entity.Actor
from Entity.World import *
import GUI.Helper
import Input.Manager

# --------------------------------------------------
# Managers
TimeManager     = Foundation.TimeManager()
Scheduler       = Foundation.Scheduler()
GraphicManager  = Foundation.GraphicManager()
InputManager    = Foundation.InputManager()
GUIManager      = Foundation.GUIManager()
GUIHelper       = GUI.Helper.Helper()
AudioManager    = Foundation.AudioManager()
PhysicsManager  = Foundation.PhysicsManager()
TerrainManager  = Foundation.TerrainManager()
FileManager     = Foundation.FileManager()
Camera0         = None

MouseEventChannel           = Foundation.Channel()
SelectionCallbackChannel    = Foundation.Channel()

# -----------------------------------------------
# Inputs
KEY_DELAY   = 0.015
KEYS        = [Foundation.Keycode.A, Foundation.Keycode.B, Foundation.Keycode.C,
               Foundation.Keycode.D, Foundation.Keycode.E, Foundation.Keycode.F,
               Foundation.Keycode.G, Foundation.Keycode.H, Foundation.Keycode.I, Foundation.Keycode.J,
               Foundation.Keycode.K, Foundation.Keycode.L, Foundation.Keycode.M, Foundation.Keycode.N,
               Foundation.Keycode.O, Foundation.Keycode.P, Foundation.Keycode.Q, Foundation.Keycode.R,
               Foundation.Keycode.S, Foundation.Keycode.T, Foundation.Keycode.U, Foundation.Keycode.V,
               Foundation.Keycode.W, Foundation.Keycode.X, Foundation.Keycode.Y, Foundation.Keycode.Z,
               Foundation.Keycode.NUMPAD_0, Foundation.Keycode.NUMPAD_1, Foundation.Keycode.NUMPAD_2,
               Foundation.Keycode.NUMPAD_3, Foundation.Keycode.NUMPAD_4, Foundation.Keycode.NUMPAD_5,
               Foundation.Keycode.NUMPAD_6, Foundation.Keycode.NUMPAD_7, Foundation.Keycode.NUMPAD_8,
               Foundation.Keycode.NUMPAD_9,
               Foundation.Keycode.SPACE,
               Foundation.Keycode.ESCAPE]

TimerKeyDelay       = Foundation.Timer()
MouseStateChange    = Input.Manager.MouseState()
KeyboardStateChange = Input.Manager.KeyboardState()
Joystick0StateChange = Input.Manager.JoystickState()
Joystick1StateChange = Input.Manager.JoystickState()

CAMERA_SPEED = 1000

SelectionBounds         = Foundation.Vector4(0, 0, 0, 0)
SelectionWorldBounds    = Foundation.Vector4(0, 0, 0, 0)
SelectedEntityList      = []

# ------------------------------------------------
# Main Tasklets
def schedulerTasklet():
    global EntityManager
    
    uMainTimer = Foundation.Timer()
    nDeltaTime = 0
    while True:
        nDeltaTime = uMainTimer.getTime()
        uMainTimer.reset()
        nDeltaTime = Foundation.f_clamp(nDeltaTime, 0.0, 1.0)

        # Update internals
        Scheduler.Step(1.0)

        TimeManager.sleep(1)

        stackless.schedule()

# ------------------------------------------------
# Entry Point
def main(argv):
    global EntityManager

    try:
        HTTPLogger("../oilking_log.html")
        HTTPLogger().newTable("Foundation Engine (Project: Oil King)", "Description")
        HTTPLogger().writeContent(LoggerError.SUCCESS, "Initialized (%s/%s)" % (sys.path[0], sys.argv[0]))

        stackless.tasklet(schedulerTasklet)()

        # Spin up some units
        Game = World(18, 4)

        try:
            stackless.run()
        except TaskletExit:
            raise

    except KeyboardInterrupt:
        print "\n"

    shutdown()

# ------------------------------------------------
# Shutdown
def shutdown():
    # Cleanup
    HTTPLogger().writeContent(LoggerError.SUCCESS, "Shutdown Complete")
    HTTPLogger().endTable()
    HTTPLogger().closeLog()


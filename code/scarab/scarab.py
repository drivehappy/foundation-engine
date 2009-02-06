# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from input.input import *
import gui.gui
import entity.EntityManager
import random

# --------------------------------------------------
# Python Libs
import psyco
psyco.full()

# --------------------------------------------------
# Managers
TimeManager     = Foundation.TimeManager()
Scheduler       = Foundation.Scheduler()
GraphicManager  = Foundation.GraphicManager()
InputManager    = Foundation.InputManager()
GUIManager      = Foundation.GUIManager()
GUIHelper       = gui.gui.GUIHelper()
AudioManager    = Foundation.AudioManager()
PhysicsManager  = Foundation.PhysicsManager()
TerrainManager  = Foundation.TerrainManager()
EntityManager   = entity.EntityManager.EntityManager("../../data/scarabEntityTypes.yaml")
Camera0         = None

MouseEventChannel   = Foundation.Channel()

# ----
# States
STATES = {
    "game": 0,
}
GAMESTATES  =  {
                "init":      0,
                "title":     1,
                "play":      2,
                "end":       3,
                "destroy":   4,            
               }
GameState   = GAMESTATES["init"]


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
MouseStateChange    = MouseState()
KeyboardStateChange = KeyboardState()
Joystick0StateChange = JoystickState()
Joystick1StateChange = JoystickState()

CAMERA_SPEED = 1000

# ------------------------------------------------
# InputWork
def doInput(_nDeltaTime):
    global MouseStateChange, KeyboardStateChange, Joystick0StateChange, Joystick1StateChange
    global TimerKeyDelay

    mouseState, keyboardState, joystickState = consumeEvent(InputManager, GUIManager)

    # Keyboard
    if keyboardState and TimerKeyDelay.getTime() > KEY_DELAY:
        for KeyIndex in keyboardState.Keys:
            if keyboardState.Keys[KeyIndex]:
                if KeyIndex == Foundation.Keycode.ESCAPE:
                    doStateTransition(STATES["game"], GAMESTATES["end"])

                nCamSpeed = _nDeltaTime * CAMERA_SPEED
                if KeyIndex == Foundation.Keycode.A:
                    Camera0.moveRelative(Foundation.Vector3(-nCamSpeed, 0, 0))
                elif KeyIndex == Foundation.Keycode.W:
                    Camera0.moveRelative(Foundation.Vector3(0, nCamSpeed, 0))

                if KeyIndex == Foundation.Keycode.S:
                    Camera0.moveRelative(Foundation.Vector3(0, -nCamSpeed, 0))
                elif KeyIndex == Foundation.Keycode.D:
                    Camera0.moveRelative(Foundation.Vector3(nCamSpeed, 0, 0))

                if KeyIndex == Foundation.Keycode.Z:
                    Camera0.moveRelative(Foundation.Vector3(0, 0.0, -nCamSpeed))
                elif KeyIndex == Foundation.Keycode.X:
                    Camera0.moveRelative(Foundation.Vector3(0, 0, nCamSpeed))

                if KeyIndex == Foundation.Keycode.NUMPAD_8:
                    Camera0.setLookAt(Foundation.Vector3(0, 0, 10))

        KeyboardStateChange.assign(keyboardState)
        TimerKeyDelay.reset()

    # Mouse
    if mouseState:
        MouseStateChange.assign(mouseState)
    
    # Joystick
    if joystickState:
        if (joystickState.Index == 0):
            Joystick0StateChange.assign(joystickState)
        elif (joystickState.Index == 1):
            Joystick1StateChange.assign(joystickState)

    if joystickState:
        pass

# ------------------------------------------------
# Initialize Managers
def initManagers():
    global TimeManager, Schdeuler, AudioManager, InputManager, GraphicManager, GUIManager, MouseEventChannel, Camera0
    
    # Init Audio
    print " - [AudioManager] Initializing..."
    bResult = AudioManager.initialize()
    if bResult:
        nAudioDeviceCount = AudioManager.getDriverCount()
        print " - [AudioManager] Found %i devices:\n" % (nAudioDeviceCount)
        sAudioDeviceName = ""
        for nIndex in range(0, nAudioDeviceCount):
            sAudioDeviceName += AudioManager.getDriverInfo(nIndex) + ", "
        print " - [AudioManager] Devices: %s\n" % (sAudioDeviceName)
        Scheduler.AddTask(AudioManager.getTaskUpdate(), 0, 1)
        print " - [AudioManager] Success."
    else:
        print " - [AudioManager] Error."
        return False

    # Init Physics
    print " - [PhysicsManager] Initializing..."
    Scheduler.AddTask(PhysicsManager.getTaskUpdate(), 1, 0)
    print " - [PhysicsManager] Success."

    # Init Graphics
    print " - [GraphicManager] Initializing..."
    bResult = GraphicManager.initialize("Scarab")
    if bResult:
        GraphicManager.showCursor(False)
        GraphicManager.addSceneManager("SceneManager0")
        TerrainManager.create("SceneManager0")
        Camera0 = GraphicManager.addCamera("SceneManager0", "Camera0", 0, 0.0, 0.0, 1.0, 1.0)
        Camera0.setPosition(Foundation.Vector3(0, 500, 0.01))
        Camera0.setLookAt(Foundation.Vector3(0, 0, 0))
        Scheduler.AddTask(GraphicManager.getTaskRender(), 1, 0)
        print " - [GraphicManager] Success."
    else:
        print " - [GraphicManager] Error - check ogre.log for more information."
        return False

    # Init Input
    print " - [InputManager] Initializing..."
    nWindowHandle = GraphicManager.getWindowHandle()
    InputManager.initialize(nWindowHandle)
    Scheduler.AddTask(InputManager.getTaskCapture(), 1, 0)

    # Init our mouse button event callback function
    MouseEventChannel.Channel_Join("MOUSE_EVENTS", onMouseEvent)
    print " - [InputManager] Success."

    # Init GUI
    print " - [GUIManager] Initializing..."
    GUIManager.initialize(GraphicManager.getRenderWindow())
    print " - [GUIManager] Success."

    # Init EntityManager
    print " - [EntityManager] Initializing..."
    Scheduler.AddTask(EntityManager.Task, 1, 0)
    print " - [EntityManager] Success."

    return True


# ------------------------------------------------
# Cleanup Managers
def cleanupManagers():
    global TimeManager, Schdeuler, AudioManager, InputManager, GraphicManager, GUIManager

    print " - [Scheduler] Shutting down..."
    if Scheduler:
        if EntityManager:
            print " - [EntityManager] Shutting down..."
            EntityManagerTask = Foundation.Task(EntityManager, "doTask")
            Scheduler.RemoveTask(EntityManagerTask)
            print " - [EntityManager] Complete."

        if AudioManager:
            print " - [AudioManager] Shutting down..."
            Scheduler.RemoveTask(AudioManager.getTaskUpdate())
            AudioManager.destroy()
            AudioManager = None
            print " - [AudioManager] Complete."

        if GUIManager:
            print " - [GUIManager] Shutting down..."
            GUIManager.destroy()
            GUIManager = None
            print " - [GUIManager] Complete."

        if InputManager:
            print " - [InputManager] Shutting down..."
            Scheduler.RemoveTask(InputManager.getTaskCapture())
            InputManager.destroy()
            InputManager = None
            print " - [InputManager] Complete."

        if PhysicsManager:
            print " - [PhysicsManager] Shutting down..."
            Scheduler.RemoveTask(PhysicsManager.getTaskUpdate())
            print " - [PhysicsManager] Complete."

        if GraphicManager:
            print " - [GraphicManager] Shutting down..."
            GraphicManager.showCursor(True)
            Scheduler.RemoveTask(GraphicManager.getTaskRender())
            GraphicManager.destroy()
            GraphicManager = None
            print " - [GraphicManager] Complete."

        print " - [Scheduler] Complete."
    else:
        print " - [Scheduler] Error: Scheduler is already destroyed."


# --------------------------------------------------
# MouseEventCallback
def onMouseEvent(channel, header, data, size):
    
    # Mouse move
    sButton = data
    if header == 2:
        if sButton == "Button_Start":
            pass
            
    elif header == 3:
        pass

# ------------------------------------------------
# State Machine Transitions
def doStateTransition(_nSuperState, _nTargetState):
    global GameState

    # Don't transition into a state we're already in
    if _nSuperState == STATES["game"]:
        if _nTargetState == GameState:
            return

        elif _nTargetState == GAMESTATES["init"]:
            pass

        elif _nTargetState == GAMESTATES["title"]:
            pass

        elif _nTargetState == GAMESTATES["play"]:
            pass

        elif _nTargetState == GAMESTATES["end"]:
            pass

        elif _nTargetState == GAMESTATES["destroy"]:
            print "Destroying..."
            GUIHelper.cleanupGameUI()
            cleanupManagers()

        GameState = _nTargetState

# ------------------------------------------------
# Entry Point
def main(argv):
    global TimeManager, Schdeuler, AudioManager, InputManager, GraphicManager, GUIManager
    global GameState

    doStateTransition(STATES["game"], GAMESTATES["init"])
    initManagers()

    GUIHelper.createGameUI()

    # -----------------------------------
    # TESTING ENTITIES
    for x in range(0, 10):
        NewEntity = EntityManager.addEntity("MyNewEntity", "Scout")
        NewEntity.createGraphic("SceneManager0")
        NewEntity.createCollision(Foundation.CollisionShapeId.BOX, Foundation.Vector3(1, 1, 1))
        NewEntity.setPosition(Foundation.Vector3(x * 20, 20, 0))
        #NewEntity.moveTo(Foundation.Vector3(random.randrange(-1000, 1000, 0), random.randrange(-1000, 1000, 0), 0))
    # -----------------------------------

    uMainTimer = Foundation.Timer()
    nDeltaTime = 0
    while GameState != GAMESTATES["end"]:
        nDeltaTime = uMainTimer.getTime()
        uMainTimer.reset()
        nDeltaTime = Foundation.f_clamp(nDeltaTime, 0.0, 1.0)

        # Update Input
        doInput(nDeltaTime)

        # Update GUI
        GUIHelper.updateGameUI(nDeltaTime, TimeManager.getTime(), GraphicManager.getAverageFPS())

        Scheduler.Step(1.0)

        #TimeManager.sleep(1)

    doStateTransition(STATES["game"], GAMESTATES["destroy"])
    print "Scarab Done."

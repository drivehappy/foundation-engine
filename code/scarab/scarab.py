# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from input.input import *
import gui.gui
import entity.EntityManager
from log.HTTPLogger import *

# --------------------------------------------------
# Python Libs
import stackless
import random
import struct
import math

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
EntityManager   = entity.EntityManager.EntityManager("../../data/scarabEntityTypes.yaml", "../../data/scarabWeaponTypes.yaml")
FileManager     = Foundation.FileManager()
Camera0         = None
Logger          = None

MouseEventChannel           = Foundation.Channel()
SelectionCallbackChannel    = Foundation.Channel()

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

SelectionBounds         = Foundation.Vector4(0, 0, 0, 0)
SelectionWorldBounds    = Foundation.Vector4(0, 0, 0, 0)
SelectedEntityList      = []

# ------------------------------------------------
# InputWork
def doInput(_nDeltaTime):
    global MouseStateChange, KeyboardStateChange, Joystick0StateChange, Joystick1StateChange
    global TimerKeyDelay
    global SelectionBounds, SelectionWorldBounds, SelectedEntityList

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
        # Our left mouse button just went down, create an intersection point
        if mouseState.Button0 and not MouseStateChange.Button0:
            print ("MouseDown (%i, %i), Creating Selection Frustrum..." % (mouseState.AxisX, mouseState.AxisY))
            SelectionBounds[0] = mouseState.AxisX
            SelectionBounds[1] = mouseState.AxisY

            # FIXME: Screen Width/Height Variable
            SelectionPoint = Foundation.Vector2(SelectionBounds[0], SelectionBounds[1])
            nIntersectionPoint = TerrainManager.getRayIntersection("SceneManager0", "Camera0", SelectionPoint, Foundation.Vector2(1024, 768))
            SelectionWorldBounds[0] = nIntersectionPoint[0]
            SelectionWorldBounds[1] = nIntersectionPoint[2]
            print "TerrainIntersection: %f, %f, %f" % (nIntersectionPoint[0], nIntersectionPoint[1], nIntersectionPoint[2])

            SelectedEntityList = []

        # Out left mouse button is still down, update our selection line
        elif mouseState.Button0 and MouseStateChange.Button0:
            SelectionPoint = Foundation.Vector2(mouseState.AxisX, mouseState.AxisY)

            # FIXME: Screen Width/Height Variable
            nIntersectionPoint = TerrainManager.getRayIntersection("SceneManager0", "Camera0", SelectionPoint, Foundation.Vector2(1024, 768))
            SelectionWorldBounds[2] = nIntersectionPoint[0]
            SelectionWorldBounds[3] = nIntersectionPoint[2]

            nTopLeft        = (SelectionWorldBounds[0], 20, SelectionWorldBounds[1])
            nTopRight       = (SelectionWorldBounds[2], 20, SelectionWorldBounds[1])
            nBottomLeft     = (SelectionWorldBounds[0], 20, SelectionWorldBounds[3])
            nBottomRight    = (SelectionWorldBounds[2], 20, SelectionWorldBounds[3])

            GraphicManager.updateLine("SceneManager0", "SELECTION_LINELIST", [nTopLeft, nTopRight, nBottomRight, nBottomLeft, nTopLeft])

        # Our left mouse button just went up, do picking across our selection area
        elif not mouseState.Button0 and MouseStateChange.Button0:
            print ("MouseUp (%i, %i), Selection Frustrum Created." % (mouseState.AxisX, mouseState.AxisY))
            SelectionBounds[2] = mouseState.AxisX
            SelectionBounds[3] = mouseState.AxisY
            print " Selection Frustrum:", SelectionBounds

            # FIXME: Screen Width/Height Variable
            GraphicManager.doPicking("SceneManager0", "Camera0", 0x1, SelectionBounds, Foundation.Vector2(1024, 768))

            GraphicManager.updateLine("SceneManager0", "SELECTION_LINELIST", [(0, 0, 0)])

            print "Total Selection:", SelectedEntityList

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
    HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Initializing...")
    bResult = AudioManager.initialize()
    if bResult:
        nAudioDeviceCount = AudioManager.getDriverCount()
        HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Found %i devices:" % (nAudioDeviceCount))
        sAudioDeviceName = ""
        for nIndex in range(0, nAudioDeviceCount):
            sAudioDeviceName += AudioManager.getDriverInfo(nIndex) + ", "
        HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Devices: %s" % (sAudioDeviceName))
        Scheduler.AddTask(AudioManager.getTaskUpdate(), 0, 1)
        HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Success.")
    else:
        HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Error.")
        return False

    # Init Physics
    HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Initializing...")
    Scheduler.AddTask(PhysicsManager.getTaskUpdate(), 1, 0)
    HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Success.")

    # Init Graphics
    HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Initializing...")
    bResult = GraphicManager.initialize("Scarab")
    if bResult:
        GraphicManager.showCursor(False)
        GraphicManager.addSceneManager("SceneManager0")
        TerrainManager.create("SceneManager0")
        Camera0 = GraphicManager.addCamera("SceneManager0", "Camera0", 0, 0.0, 0.0, 1.0, 1.0)
        Camera0.setPosition(Foundation.Vector3(0, 500, 0.01))
        Camera0.setLookAt(Foundation.Vector3(0, 0, 0))
        GraphicManager.addLine("SceneManager0", "SELECTION_LINELIST", [(0, 0, 0)], (0.0, 1.0, 0.0))

        SelectionCallbackChannel.Channel_Join("GRAPHICS_SELECTION", onSelection)

        Scheduler.AddTask(GraphicManager.getTaskRender(), 1, 0)
        HTTPLogger().writeContent(LoggerError.NONE, "[GraphicManager] Success.")
    else:
        HTTPLogger().writeContent(LoggerError.ERROR, "[AudioManager] Check ogre.log for more information.")
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

    HTTPLogger().writeContent(LoggerError.NONE, "[Scheduler] Shutting down...")
    if Scheduler:
        if EntityManager:
            HTTPLogger().writeContent(LoggerError.NONE, "[EntityManager] Shutting down...")
            Scheduler.RemoveTask(EntityManager.Task)
            HTTPLogger().writeContent(LoggerError.NONE, "[EntityManager] Complete.")

        if AudioManager:
            HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Shutting down...")
            Scheduler.RemoveTask(AudioManager.getTaskUpdate())
            AudioManager.destroy()
            AudioManager = None
            HTTPLogger().writeContent(LoggerError.NONE, "[AudioManager] Complete.")

        if GUIManager:
            HTTPLogger().writeContent(LoggerError.NONE, "[GUIManager] Shutting down...")
            GUIManager.destroy()
            GUIManager = None
            HTTPLogger().writeContent(LoggerError.NONE, "[EntiGUIManagertyManager] Complete.")

        if InputManager:
            HTTPLogger().writeContent(LoggerError.NONE, "[InputManager] Shutting down...")
            Scheduler.RemoveTask(InputManager.getTaskCapture())
            InputManager.destroy()
            InputManager = None
            HTTPLogger().writeContent(LoggerError.NONE, "[InputManager] Complete.")

        if PhysicsManager:
            HTTPLogger().writeContent(LoggerError.NONE, "[PhysicsManager] Shutting down...")
            Scheduler.RemoveTask(PhysicsManager.getTaskUpdate())
            HTTPLogger().writeContent(LoggerError.NONE, "[PhysicsManager] Complete.")

        if GraphicManager:
            HTTPLogger().writeContent(LoggerError.NONE, "[GraphicManager] Shutting down...")
            GraphicManager.showCursor(True)
            Scheduler.RemoveTask(GraphicManager.getTaskRender())
            GraphicManager.destroy()
            GraphicManager = None
            HTTPLogger().writeContent(LoggerError.NONE, "[GraphicManager] Complete.")

        HTTPLogger().writeContent(LoggerError.NONE, "[Scheduler] Complete.")
    else:
        HTTPLogger().writeContent(LoggerError.ERROR, "[Scheduler] Scheduler is already destroyed.")


# --------------------------------------------------
# MouseEventCallback
def onMouseEvent(channel, header, data, size):
    
    # Mouse move
    sButton = data
    if header == 2:
        for i in range(0, 9):
            if sButton == "Btn_Entity_Create" + str(i):
                #print sButton, "pressed", "ENTITY =", SelectedEntityList[0].getCreationAbilities()[i]
                sType = SelectedEntityList[0].getCreationAbilities()[i]
                SelectedEntityList[0].createUnit(EntityManager.getEntityTypeFromName(sType))
                print "+ GUI Selected Unit of Type %s From Unit %s" % (sType, SelectedEntityList[0])

            
    elif header == 3:
        pass

# --------------------------------------------------
# Selection Callback
def onSelection(channel, header, data, size):
    global SelectedEntityList

    #print "Selection Channel Callback Received"

    nObjectNameLen = len(data) - 13
    nObjectPosition = struct.unpack("fff", data[nObjectNameLen:nObjectNameLen+12])
    nObjectName = data[:nObjectNameLen]

    HTTPLogger().writeContent(LoggerError.NONE, "Selected Object: " + nObjectName + " at " + str(nObjectPosition[0]) + "," + str(nObjectPosition[1]) + "," + str(nObjectPosition[2]))
    #print "Name:", nObjectName, "Position:", nObjectPosition[0], nObjectPosition[1], nObjectPosition[2]

    nEntityId = nObjectName[:len(nObjectName) - len("_GRAPHIC")]
    uEntity = EntityManager.getEntityById(nEntityId)
    SelectedEntityList.append(uEntity)

# ------------------------------------------------
# State Machine Transitions
def doStateTransition(_nSuperState, _nTargetState):
    global GameState

    HTTPLogger().writeContent(LoggerError.NONE, "Game State Transition: " + str(GameState) + "=>" + str(_nTargetState))

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
    global SelectedEntityList
    global Logger
    
    print "Starting up..."

    HTTPLogger("../scarab_log.html")
    HTTPLogger().newTable("Foundation Engine (scarab)", "Time (s)", "Description")

    doStateTransition(STATES["game"], GAMESTATES["init"])
    initManagers()

    GUIHelper.createGameUI()
    GUIHelper.createEntityUI()

    # -----------------------------------
    # TESTING ENTITIES
    '''
    for x in range(0, 100):
        uUnitType = EntityManager.getEntityTypeFromName("Scout")
        NewEntity = EntityManager.addEntity("MyNewEntity" + str(x), uUnitType)
        NewEntity.createGraphic("SceneManager0")
        NewEntity.createCollision(Foundation.CollisionShapeId.BOX, Foundation.Vector3(1, 1, 1))
        NewEntity.setPosition(Foundation.Vector3((x / 10) * 20, 20, (x % 10) * 20))
        NewEntity.moveTo(Foundation.Vector3(random.randrange(-1000, 1000, 1), 20, random.randrange(-1000, 1000, 1)))
        #Scheduler.AddTask(NewEntity.Task, 1, 0)
    '''
    
    # TEST BUILDING ENTITIES
    uUnitType = EntityManager.getEntityTypeFromName("CommandCenter")
    CommandCenter = EntityManager.addEntity("CommandCenter0", uUnitType)
    CommandCenter.createGraphic("SceneManager0")
    CommandCenter.createCollision(Foundation.CollisionShapeId.BOX, Foundation.Vector3(4, 4, 4))
    CommandCenter.setPosition(Foundation.Vector3(0, 20, 0))
    #Scheduler.AddTask(CommandCenter.Task, 1, 0)
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
        if len(SelectedEntityList) > 0:
            GUIHelper.updateEntityUI(nDeltaTime, SelectedEntityList[0])

        # Update EntityManager
        #EntityManager.doTask()

        Scheduler.Step(1.0)

        #TimeManager.sleep(1)

    doStateTransition(STATES["game"], GAMESTATES["destroy"])
    HTTPLogger().writeContent(LoggerError.NONE, "Done.")
    HTTPLogger().endTable()
    print "Scarab Done."

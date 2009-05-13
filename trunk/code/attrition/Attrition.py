# --------------------------------------------------
# Python Libs
import stackless
import sys
import struct
import random

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Log.HTTPLogger import *
import Entity.Actor
import Entity.Manager
import Entity.World
import Entity.Unit
from Common.Common import Team
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
EntityManager   = None
FileManager     = Foundation.FileManager()
Camera0         = None

MouseEventChannel           = Foundation.Channel()
SelectionCallbackChannel    = Foundation.Channel()

# -----------------------------------------------
# Inputs
KEY_DELAY   = 0.010
LARGE_KEY_DELAY = 0.10
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

# Sphere Tree Demo CS709
RenderSphereTree = True
RenderSphereTreeLevel = 5
Pause = False
SphereTreeBucketSize = 3

# ------------------------------------------------
# InputWork
def doInput(_nDeltaTime):
    global MouseStateChange, KeyboardStateChange, Joystick0StateChange, Joystick1StateChange
    global TimerKeyDelay
    global SelectionBounds, SelectionWorldBounds, SelectedEntityList
    global RenderSphereTree, Pause, SphereTreeBucketSize

    resetKey = False
    mouseState, keyboardState, joystickState = Input.Manager.consumeEvent(InputManager, GUIManager)

    # Keyboard
    if keyboardState and TimerKeyDelay.getTime() > KEY_DELAY:
        for KeyIndex in keyboardState.Keys:
            if keyboardState.Keys[KeyIndex]:
                if KeyIndex == Foundation.Keycode.ESCAPE:
                    return False

                if KeyIndex == Foundation.Keycode.NUMPAD_8:
                    Camera0.setLookAt(Foundation.Vector3(0, 0, 10))
                    
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
                    Camera0.moveRelative(Foundation.Vector3(0, 0, -nCamSpeed))
                elif KeyIndex == Foundation.Keycode.X:
                    Camera0.moveRelative(Foundation.Vector3(0, 0, nCamSpeed))

                if (TimerKeyDelay.getTime() > LARGE_KEY_DELAY):                    
                    if KeyIndex == Foundation.Keycode.I:
                        SphereTreeBucketSize -= 1
                        if (SphereTreeBucketSize < 2):
                            SphereTreeBucketSize = 2
                    elif KeyIndex == Foundation.Keycode.O:
                        SphereTreeBucketSize += 1
                        if (SphereTreeBucketSize > 20):
                            SphereTreeBucketSize = 20

                    if KeyIndex == Foundation.Keycode.SPACE:
                        EntityManager.sphereTree.dump()
                    elif KeyIndex == Foundation.Keycode.B:
                        RenderSphereTree = not RenderSphereTree
                        print "RenderSphereTree: ", RenderSphereTree
                    elif KeyIndex == Foundation.Keycode.P:
                        Pause = not Pause
                        print "Simulation Pause: ", Pause
                        
                resetKey = True           
            
        if resetKey:
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

    return True

# ------------------------------------------------
# Initialize Managers
def initManagers():
    global TimeManager, Schdeuler, AudioManager, InputManager, GraphicManager, EntityManager
    global GUIManager, MouseEventChannel, Camera0

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
    HTTPLogger().writeContent(LoggerError.NONE, "[PhysicsManager] Initializing...")
    Scheduler.AddTask(PhysicsManager.getTaskUpdate(), 1, 0)
    HTTPLogger().writeContent(LoggerError.NONE, "[PhysicsManager] Success.")

    # Init Graphics
    HTTPLogger().writeContent(LoggerError.NONE, "[GraphicManager] Initializing...")
    bResult = GraphicManager.initialize("Attrition - Sphere Tree Demo CS709")
    if bResult:
        GraphicManager.showCursor(False)
        GraphicManager.addSceneManager("SceneManager0")
        Camera0 = GraphicManager.addCamera("SceneManager0", "Camera0", 0, 0.0, 0.0, 1.0, 1.0)
        Camera0.setPosition(Foundation.Vector3(0, 500, 0.01))
        Camera0.setLookAt(Foundation.Vector3(0, 0, 0))
        GraphicManager.addLine("SceneManager0", "SELECTION_LINELIST", [(0, 0, 0)], (0.0, 1.0, 0.0))

        TerrainManager.create("SceneManager0", "Camera0")
        SelectionCallbackChannel.Channel_Join("GRAPHICS_SELECTION", onSelection)

        Scheduler.AddTask(GraphicManager.getTaskRender(), 1, 0)
        HTTPLogger().writeContent(LoggerError.NONE, "[GraphicManager] Success.")
    else:
        HTTPLogger().writeContent(LoggerError.ERROR, "[GraphicManager] Check ogre.log for more information.")
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
    EntityManager = Entity.Manager.Manager("../../data/scarabEntityTypes.yaml", "../../data/scarabWeaponTypes.yaml")
    #Scheduler.AddTask(EntityManager.Task, 1, 0)
    print " - [EntityManager] Success."

    return True


# ------------------------------------------------
# Cleanup Managers
def cleanupManagers():
    global TimeManager, Schdeuler, AudioManager, InputManager, GraphicManager, GUIManager, EntityManager

    HTTPLogger().writeContent(LoggerError.NONE, "[Scheduler] Shutting down...")
    if Scheduler:
        if EntityManager:
            HTTPLogger().writeContent(LoggerError.NONE, "[EntityManager] Shutting down...")
            EntityManager.sphereTree.destroy()
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
            HTTPLogger().writeContent(LoggerError.NONE, "[GUIManager] Complete.")

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
                if (len(SelectedEntityList) > 0) :
                    sType = SelectedEntityList[0].creationAbilities[i]
                    SelectedEntityList[0].createUnit(EntityManager.getEntityTypeFromName(sType))
                    print "+ GUI Selected Unit of Type %s From Unit %s" % (sType, SelectedEntityList[0])
                else:
                    print "No Entity Selected"

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
    uEntity = EntityManager.getEntityByName(nEntityId)

    if uEntity:
        SelectedEntityList.append(uEntity)

# ------------------------------------------------
# Main Tasklets
def schedulerTasklet():
    global EntityManager, PhysicsManager
    global RenderSphereTree, Pause, SphereTreeBucketSize
    
    uMainTimer = Foundation.Timer()
    nDeltaTime = 0
    while True:
        nDeltaTime = uMainTimer.getTime()
        uMainTimer.reset()
        nDeltaTime = Foundation.f_clamp(nDeltaTime, 0.0, 0.1)

        # Update Input
        if not doInput(nDeltaTime):
            print "EntityManager shutting down units..."
            EntityManager.shutdownUnits()
            print "Done"
            break

        # Update GUI
        entityCount = EntityManager.getEntityCount()
        GUIHelper.updateGameUI(nDeltaTime, TimeManager.getTime(), GraphicManager.getAverageFPS(), entityCount, SphereTreeBucketSize)
        if len(SelectedEntityList) > 0:
            GUIHelper.updateEntityUI(nDeltaTime, SelectedEntityList[0])

        # Update internals
        Scheduler.Step(1.0)
        TimeManager.sleep(1)
        
        EntityManager.sphereTree.setMaxBucketSize(SphereTreeBucketSize)
        EntityManager.sphereTree.update()
        if RenderSphereTree:
            EntityManager.sphereTree.debugRender("SceneManager0")
        else:
            EntityManager.sphereTree.clearDebugRender("SceneManager0")

        PhysicsManager.setPaused(Pause)
        if (not Pause):
            #EntityManager.deltaTime = nDeltaTime
            
            

            stackless.schedule()      

def buildingInitTasklet():
    uUnitType = EntityManager.getEntityTypeFromName("CommandCenter")
    CommandCenter = EntityManager.addEntity("CommandCenter0", uUnitType)
    CommandCenter.createGraphic("SceneManager0")
    CommandCenter.createCollision(Foundation.CollisionShapeId.BOX, Foundation.Vector3(4, 4, 4))
    CommandCenter.setPosition(Foundation.Vector3(0, 20, 0))

# ------------------------------------------------
# Entry Point
def main(argv):
    global EntityManager

    try:
        HTTPLogger("../attrition_log.html")
        HTTPLogger().newTable("Foundation Engine (Project: Attrition)", "Description")
        HTTPLogger().writeContent(LoggerError.SUCCESS, "Initialized (%s/%s)" % (sys.path[0], sys.argv[0]))

        initManagers()
        
        GUIHelper.createGameUI()
        GUIHelper.createEntityUI()

        # TEST CIRCLE RENDERING
        '''
        for x in range(0, 1000):
            nX = random.randint(-500, 500)
            nZ = random.randint(-500, 500)
            nRadius = random.random() * 50
            GraphicManager.addCircle("SceneManager0", "CIRCLETEST" + str(x), Foundation.Vector3(nX, 13.0, nZ), nRadius, Foundation.VectorIndex.Yelt, 1, 0, 0)
        '''
        # -----------------------------------

        # Spin up some units
        CommandCenter = EntityManager.addUnit("CommandCenter", Foundation.Vector3(400, 20, 400))
        CommandCenter.team = Team.RED

        CommandCenter = EntityManager.addUnit("CommandCenter", Foundation.Vector3(-400, 20, -400))
        CommandCenter.team = Team.BLUE

        for i in range(0, 20):
            Scout = EntityManager.addUnit("Scout", Foundation.Vector3(-400, 20, -400))
            Scout.team = Team.RED

            Scout = EntityManager.addUnit("Scout", Foundation.Vector3(400, 20, 400))
            Scout.team = Team.BLUE

        # Start
        stackless.tasklet(schedulerTasklet)()

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
    print "Shutting down..."

    # Cleanup
    GUIHelper.cleanupGameUI()
    print "GUI Cleanup done"
    cleanupManagers()
    print "Manager Cleanup done"

    HTTPLogger().writeContent(LoggerError.SUCCESS, "Shutdown Complete")
    HTTPLogger().endTable()
    HTTPLogger().closeLog()


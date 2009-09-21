# --------------------------------------------------
# Python Libs
import stackless
import sys
import struct
import random
import os
from ctypes import *

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Log.HTTPLogger import *
import Entity.Actor
import Entity.Manager
import Entity.World
import Entity.Unit
from Common.Common import Team
import Input.Manager

# --------------------------------------------------
# Managers
TimeManager     = Foundation.TimeManager()
Scheduler       = Foundation.Scheduler()
GraphicManager  = Foundation.GraphicManager()
InputManager    = Foundation.InputManager()
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
KEY_DELAY   = 0.15
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

CAMERA_SPEED = 2

SelectionBounds         = Foundation.Vector4(0, 0, 0, 0)
SelectionWorldBounds    = Foundation.Vector4(0, 0, 0, 0)
SelectedEntityList      = []

SphereGraphic = None
HumanSimTraining = True
BallMoving = True
ResetBall = False

# ------------------------------------------------
# InputWork
def doInput(_nDeltaTime):
    global MouseStateChange, KeyboardStateChange, Joystick0StateChange, Joystick1StateChange
    global TimerKeyDelay
    global SelectionBounds, SelectionWorldBounds, SelectedEntityList
    global RenderSphereTree, Pause, SphereTreeBucketSize, SphereTreeSpeedFactor, RenderSphereTreeLevel, SphereTreeTeamFlags
    global HumanSimTraining, BallMoving, SphereGraphic, ResetBall

    resetKey = False
    mouseState, keyboardState, joystickState = Input.Manager.consumeEvent(InputManager, None)

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
                    #Camera0.moveRelative(Foundation.Vector3(-nCamSpeed, 0, 0))
                    pass
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

                if (TimerKeyDelay.getTime() > KEY_DELAY):
                    if KeyIndex == Foundation.Keycode.Q:
                        HumanSimTraining = not HumanSimTraining                   
                        print "HumanSimTraining set to: ", HumanSimTraining
                        TimerKeyDelay.reset()
                    elif KeyIndex == Foundation.Keycode.E:
                        BallMoving = not BallMoving
                        print "Ball Moving set to: ", BallMoving
                        TimerKeyDelay.reset()
                    elif KeyIndex == Foundation.Keycode.A:
                        ResetBall = True
                        #SphereGraphic.setPosition(Foundation.Vector3(0, -100, 0))
                        TimerKeyDelay.reset()
 
                resetKey = True           
            
        if resetKey:
            KeyboardStateChange.assign(keyboardState)
            TimerKeyDelay.reset()

    # Mouse
    if mouseState:
        # Our left mouse button just went down, create an intersection point
        if mouseState.Button0 and not MouseStateChange.Button0:
            pass

        # Out left mouse button is still down, update our selection line
        elif mouseState.Button0 and MouseStateChange.Button0:
            pass

        # Our left mouse button just went up, do picking across our selection area
        elif not mouseState.Button0 and MouseStateChange.Button0:
            pass

#        MouseStateChange.assign(mouseState)

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

    # Init Graphics
    HTTPLogger().writeContent(LoggerError.NONE, "[GraphicManager] Initializing...")
    bResult = GraphicManager.initialize("BallFollow - Playground for NeuroVisual Control - Test1")
    if bResult:
        GraphicManager.showCursor(False)
        GraphicManager.addSceneManager("SceneManager0")
        Camera0 = GraphicManager.addCamera("SceneManager0", "Camera0", 0, 0.0, 0.0, 1.0, 1.0)
        Camera0.setPosition(Foundation.Vector3(0, 10, 0.01))
        Camera0.setLookAt(Foundation.Vector3(0, 0, 0))

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
    #MouseEventChannel.Channel_Join("MOUSE_EVENTS", onMouseEvent)
    #print " - [InputManager] Success."

    # Init EntityManager
    #print " - [EntityManager] Initializing..."
    #EntityManager = Entity.Manager.Manager("../../data/scarabEntityTypes.yaml", "../../data/scarabWeaponTypes.yaml")
    #Scheduler.AddTask(EntityManager.Task, 1, 0)
    #print " - [EntityManager] Success."

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

# ------------------------------------------------
# Main Tasklets
def schedulerTasklet():
    global EntityManager, SphereGraphic, Camera0, HumanSimTraining, BallMoving, ResetBall

    # Ball movement vars    
    uBallMoveTimer = Foundation.Timer()
    nBallTargetPosition = Foundation.Vector3(0,0,0)
    nBallOrgPosition = Foundation.Vector3(0,-100,0)    
    nBallPosition = Foundation.Vector3(0,-100,0)
    positionScale = 3
    movementTime = 3
    ballState = 0 # 2 states: going up, going down

    # Other stuff
    uMainTimer = Foundation.Timer()
    nDeltaTime = 0
    
    # Hold key down for 0.5 seconds
    uKeyTimer = Foundation.Timer()
    keyDelayTime = 0.5
    
    # Setup keyboard inject    
    Xtst = CDLL("libXtst.so.6")
    Xlib = CDLL("libX11.so.6")
    dpy = Xtst.XOpenDisplay(None)
        
    # Helpers taken from:
    #  http://wwwx.cs.unc.edu/~gb/wp/blog/2007/11/16/sending-key-events-to-pygame-programs/
    def SendInput( txt ):
        for c in txt:
            sym = Xlib.XStringToKeysym(c)
            code = Xlib.XKeysymToKeycode(dpy, sym)
            Xtst.XTestFakeKeyEvent(dpy, code, True, 0)
            Xtst.XTestFakeKeyEvent(dpy, code, False, 0)
        Xlib.XFlush(dpy)

    def SendKeyPress(key):
        sym = Xlib.XStringToKeysym(str(key))
        code = Xlib.XKeysymToKeycode(dpy, sym)
        Xtst.XTestFakeKeyEvent(dpy, code, True, 0)
        Xlib.XFlush(dpy)

    def SendKeyRelease(key):
        sym = Xlib.XStringToKeysym(str(key))
        code = Xlib.XKeysymToKeycode(dpy, sym)
        Xtst.XTestFakeKeyEvent(dpy, code, False, 0)
        Xlib.XFlush(dpy)

    # Tasklet loop
    while True:
        nDeltaTime = uMainTimer.getTime()
        uMainTimer.reset()
        nDeltaTime = Foundation.f_clamp(nDeltaTime, 0.0, 0.1)

        if BallMoving:
            nBallDiff = Foundation.Vector3(0,
                                           nDeltaTime * 30,
                                           0)
            nBallPosition = SphereGraphic.getPosition() + nBallDiff

            if HumanSimTraining:
                if nBallPosition[1] >= 0 and ballState == 0:
                    nBallPosition[1] = 5
                    SendKeyPress('a')
                    ballState = 1
                    uKeyTimer.reset()

                elif uKeyTimer.getTime() > 0.1 and ballState == 1:
                    SendKeyRelease('a')
                    nBallPosition = Foundation.Vector3(0, -100, 0)
                    ballState = 2
                    ResetBall = False

                elif (nBallPosition[1] < 0) and ballState == 2:
                    ballState = 0

            elif ResetBall:
                SendKeyRelease('a')
                nBallPosition = Foundation.Vector3(0, -100, 0)
                ResetBall = False
                

            SphereGraphic.setPosition(nBallPosition)

        # Update the camera, this is a faster "human" method for training the NN
        # simply move the camera to generally where the ball is by sending direct
        # keyboard controls (so the attached shared library sees them and correlates
        # it with the image).
        camPosition = Camera0.getPosition();
        vecDiff = camPosition - nBallPosition;

        #--

        # Update Input
        if not doInput(nDeltaTime):
            print "EntityManager shutting down units..."
            print "Done"
            break

        # Update internals
        Scheduler.Step(1.0)
        TimeManager.sleep(1)
        
        stackless.schedule()      

# ------------------------------------------------
# Entry Point
def main(argv):
    global EntityManager, SphereGraphic

    try:
        HTTPLogger("../ballfollow_log.html")
        HTTPLogger().newTable("Foundation Engine (Project: BallFollow)", "Description")
        HTTPLogger().writeContent(LoggerError.SUCCESS, "Initialized (%s/%s)" % (sys.path[0], sys.argv[0]))

        initManagers()

        # Init
        SphereGraphic = Foundation.EntityGraphic("SceneManager0", "Sphere_Graphics");
        SphereGraphic.setMesh("sphere.mesh");
        SphereGraphic.setMaterial("splatting0");
        SphereGraphic.setPosition(Foundation.Vector3(0, 0, 0))

        
        
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
    cleanupManagers()
    print "Manager Cleanup done"

    HTTPLogger().writeContent(LoggerError.SUCCESS, "Shutdown Complete")
    HTTPLogger().endTable()
    HTTPLogger().closeLog()


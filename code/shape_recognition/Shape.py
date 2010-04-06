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
GamePaused      = False

MouseEventChannel           = Foundation.Channel()
SelectionCallbackChannel    = Foundation.Channel()

Successful      = 0
Unsuccessful    = 0

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

CAMERA_SPEED = 10000

SelectionBounds         = Foundation.Vector4(0, 0, 0, 0)
SelectionWorldBounds    = Foundation.Vector4(0, 0, 0, 0)
SelectedEntityList      = []

SphereGraphic           = None
CubeGraphic             = None
TriangleGraphic         = None

HumanSimTraining        = True

#
SphereEntity = None
CubeEntity = None
TriangleEntity = None

# Track last keyboard states so we're not overfilling our input buffer
LastKeyboardState       = []
CurrentKeyboardState    = []
KeyIndexMapping         = ["4", "5", "6"]
    

nShapeChangeState       = 0
MovingShapes            = False
Xtst                    = None
Xlib                    = None
dpy                     = None

#
class Sphere:
    SphereGraphic = None
    Position = None
    Radius = 0
    Velocity = Foundation.Vector3(0, 0, 0)

    def __init__(self, radius):
        self.Radius = radius

        self.SphereGraphic = Foundation.EntityGraphic("SceneManager0", "Sphere_Graphic");
        self.SphereGraphic.setMesh("sphere.mesh");
        self.SphereGraphic.setMaterial("splatting0");
        self.SphereGraphic.setScale(Foundation.Vector3(radius, radius, radius))
        
        self.setPosition(Foundation.Vector3(-10000, 0, 0))

        print "Sphere Created"
        
    def setPosition(self, position):
        self.Position = position
        self.SphereGraphic.setPosition(position)
        
    def getPosition(self):
        return self.Position

    def setVisible(self, visible):
        if visible:
            if MovingShapes:
                xPos = random.randint(-1000, 1000)
                yPos = random.randint(-1000, 1000)
                self.setPosition(Foundation.Vector3(xPos, 0, yPos))
            else:
                self.setPosition(Foundation.Vector3(0, 0, 0))
        else:
            self.setPosition(Foundation.Vector3(-10000, 0, 0))
    

class Cube:
    Graphic = None
    Position = None
    Length = 0
    Width = 0

    def __init__(self, size):
        self.Length = size
        self.Width = size
        self.Graphic = None
        
        self.Graphic = Foundation.EntityGraphic("SceneManager0", "Cube_Graphic" + str(id));
        self.Graphic.setMesh("cube.mesh");
        self.Graphic.setMaterial("splatting0");
        self.Graphic.setScale(Foundation.Vector3(size, 0, size))
        
        self.setPosition(Foundation.Vector3(-10000, 0, 0))
        
        print "Cube Created"
        
    def setPosition(self, position):
        self.Position = position
        if self.Graphic:
            self.Graphic.setPosition(position)
        
    def getPosition(self):
        return self.Position

    def setVisible(self, visible):
        if visible:
            if MovingShapes:
                xPos = random.randint(-1000, 1000)
                yPos = random.randint(-1000, 1000)
                self.setPosition(Foundation.Vector3(xPos, 0, yPos))
            else:
                self.setPosition(Foundation.Vector3(0, 0, 0))
        else:
            self.setPosition(Foundation.Vector3(-10000, 0, 0))
    

class Triangle:
    Graphic = None
    Position = None
    Length = 0
    Width = 0

    def __init__(self, size):
        self.Length = size
        self.Width = size
        self.Graphic = None
        
        self.Graphic = Foundation.EntityGraphic("SceneManager0", "Triangle_Graphic" + str(id));
        self.Graphic.setMesh("triangle.mesh");
        self.Graphic.setMaterial("splatting0");
        self.Graphic.setScale(Foundation.Vector3(size, 0, size))
        
        self.setPosition(Foundation.Vector3(-10000, 0, 0))
        
        print "Triangle Created"
        
    def setPosition(self, position):
        self.Position = position
        if self.Graphic:
            self.Graphic.setPosition(position)
        
    def getPosition(self):
        return self.Position
       
    def setVisible(self, visible):
        if visible:
            if MovingShapes:
                xPos = random.randint(-1000, 1000)
                yPos = random.randint(-1000, 1000)
                self.setPosition(Foundation.Vector3(xPos, 0, yPos))
            else:
                self.setPosition(Foundation.Vector3(0, 0, 0))
        else:
            self.setPosition(Foundation.Vector3(-10000, 0, 0))
    

# -----------------------------------------------
# User pressed a key, check if it matches the shape on the screen
def doShapeCheck(shape):
    
    if shape == "triangle" and nShapeChangeState == 0:
        pass
    elif shape == "cube" and nShapeChangeState == 1:
        pass
    elif shape == "sphere" and nShapeChangeState == 2:
        pass
    else:
        print "Incorrect shape selected: " + shape

# ------------------------------------------------
# InputWork
def doInput(_nDeltaTime):
    global MouseStateChange, KeyboardStateChange, Joystick0StateChange, Joystick1StateChange
    global TimerKeyDelay
    global SelectionBounds, SelectionWorldBounds, SelectedEntityList
    global RenderSphereTree, Pause, SphereTreeBucketSize, SphereTreeSpeedFactor, RenderSphereTreeLevel, SphereTreeTeamFlags
    global HumanSimTraining, nCamSpeed, Boundary
    global GamePaused
    global CurrentKeyboardState

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
                   

                # Keys that specify which shape is currently on the screen
                if KeyIndex == Foundation.Keycode._4:
                    doShapeCheck("triangle")
                elif KeyIndex == Foundation.Keycode._5:
                    doShapeCheck("cube")
                elif KeyIndex == Foundation.Keycode._6:
                    doShapeCheck("sphere")

                # --
                if KeyIndex == Foundation.Keycode.P:
                    GamePaused = not GamePaused
                    print "Game Pause: " + str(GamePaused)
                
                if (TimerKeyDelay.getTime() > KEY_DELAY):
                    if KeyIndex == Foundation.Keycode.Q:
                        HumanSimTraining = not HumanSimTraining                   
                        print "HumanSimTraining set to: ", HumanSimTraining
                        TimerKeyDelay.reset()

                        if not HumanSimTraining:

                            CurrentKeyboardState = [False, False, False]
                            UpdateKeyboardStates()
                    
                resetKey = True           
            
        if resetKey:
            KeyboardStateChange.assign(keyboardState)
            TimerKeyDelay.reset()

    return True

# ------------------------------------------------
# Initialize Managers
def initManagers():
    global TimeManager, Schdeuler, AudioManager, InputManager, GraphicManager, EntityManager
    global GUIManager, MouseEventChannel, Camera0

    # Init Graphics
    HTTPLogger().writeContent(LoggerError.NONE, "[GraphicManager] Initializing...")
    bResult = GraphicManager.initialize("ShapeRecognition - Playground for nvc - Test4")
    if bResult:
        GraphicManager.showCursor(False)
        GraphicManager.addSceneManager("SceneManager0")
        Camera0 = GraphicManager.addCamera("SceneManager0", "Camera0", 0, 0.0, 0.0, 1.0, 1.0)
        Camera0.setPosition(Foundation.Vector3(0, 5000, 0.01))
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



# Testing new and improved keyboard handling to keep the input buffer sane
def UpdateKeyboardStates():
    global CurrentKeyboardState, LastKeyboardState

    SendKeyPress("-")
    for x in xrange(0, len(KeyIndexMapping)):
        keyState = CurrentKeyboardState[x]
        if (keyState != LastKeyboardState[x]):
        
            key = KeyIndexMapping[x]
            #print "KeyChangeState: " + str(key) + ", " + str(keyState)

            if (keyState == True):
                SendKeyPress(key)
            else:
                SendKeyRelease(key)

    SendKeyComplete()

    LastKeyboardState = CurrentKeyboardState
    CurrentKeyboardState = []
        
# Helpers taken from:
#  http://wwwx.cs.unc.edu/~gb/wp/blog/2007/11/16/sending-key-events-to-pygame-programs/
def SendKeyPress(key):
    sym = Xlib.XStringToKeysym(str(key))
    code = Xlib.XKeysymToKeycode(dpy, sym)
    Xtst.XTestFakeKeyEvent(dpy, code, True, 0)
    #Xlib.XFlush(dpy)

def SendKeyRelease(key):
    sym = Xlib.XStringToKeysym(str(key))
    code = Xlib.XKeysymToKeycode(dpy, sym)
    Xtst.XTestFakeKeyEvent(dpy, code, False, 0)
    #Xlib.XFlush(dpy)

def SendKeyComplete():
    Xlib.XFlush(dpy)

# ------------------------------------------------
# Main Tasklets
def schedulerTasklet():
    global EntityManager, Camera0
    global SphereEntity, CubeEntity, TriangleEntity
    global LastKeyboardState, CurrentKeyboardState
    global nShapeChangeState 

    # Shape change vars
    nShapeChangeLastState = 0
    nShapeChangeTimer = Foundation.Timer()
    nBallPosition = Foundation.Vector3(0,0,0)
    positionScale = 1
    movementTime = 1
    ballState = 0 # 2 states: going up, going down

    # Other stuff
    uMainTimer = Foundation.Timer()
    nDeltaTime = 0
    
    # Hold key down for 0.5 seconds
    uKeyTimer = Foundation.Timer()
    keyDelayTime = 0.015
    
    LastKeyboardState = [False, False, False]
        

    # Tasklet loop
    while True:
        nDeltaTime = uMainTimer.getTime()
        uMainTimer.reset()
        nDeltaTime = Foundation.f_clamp(nDeltaTime, 0.0, 0.1)

        if (not GamePaused):
            # Update the shape on the screen
            if (nShapeChangeTimer.getTime() > 2):
                nShapeChangeState += 1
                nShapeChangeTimer.reset()

            if (nShapeChangeLastState != nShapeChangeState):
                if (nShapeChangeState > 2):
                    nShapeChangeState = 0
                nShapeChangeLastState = nShapeChangeState

                print "New Shape State: " + str(nShapeChangeState)
    
                if (nShapeChangeState == 0):
                    SphereEntity.setVisible(False)
                    CubeEntity.setVisible(False)
                    TriangleEntity.setVisible(True)
                elif (nShapeChangeState == 1):
                    SphereEntity.setVisible(True)
                    CubeEntity.setVisible(False)
                    TriangleEntity.setVisible(False)
                elif (nShapeChangeState == 2):
                    TriangleEntity.setVisible(False)
                    SphereEntity.setVisible(False)
                    CubeEntity.setVisible(True)
        else:
            print "Game Paused" 

        # Human Simulation Training player
        if HumanSimTraining:
            CurrentKeyboardState = [False, False, False]

            CurrentKeyboardState[nShapeChangeState] = True

            UpdateKeyboardStates()

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
    global EntityManager, SphereEntity, CubeEntity, TriangleEntity
    global Xtst, Xlib, dpy
    
    # Setup keyboard inject    
    Xtst = CDLL("libXtst.so.6")
    Xlib = CDLL("libX11.so.6")
    dpy = Xtst.XOpenDisplay(None)

    random.seed()

    try:
        HTTPLogger("../pong_log.html")
        HTTPLogger().newTable("Foundation Engine (Project: Pong)", "Description")
        HTTPLogger().writeContent(LoggerError.SUCCESS, "Initialized (%s/%s)" % (sys.path[0], sys.argv[0]))

        initManagers()

        # Init
        CubeEntity = Cube(10)
        SphereEntity = Sphere(400)
        TriangleEntity = Triangle(400)
        
        # Start
        stackless.tasklet(schedulerTasklet)()

        try:
            stackless.run()
        except TaskletExit:
            raise

    except KeyboardInterrupt:
        print "\n"

    print "Shutting down X display..."
    Xtst.XCloseDisplay(dpy)

    shutdown()

# ------------------------------------------------
# Shutdown
def shutdown():
    global CurrentKeyboardState

    print "Shutting down..."

    # Cleanup
    cleanupManagers()
    print "Manager Cleanup done"

    HTTPLogger().writeContent(LoggerError.SUCCESS, "Shutdown Complete")
    HTTPLogger().endTable()
    HTTPLogger().closeLog()

    CurrentKeyboardState = [False, False, False]
    UpdateKeyboardStates()

    print "Shutdown game complete"


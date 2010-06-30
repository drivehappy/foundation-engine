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

HumanSimTraining        = True

#
CircleEntity = None
SquareEntity = None
TriangleEntity = None

# Track last keyboard states so we're not overfilling our input buffer
LastKeyboardState       = []
CurrentKeyboardState    = []
KeyIndexMapping         = ["4", "5", "6"]
    

nShapeChangeState       = 0
MovingShapes            = True
RotatingShapes          = False
ScalingShapes           = False
Position                = Foundation.Vector3(0, 0, 0)

# Control function that the NVC system can hook into and retrieve these values
def nvcControlLoopback(controlIndex, controlValue):
    global nShapeChangeState, Position

    # For the Dot Position we're just going to show the triangle
    if controlIndex == 0:
        if controlValue == 1:
            CubeEntity.setVisible(True)
        else:
            CubeEntity.setVisible(False)

    # Control index 1 is the X position
    elif controlIndex == 1:
        Position[0] = controlValue * 2200
    # Control index 2 is the Y position
    elif controlIndex == 2:
        Position[2] = controlValue * 1600

    CubeEntity.setPosition(Position)

    #print "Position: ", Position[0], Position[1]

# Base class for all of our shapes
class Shape:
    Graphic = None
    Position = None
    Scale = 0
    Velocity = Foundation.Vector3(0, 0, 0)

    def __init__(self, scale, mesh, name):
        self.Scale = scale
       
        self.Graphic = Foundation.EntityGraphic("SceneManager0", "Graphic_" + name);
        self.Graphic.setMesh(mesh);
        self.Graphic.setMaterial("Scarab/EntityTestMaterial_BlackTeam");
        self.Graphic.setScale(Foundation.Vector3(scale, 0, scale))

        #self.setRotation(0)
        self.setVisible(False)

    def setPosition(self, position):
        self.Position = position
        self.Graphic.setPosition(position)

    def setRotation(self, rotation):
        # Rotate about the Y-axis
        if self.Graphic:
            self.Graphic.setRotation(Foundation.Quaternion(0, rotation, 0, 1))
        
    def getPosition(self):
        return self.Position

    def setVisible(self, visible):
        if visible:
            if MovingShapes:
                xPos = random.randint(-2000, 2000)
                yPos = random.randint(-1500, 1500)
                self.setPosition(Foundation.Vector3(xPos, 0, yPos))
            else:
                self.setPosition(Foundation.Vector3(0, 0, 0))
            
            if RotatingShapes:
                rotate = random.randrange(-1000, 1000) / 500.0
                self.setRotation(rotate)
                self.setPosition(Foundation.Vector3(0, 0, 0))
            
            if ScalingShapes:
                scale = random.randrange(100, 400) / 200.0;
                scale *= self.Radius
                self.SphereGraphic.setScale(Foundation.Vector3(scale, scale, scale))
        
        else:
            self.setPosition(Foundation.Vector3(-10000, 0, 0))
    
class Triangle(Shape):
    def __init__(self, scale):
        Shape.__init__(self, scale, "triangle.mesh", "Tri")

class Square(Shape):
    def __init__(self, scale):
        Shape.__init__(self, scale, "cube.mesh", "Square")

class Circle(Shape):
    def __init__(self, scale):
        Shape.__init__(self, scale, "sphere.mesh", "Circle")

# ------------------------------------------------
# InputWork
def doInput(_nDeltaTime):
    global MouseStateChange, KeyboardStateChange, Joystick0StateChange, Joystick1StateChange
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
                   
                if KeyIndex == Foundation.Keycode.T:
                    GraphicManager.createScreenshot("screenshot.png")

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


# ------------------------------------------------
# Move the shape to a random position
def MoveShapeRandom():
    x = random.randint(-10000, 10000) / 10000.0
    y = random.randint(-10000, 10000) / 10000.0

    Foundation.nvcControl(nvcControlLoopback, 1, x)
    Foundation.nvcControl(nvcControlLoopback, 2, y)

# ------------------------------------------------
# Main Tasklets
def schedulerTasklet():
    global EntityManager, Camera0
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
    uShapeTimer = Foundation.Timer()
    nDeltaTime = 0
    
    # Hold key down for 0.5 seconds
    uKeyTimer = Foundation.Timer()
    keyDelayTime = 0.015
    
    LastKeyboardState = [False, False, False]

    Velocity = Foundation.Vector3(random.randint(-1000, 1000), 0, random.randint(-1000, 1000))
    #Velocity = Foundation.Vector3(random.randint(-1000, 1000), 0, 0)
    #CubeEntity.setVisible(True)
        

    # Tasklet loop
    while True:
        nDeltaTime = uMainTimer.getTime()
        uMainTimer.reset()
        nDeltaTime = Foundation.f_clamp(nDeltaTime, 0.0, 0.1)

        #Foundation.nvcControl(nvcControlLoopback, 100, 555)

        if (not GamePaused):
            pass
        else:
            print "Game Paused" 

        # Human Simulation Training player
        if HumanSimTraining:
            CurrentKeyboardState = [False, False, False]

            CurrentKeyboardState[nShapeChangeState] = True

            if uShapeTimer.getTime() > 5:
                uShapeTimer.reset()
                MoveShapeRandom()

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

    random.seed()

    try:
        HTTPLogger("../shape_log.html")
        HTTPLogger().newTable("Foundation Engine (Project: Shape Recognition)", "Description")
        HTTPLogger().writeContent(LoggerError.SUCCESS, "Initialized (%s/%s)" % (sys.path[0], sys.argv[0]))

        initManagers()

        # Init
        CubeEntity = Square(2)
        CubeEntity.setVisible(True)
        #SphereEntity = Circle(400)
        #TriangleEntity = Triangle(400)
        #TriangleEntity.setVisible(True)
        
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


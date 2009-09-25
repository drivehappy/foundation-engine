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
KEY_DELAY   = 0.015
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

BallGraphic = None
Paddle0Graphic = None
Paddle1Graphic = None

HumanSimTraining = True
BallMoving = True
ResetBall = False
MoveLeft = False
MoveRight = False

#
PongBall = None
Boundary = None

#
class Ball:
    BallGraphic = None
    Position = None
    Radius = 0
    Velocity = Foundation.Vector3(0, 0, 0)

    def __init__(self, radius):
        self.Radius = radius

        self.BallGraphic = Foundation.EntityGraphic("SceneManager0", "Ball_Graphic");
        self.BallGraphic.setMesh("sphere.mesh");
        self.BallGraphic.setMaterial("splatting0");
        self.BallGraphic.setScale(Foundation.Vector3(radius, radius, radius))
        
        self.setPosition(Foundation.Vector3(0, 0, 0))
        
    def setPosition(self, position):
        self.Position = position
        self.BallGraphic.setPosition(position)
        
    def getPosition(self):
        return self.Position

    def setVelocity(self, velocity):
        self.Velocity = velocity

    def reset(self):
        self.setPosition(Foundation.Vector3(0, 0, 0))
        self.resetVelocity()

    def resetVelocity(self):
        xVel = random.randint(500, 1000) * (1 if random.randint(0, 1) == 1 else -1)
        zVel = random.randint(500, 1000) * (1 if random.randint(0, 1) == 1 else -1)
        self.setVelocity(Foundation.Vector3(xVel, 0, zVel))

    def update(self, nDeltaTime):
        self.Position += self.Velocity * nDeltaTime
        self.BallGraphic.setPosition(self.Position)

        self.__checkCollision()

    def __checkCollision(self):
        global Boundary

        # Check if out of bounds and paddle contact
        if (self.Position[0] > 1900) and (abs(Boundary[1].getPosition()[2] - self.Position[2]) < 350):
            self.setPosition(Foundation.Vector3(1899, 0, self.Position[2]))
            self.Velocity[0] = -self.Velocity[0]
        elif (self.Position[0] < -1900) and (abs(Boundary[0].getPosition()[2] - self.Position[2]) < 350):
            self.setPosition(Foundation.Vector3(-1899, 0, self.Position[2]))
            self.Velocity[0] = -self.Velocity[0]

        # Simply check out of bounds
        if (self.Position[0] > 2000) or (self.Position[0] < -2000):
            print "Reset: Ball pos =", self.Position[0], self.Position[2], " + Boundary pos =", Boundary[0].getPosition()[0], Boundary[0].getPosition()[2]
            self.reset()      
 
        # Top and Bottom are always present 
        if (self.Position[2] > 1900):
            self.setPosition(Foundation.Vector3(self.Position[0], 0, 1899))
            self.Velocity[2] = -self.Velocity[2]
        elif (self.Position[2] < -1900):
            self.setPosition(Foundation.Vector3(self.Position[0], 0, -1899))
            self.Velocity[2] = -self.Velocity[2]

    
class Wall:
    Graphic = None
    Position = None
    Length = 0
    Width = 0

    def __init__(self, id, length, width):
        self.Length = length
        self.Width = width

        self.Graphic = Foundation.EntityGraphic("SceneManager0", "Wall_Graphic" + str(id));
        self.Graphic.setMesh("cube.mesh");
        self.Graphic.setMaterial("splatting0");
        self.Graphic.setScale(Foundation.Vector3(length, 0, width))
        
        self.setPosition(Foundation.Vector3(0, 0, 0))
        
    def setPosition(self, position):
        self.Position = position
        self.Graphic.setPosition(position)
        
    def getPosition(self):
        return self.Position

    def move(self, offset):
        self.Position += offset
        self.Graphic.setPosition(self.Position)
        

    
# ------------------------------------------------
# InputWork
def doInput(_nDeltaTime):
    global MouseStateChange, KeyboardStateChange, Joystick0StateChange, Joystick1StateChange
    global TimerKeyDelay
    global SelectionBounds, SelectionWorldBounds, SelectedEntityList
    global RenderSphereTree, Pause, SphereTreeBucketSize, SphereTreeSpeedFactor, RenderSphereTreeLevel, SphereTreeTeamFlags
    global HumanSimTraining, nCamSpeed, Boundary

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
                    MoveLeft = True
                elif KeyIndex == Foundation.Keycode.W:
                    Boundary[0].move(Foundation.Vector3(0, 0, -30))

                if KeyIndex == Foundation.Keycode.S:
                    Boundary[0].move(Foundation.Vector3(0, 0, 30))
                elif KeyIndex == Foundation.Keycode.D:
                    MoveRight = True
                    #Camera0.moveRelative(Foundation.Vector3(nCamSpeed, 0, 0))

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
    global EntityManager, Camera0, HumanSimTraining, PongBall

    # Ball movement vars    
    uBallMoveTimer = Foundation.Timer()
    nBallTargetPosition = Foundation.Vector3(0,0,0)
    nBallOrgPosition = Foundation.Vector3(0,0,0)    
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

        PongBall.update(nDeltaTime)

        # AI Player
        if (PongBall.getPosition()[2] > Boundary[1].getPosition()[2]):
            Boundary[1].move(Foundation.Vector3(0, 0, 1000 * nDeltaTime))
        elif (PongBall.getPosition()[2] < Boundary[1].getPosition()[2]):
            Boundary[1].move(Foundation.Vector3(0, 0, -1000 * nDeltaTime))

        # Human Simulation Training player
        if HumanSimTraining:
            if (PongBall.getPosition()[2] > Boundary[0].getPosition()[2] + 100):
                SendKeyPress("s")
            else:
                SendKeyRelease("s")

            if (PongBall.getPosition()[2] < Boundary[0].getPosition()[2] - 100):
                SendKeyPress("w")
            else:
                SendKeyRelease("w")

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
    global EntityManager, PongBall, Boundary

    random.seed()

    try:
        HTTPLogger("../ballfollow_log.html")
        HTTPLogger().newTable("Foundation Engine (Project: BallFollow)", "Description")
        HTTPLogger().writeContent(LoggerError.SUCCESS, "Initialized (%s/%s)" % (sys.path[0], sys.argv[0]))

        initManagers()

        # Init
        Boundary = [Wall(0, 1, 5), Wall(1, 1, 5), Wall(2, 40, 1), Wall(3, 40, 1)]
        Boundary[0].setPosition(Foundation.Vector3(-2000, 0, 0))
        Boundary[1].setPosition(Foundation.Vector3(2000, 0, 0))
        Boundary[2].setPosition(Foundation.Vector3(0, 0, 2000))
        Boundary[3].setPosition(Foundation.Vector3(0, 0, -2000))
        
        PongBall = Ball(100)
        xVel = random.randint(500, 1000) * (1 if random.randint(0, 1) == 1 else -1)
        zVel = random.randint(500, 1000) * (1 if random.randint(0, 1) == 1 else -1)
        PongBall.setVelocity(Foundation.Vector3(xVel, 0, zVel))
        
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


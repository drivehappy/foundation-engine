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

# Track last keyboard states so we're not overfilling our input buffer
LastKeyboardState       = []
CurrentKeyboardState    = []
KeyIndexMapping         = ["4", "5", "6", "7", "8", "9", "0", "s", "w"]

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
        global Successful, Unsuccessful

        self.setPosition(Foundation.Vector3(2500, 0, 0))
        self.resetVelocity()
        if ((Successful > 0 or Unsuccessful > 0) and not HumanSimTraining):
            print "Success Rate: " + str(((Successful * 1.) / (Successful + Unsuccessful)) * 100) + "% (S:" + str(Successful) + ", U:" + str(Unsuccessful) + ")"

    def resetVelocity(self):
        xVel = random.randint(1000, 2000) * -1
        zVel = random.randint(0, 2000) * (1 if random.randint(0, 1) == 1 else -1)
        self.setVelocity(Foundation.Vector3(xVel, 0, zVel))

    def update(self, nDeltaTime):
        self.Position += self.Velocity * nDeltaTime
        self.BallGraphic.setPosition(self.Position)

        self.__checkCollision()

    def __checkCollision(self):
        global Boundary
        global Successful, Unsuccessful

        # Check if out of bounds and paddle contact
        #if (self.Position[0] > 1900) and (abs(Boundary[1].getPosition()[2] - self.Position[2]) < 350):
        #    self.setPosition(Foundation.Vector3(1899, 0, self.Position[2]))
        #    self.Velocity[0] = -self.Velocity[0]
        #elif (self.Position[0] < -1900) and (abs(Boundary[0].getPosition()[2] - self.Position[2]) < 350):
        if (self.Position[0] < -1900 and self.Position[0] > -2200) and (abs(Boundary[0].getPosition()[2] - self.Position[2]) < 450):
            self.setPosition(Foundation.Vector3(-1899, 0, self.Position[2]))
            self.Velocity[0] = -self.Velocity[0]
            if not HumanSimTraining:
                Successful += 1
            self.reset()

        # Simply check out of bounds
        #if (self.Position[0] > 2000) or (self.Position[0] < -2000):
        if (self.Position[0] < -3000):
            print "Reset: Ball pos =", self.Position[0], self.Position[2], " + Boundary pos =", Boundary[0].getPosition()[0], Boundary[0].getPosition()[2]
            if not HumanSimTraining:
                Unsuccessful += 1
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
        self.Graphic = None
        
        if id == 0:
            self.Graphic = Foundation.EntityGraphic("SceneManager0", "Wall_Graphic" + str(id));
            self.Graphic.setMesh("cube.mesh");
            self.Graphic.setMaterial("splatting0");
            self.Graphic.setScale(Foundation.Vector3(length, 0, width))
        
        self.setPosition(Foundation.Vector3(0, 0, 0))
        
    def setBounds(self):
        if (self.Position[2] > 1500):
            self.Position[2] = 1500
        elif (self.Position[2] < -1500):
            self.Position[2] = -1500

    def setPosition(self, position):
        self.Position = position
        self.setBounds()
        if self.Graphic:
            self.Graphic.setPosition(position)
        
    def getPosition(self):
        return self.Position

    def move(self, offset):
        self.Position += offset
        self.setBounds()
        if self.Graphic:
            self.Graphic.setPosition(self.Position)
        

    
# ------------------------------------------------
# InputWork
def doInput(_nDeltaTime):
    global MouseStateChange, KeyboardStateChange, Joystick0StateChange, Joystick1StateChange
    global TimerKeyDelay
    global SelectionBounds, SelectionWorldBounds, SelectedEntityList
    global RenderSphereTree, Pause, SphereTreeBucketSize, SphereTreeSpeedFactor, RenderSphereTreeLevel, SphereTreeTeamFlags
    global HumanSimTraining, nCamSpeed, Boundary
    global GamePaused

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
                   
                ''' 
                # Old method of moving paddle to relative positions
                nCamSpeed = _nDeltaTime * CAMERA_SPEED
                if KeyIndex == Foundation.Keycode.W:
                    Boundary[0].move(Foundation.Vector3(0, 0, -16000 * _nDeltaTime))
                elif KeyIndex == Foundation.Keycode.S:
                    Boundary[0].move(Foundation.Vector3(0, 0, 16000 * _nDeltaTime))
                '''

                # New method of moving paddle to absolute positions, not the old relative
                if KeyIndex == Foundation.Keycode._0:
                    Boundary[0].setPosition(Foundation.Vector3(-2000, 0, -1600))
                    print "Received: 0"
                elif KeyIndex == Foundation.Keycode._9:
                    Boundary[0].setPosition(Foundation.Vector3(-2000, 0, -1000))
                    print "Received: 9"
                elif KeyIndex == Foundation.Keycode._8:
                    Boundary[0].setPosition(Foundation.Vector3(-2000, 0, -400))
                    print "Received: 8"
                elif KeyIndex == Foundation.Keycode._7:
                    Boundary[0].setPosition(Foundation.Vector3(-2000, 0, 200))
                    print "Received: 7"
                elif KeyIndex == Foundation.Keycode._6:
                    Boundary[0].setPosition(Foundation.Vector3(-2000, 0, 800))
                    print "Received: 6"
                elif KeyIndex == Foundation.Keycode._5:
                    Boundary[0].setPosition(Foundation.Vector3(-2000, 0, 1400))
                    print "Received: 5"
                elif KeyIndex == Foundation.Keycode._4:
                    Boundary[0].setPosition(Foundation.Vector3(-2000, 0, 2000))
                    print "Received: 4"

                if KeyIndex == Foundation.Keycode.P:
                    GamePaused = not GamePaused
                    print "Game Pause: " + str(GamePaused)
                elif KeyIndex == Foundation.Keycode.O:
                    PongBall.update(0.02)
                
                if (TimerKeyDelay.getTime() > KEY_DELAY):
                    if not GamePaused:
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
    bResult = GraphicManager.initialize("Pong - Playground for NeuroVisual Control - Test3")
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


# ------------------------------------------------
# Main Tasklets
def schedulerTasklet():
    global EntityManager, Camera0, HumanSimTraining, PongBall
    global LastKeyboardState, CurrentKeyboardState

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
    def SendInput(txt):
        for c in txt:
            sym = Xlib.XStringToKeysym(c)
            code = Xlib.XKeysymToKeycode(dpy, sym)
            Xtst.XTestFakeKeyEvent(dpy, code, True, 0)
            Xtst.XTestFakeKeyEvent(dpy, code, False, 0)
        Xlib.XFlush(dpy)

    def SendKeyPress(key):
        print "KeyPress: " + str(key)

        sym = Xlib.XStringToKeysym(str(key))
        code = Xlib.XKeysymToKeycode(dpy, sym)
        Xtst.XTestFakeKeyEvent(dpy, code, True, 0)
        Xlib.XFlush(dpy)

    def SendKeyRelease(key):
        print "KeyRelease: " + str(key)

        sym = Xlib.XStringToKeysym(str(key))
        code = Xlib.XKeysymToKeycode(dpy, sym)
        Xtst.XTestFakeKeyEvent(dpy, code, False, 0)
        Xlib.XFlush(dpy)

    # Testing new and improved keyboard handling to keep the input buffer sane
    def UpdateKeyboardStates():
        global CurrentKeyboardState, LastKeyboardState
        for x in xrange(0, len(KeyIndexMapping)):
            keyState = CurrentKeyboardState[x]
            if (keyState != LastKeyboardState[x]):
                key = KeyIndexMapping[x]

                if (keyState == True):
                    SendKeyPress(key)
                else:
                    SendKeyRelease(key)

        LastKeyboardState = CurrentKeyboardState
        CurrentKeyboardState = []
        

    # Tasklet loop
    while True:
        nDeltaTime = uMainTimer.getTime()
        uMainTimer.reset()
        nDeltaTime = Foundation.f_clamp(nDeltaTime, 0.0, 0.1)

        if (not GamePaused):
            PongBall.update(nDeltaTime)

        # Human Simulation Training player
        if HumanSimTraining:
            CurrentKeyboardState = [False, False, False, False, False, False, False, False, False]

            if (PongBall.getPosition()[0] < 3500):
                '''
                # Old method of relative paddle movement
                if (PongBall.getPosition()[2] > Boundary[0].getPosition()[2] + 250):
                    CurrentKeyboardState[7] = True
                elif (PongBall.getPosition()[2] < Boundary[0].getPosition()[2] - 250):
                    CurrentKeyboardState[8] = True
                '''

                # New method of absolute paddle movement
                yPos = PongBall.getPosition()[2];


                if (yPos >= 1500):
                    CurrentKeyboardState[0] = True
                elif (yPos < 1500 and yPos >= 1000):
                    CurrentKeyboardState[1] = True
                elif (yPos < 1000 and yPos >= 500):
                    CurrentKeyboardState[2] = True
                elif (yPos < 500 and yPos >= 0):
                    CurrentKeyboardState[3] = True
                elif (yPos < 0 and yPos >= -500):
                    CurrentKeyboardState[4] = True
                elif (yPos < -500 and yPos >= -1500):
                    CurrentKeyboardState[5] = True
                elif (yPos < -1500):
                    CurrentKeyboardState[6] = True

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
    global EntityManager, PongBall, Boundary
    global LastKeyboardState


    LastKeyboardState = [False, False, False, False, False, False, False, False, False]

    random.seed()

    try:
        HTTPLogger("../pong_log.html")
        HTTPLogger().newTable("Foundation Engine (Project: Pong)", "Description")
        HTTPLogger().writeContent(LoggerError.SUCCESS, "Initialized (%s/%s)" % (sys.path[0], sys.argv[0]))

        initManagers()

        # Init
        Boundary = [Wall(0, 1.6, 5)]
        Boundary[0].setPosition(Foundation.Vector3(-2000, 0, 0))
        
        PongBall = Ball(200)
        xVel = random.randint(1000, 2000) * -1
        zVel = random.randint(1000, 2000) * (1 if random.randint(0, 1) == 1 else -1)
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

    print "Pong done"


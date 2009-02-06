# --------------------------------------------------
# input.py
# 
# Handles Mouse, Keyboard, Joystick input.
# --------------------------------------------------
import FoundationPython as Foundation

# --------------------------------------------------
# Hold the state of the mouse
# --------------------------------------------------
class MouseState:
    def __init__(self):
        self.setDefault()
        
    def setDefault(self):
        self.Button0 = False
        self.Button1 = False
        self.AxisX = 0
        self.AxisY = 0
        self.Move = False
        self.DeltaAxisX = 0
        self.DeltaAxisY = 0

    def assign(self, other):
        self.Button0 = other.Button0
        self.Button1 = other.Button1
        self.AxisX = other.AxisX
        self.AxisY = other.AxisY
        self.Move = other.Move
        self.DeltaAxisX = other.DeltaAxisX
        self.DeltaAxisY = other.DeltaAxisY

# --------------------------------------------------
# Hold the state of the keyboard
# --------------------------------------------------
class KeyboardState:
    def __init__(self):
        self.setDefault()
        
    def setDefault(self):
        self.Keys    = {}

    def assign(self, other):
        for KeyVal in other.Keys:
            self.Keys[KeyVal] = other.Keys[KeyVal]

    def isKeyDown(self):
        for KeyVal in self.Keys:
            if self.Keys[KeyVal]:
                return True
        return False

# --------------------------------------------------
# Hold the state of the joystick
# --------------------------------------------------
class JoystickState:
    def __init__(self):
        self.setDefault()
        
    def setDefault(self):
        self.Axis   = [0] * 16
        self.Button = [False] * 32
        self.Index  = 0

    def assign(self, other):
        self.Axis = other.Axis
        self.Button = other.Button
        self.Index = other.Index
  
# --------------------------------------------------
# Previous Input States
# --------------------------------------------------
mouseStatePrev = MouseState()
keyboardStatePrev = KeyboardState()
joystickStatePrev = JoystickState()

# --------------------------------------------------
# Give our Joystick axis' some jitter allowance
# --------------------------------------------------
JOYSTICK_AXIS_ALLOWANCE = 100.0

# --------------------------------------------------
# Consume Foundation.InputManager events
#
# Requires at least Foundation.InputManager to be passed
#  in. GUIManager can be None if events aren't to be
#  sent to the GUI.
# --------------------------------------------------
def consumeEvent(InputManager, GUIManager):
    global mouseStatePrev, keyboardStatePrev, joystickStatePrev

    mouseState = None
    keyboardState = None
    joystickState = None

    # Work on Mouse Events
    MouseEvent = InputManager.getMouseEvent()
    if MouseEvent:
        # Set the state to the last state, then update according to events
        mouseState = MouseState()
        if mouseStatePrev:
            mouseState.assign(mouseStatePrev)
        else:
            mouseState.setDefault()

        bGUIConsumeEvent = False
        if GUIManager:
            bGUIConsumeEvent = GUIManager.injectMouseEvent(MouseEvent)    
        
        #if (not bGUIConsumeEvent):
        if MouseEvent.MouseEventID == Foundation.Events.MOUSE_MOVE:
            mouseState.AxisX = MouseEvent.MouseAxisX
            mouseState.AxisY = MouseEvent.MouseAxisY
            mouseState.DeltaAxisX = mouseState.AxisX - mouseStatePrev.AxisX
            mouseState.DeltaAxisY = mouseState.AxisY - mouseStatePrev.AxisY
            mouseState.Move = True

        if MouseEvent.MouseEventID == Foundation.Events.MOUSE_BUTTON_PRESSED:
            if MouseEvent.MouseButtonID == 0:
                mouseState.Button0 = True
            elif MouseEvent.MouseButtonID == 1:
                mouseState.Button1 = True
        elif MouseEvent.MouseEventID == Foundation.Events.MOUSE_BUTTON_RELEASED:
            if MouseEvent.MouseButtonID == 0:
                mouseState.Button0 = False
            elif MouseEvent.MouseButtonID == 1:
                mouseState.Button1 = False

        mouseStatePrev.assign(mouseState)
        
    '''
    # Work on Keyboard Events
    KeyboardEvent = InputManager.getKeyboardEvent()
    if KeyboardEvent:
        # Set the state to the last state, then update according to events
        keyboardState = KeyboardState()
        if keyboardStatePrev:
            keyboardState.assign(keyboardStatePrev)
        else:
            keyboardState.setDefault()

        bGUIConsumeEvent = False
        if GUIManager:
            bGUIConsumeEvent = GUIManager.injectKeyboardEvent(KeyboardEvent)

        keyboardState.Keys[KeyboardEvent.KeyboardValue] = (KeyboardEvent.KeyboardEventID == Foundation.Events.KEYBOARD_KEY_PRESSED)

        keyboardStatePrev.assign(keyboardState)
    else:
        # If we don't have a keyboard event, but we have a keydown, send it
        if keyboardStatePrev:
            if keyboardStatePrev.isKeyDown():
                keyboardState = KeyboardState()
                keyboardState.assign(keyboardStatePrev)
    '''

    # Work on Keyboard Events
    # Set the state to the last state, then update according to events
    keyboardState = KeyboardState()
    if keyboardStatePrev:
        keyboardState.assign(keyboardStatePrev)
    else:
        keyboardState.setDefault()

    KeyboardEvent = InputManager.getKeyboardEvent()
    while (KeyboardEvent):
        bGUIConsumeEvent = False
        if GUIManager:
            bGUIConsumeEvent = GUIManager.injectKeyboardEvent(KeyboardEvent)

        keyboardState.Keys[KeyboardEvent.KeyboardValue] = (KeyboardEvent.KeyboardEventID == Foundation.Events.KEYBOARD_KEY_PRESSED)
        keyboardStatePrev.assign(keyboardState)

        KeyboardEvent = InputManager.getKeyboardEvent()

    # Work on Joystick Events
    JoystickEvent = InputManager.getJoystickEvent()
    if JoystickEvent:
        joystickState = JoystickState()
        joystickState.setDefault()

    while (JoystickEvent):
        joystickState.Index = JoystickEvent.JoystickIndex

        if JoystickEvent.JoystickEventID == Foundation.Events.JOYSTICK_MOVE:
            #if abs(joystickStatePrev.Axis[JoystickEvent.JoystickAxis] - joystickState.Axis[JoystickEvent.JoystickAxis]) > JOYSTICK_AXIS_ALLOWANCE:
            joystickState.Axis[JoystickEvent.JoystickAxis] = JoystickEvent.JoystickValue
        
        if JoystickEvent.JoystickEventID == Foundation.Events.JOYSTICK_BUTTON_PRESSED:
            joystickState.Button[JoystickEvent.JoystickButtonID] = (JoystickEvent.JoystickValue > 0)

        if JoystickEvent.JoystickEventID == Foundation.Events.JOYSTICK_BUTTON_RELEASED:
            joystickState.Button[JoystickEvent.JoystickButtonID] = (JoystickEvent.JoystickValue > 0)

        JoystickEvent = InputManager.getJoystickEvent()

    # Return the states
    return mouseState, keyboardState, joystickState
    

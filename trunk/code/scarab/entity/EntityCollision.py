# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation

# --------------------------------------------------
# Python Libs


class EntityCollision:
    def __init__(self, _uShape, _nSize):
        self.m_nId = Foundation.PhysicsManager().createCollisionShape(_uShape, _nSize)
        self.m_nPosition = Foundation.Vector3()
        self.m_nRotation = Foundation.Quaternion()
        self.m_uCollideObjects = []
        self.m_uCollideObjectsPoints = {}
        self.m_uShape = None
        self.m_nSize = _nSize
        
    # Accessor functions for ID
    def getId(self):
        return self.m_nId

    # Position
    def getPosition(self):
        return self.m_nPosition
    def setPosition(self, _nPosition):
        self.m_nPosition = _nPosition
        
        Foundation.PhysicsManager().setCollisionShapeOrigin(self.m_nId, _nPosition)
    
    # Rotation
    def getRotation(self):
        return self.m_nRotation
    def setRotation(self, _nRotation):
        self.m_nRotation = _nRotation
        
        Foundation.PhysicsManager().setCollisionShapeRotation(self.m_nId, _nRotation)
        
    # Shape
    def getShape(self):
        return self.m_uShape
    def setShape(self, _uShape):
        self.m_uShape = _uShape
        
    # Size
    def getSize(self):
        return self.m_nSize
    def setSize(self, _nSize):
        self.m_nSize = _nSize

    # Collision Objects
    def addCollideObject(self, _uObject):
        self.m_uCollideObjects.append(_uObject)
    def removeCollideObject(self, _uObject):
        self.m_uCollideObjects.remove(_uObject)
        #if self.m_uCollideObjectsPoints.has_key(_uObject):
        #    self.m_uCollideObjectsPoints[_uObject] = None
    def getCollideObject(self):
        return self.m_uCollideObjects
    #def getCollideObjectPoints(self, _uObject):
    #    return self.m_uCollideObjectsPoints[_uObject]
    def clearCollideObject(self):
        self.m_uCollideObjects = []
        #self.m_uCollideObjectsPoints.clear()

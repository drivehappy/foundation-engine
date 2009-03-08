# --------------------------------------------------
# Python Libs
import stackless
import random

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Entity.Actor import Actor
from Entity.Well import Well
from Entity.Pipeline import Pipeline
from Log.HTTPLogger import *

# --------------------------------------------------
# Property
class Property():

    def __init__(self, wellCount, name=''):
        self.wells = {}
        for k in range(0, wellCount):
            self.wells[k] = Well(self, "Well" + str(k))
        self.neighbors = []
        self.pipelines = []
        self.owner = None
        self.name = name

    def addNeighbor(self, property):
        self.neighbors.append(property)

    def getDerrickCount(self):
        value = 0
        for well in self.wells:
            value += self.wells[well].getValue()
        return value

    def getCost(self):
        return 1000 * len(self.wells)

    def pipelineNeighbor(self, neighbor):
        # Check to make sure we don't have 3 pipelines already
        if (len(self.pipelines) < 3):
            if len(self.pipelines) < (self.getDerrickCount() - 3):
                newPipeline = Pipeline(self, neighbor)
                self.pipelines.append(newPipeline)
                HTTPLogger().writeContent(LoggerError.SUCCESS, self.name + ": I pipelined successfully to %s" % (neighbor.name))
            else:
                HTTPLogger().writeContent(LoggerError.ERROR, self.name + ": Not enough derricks to support another pipeline (I have %i derricks and %i pipelines)" % (self.getDerrickCount(), len(self.pipelines)))
        else:
            HTTPLogger().writeContent(LoggerError.ERROR, self.name + ": Pipelines maxed out, cannot place.")

    def getUntappedWellCount(self):
        count = 0
        for wellIndex in self.wells:
            if not self.wells[wellIndex].isTapped():
                count += 1
        return count

    def getPipelineCountIntoPlayer(self, player):
        propertyPipelineCount = {}
        for pipeline in self.pipelines:
            targetProperty = pipeline.target
            
            if targetProperty.owner == player:
                if not propertyPipelineCount.has_key(targetProperty):
                    propertyPipelineCount[targetProperty] = 0

                propertyPipelineCount[targetProperty] += 1

        return propertyPipelineCount

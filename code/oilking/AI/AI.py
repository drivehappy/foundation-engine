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
from Entity.Actor import Actor
from Common.Common import Message, GameState
from Entity.Card import Disaster

# --------------------------------------------------
# AIPlayer
class AIPlayer(Actor):

    def __init__(self, player=None, world=None):
        Actor.__init__(self, self.__handleTasklet)
        self.player = player
        self.player.aiplayer = self
        self.world = world
        
        self.myturn = False
        self.receivedCard = None
        self.receivedBankProperties = False
        self.bankProperties = None
        
        self.drillsRemaining = 0
        self.propertyRemaining = 0


        stackless.tasklet(self.__handleNonblockingTasklet)()

    def __handleNonblockingTasklet(self):
        while True:
            if self.shutdownFlag:
                raise TaskletExit

            if self.myturn:
                while not self.receivedCard:
                    #print "AI: Waiting on my card"
                    stackless.schedule()

                HTTPLogger().writeContent(LoggerError.DEBUG, self.player.name + ": I have %i drills remaining and %i properties to purchase" % (self.drillsRemaining, self.propertyRemaining))

                while self.drillsRemaining > 0:
                    if not self.drillRandomWell():
                        break

                if self.propertyRemaining:
                    # Request a list of bank properties
                    self.player.bank.send((self.player.channel, Message.REQUEST_PROPERTIES))

                    while not self.receivedBankProperties:
                        print "AI: Waiting on bank to reply with available properties"
                        stackless.schedule()

                    #print "Done. Purchasing property from bank..."
                    self.purchaseRandomProperty(self.bankProperties)

                self.pipelineRandomProperty()

                # End my turn I'm done
                self.endTurn()

            stackless.schedule()

    def __handleTasklet(self, channelData):
        pass

    def onGameReady(self, properties):
        HTTPLogger().writeContent(LoggerError.DEBUG, "AI Player received game ready")
        self.purchaseRandomProperty(properties)

    def purchaseRandomProperty(self, properties):
        if len(properties):
            randProperty = properties[random.randint(0, len(properties) - 1)]
            self.player.bank.send((self.player.channel, Message.REQUEST_PURCHASE, randProperty))

    def pipelineRandomProperty(self):
        if self.player.cash > 35000:
            HTTPLogger().writeContent(LoggerError.DEBUG, "AI Player looking for best property to pipeline")

            # Acceptable amount of cash, check for the best property
            bestProperty = None
            bestPropertyTarget = None
            for property in self.player.properties:
                for neighbor in property.neighbors:
                    if bestProperty:
                        if neighbor.getDerrickCount() > property.getDerrickCount():
                            bestPropertyTarget = neighbor
                            bestProperty = property
                    elif neighbor != self.player.bank:
                        if neighbor.getDerrickCount() > 0:
                            bestPropertyTarget = neighbor
                            bestProperty = property

            # Pipeline from our best property to the best target
            if bestProperty:
                if bestProperty.getDerrickCount() >= 4:
                    HTTPLogger().writeContent(LoggerError.DEBUG, "AI Player found best property pipeline: %s to %s" % (bestProperty.name, bestPropertyTarget.name))
                    bestProperty.pipelineNeighbor(bestPropertyTarget)

    def drillRandomWell(self):
        wellToDrill = None
        
        # Attempt to drill on my property
        for property in self.player.properties:
            for wellIndex in property.wells:
                if not property.wells[wellIndex].isTapped():
                    wellToDrill = property.wells[wellIndex]
                    break
            if wellToDrill:
                break

        if wellToDrill:
            self.player.drillWell(wellToDrill)

            derrickCount = self.player.getDerrickCount()
            HTTPLogger().writeContent(LoggerError.DEBUG, self.player.name + ": After drilling I now have %i Derricks" % (derrickCount))

            self.drillsRemaining -= 1
        else:
            HTTPLogger().writeContent(LoggerError.ERROR, self.player.name + ": I couldn't find any available wells to drill, untapped wells on this property: %i" % (self.player.properties[0].getUntappedWellCount()))
            return False

        return True

    def onReceivedProperties(self, properties):
        self.receivedBankProperties = True
        self.bankProperties = properties

    def onPurchaseReply(self, success, property):
        HTTPLogger().writeContent(LoggerError.DEBUG, "AI Player received property purchase reply: %s" % (success))

    def onTurn(self):
        HTTPLogger().writeContent(LoggerError.DEBUG, "AI Player entered turn")

        self.myturn = True

    def onForfeitGameRequested(self):
        HTTPLogger().writeContent(LoggerError.DEBUG, "AI Player has been informed of forfeit request.")

    def onForfeitGameComplete(self):
        HTTPLogger().writeContent(LoggerError.DEBUG, "AI Player has been informed of forfeit completed.")

    def onReceiveCard(self):
        HTTPLogger().writeContent(LoggerError.DEBUG, "AI Player received card")

        self.receivedCard = self.player.card
        self.drillsRemaining = self.receivedCard.drillMin
        self.propertyRemaining = self.receivedCard.property

        if self.receivedCard.disaster == Disaster.FIRE:
            # Remove x derricks here
            pass

    def endTurn(self):
        HTTPLogger().writeContent(LoggerError.DEBUG, self.player.name + ": Ending my turn... (cash = %i)" % (self.player.cash))

        self.world.send((self.player.channel, Message.PLAYER_TURN_DONE))
        self.myturn = False
        self.receivedCard = None
        self.receivedBankProperties = False

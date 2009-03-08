# --------------------------------------------------
# Python Libs
import stackless
import random

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation
from Common.Common import Message, GameState
from Entity.Actor import Actor
from Entity.Card import Card, Disaster
from AI.AI import AIPlayer
from Log.HTTPLogger import HTTPLogger, LoggerError

# --------------------------------------------------
# Player
class Player(Actor):

    def __init__(self, name='', bank=None):
        Actor.__init__(self, self.__handleTasklet)

        self.name = name
        self.properties = []
        self.cash = 0
        self.bank = bank
        self.gameState = GameState()
        self.world = None
        self.currentPlayerTurn = None
        self.aiplayer = None
        self.card = None
        
        stackless.tasklet(self.__handleNonblockingTasklet)()

    def __handleNonblockingTasklet(self):
        while True:
            if self.shutdownFlag:
                raise TaskletExit
            
            #print "Player.update", Foundation.TimeManager().getTime()
            stackless.schedule()

    def __handleTasklet(self, channelData):
        channel, msg, msgdata = channelData[0], channelData[1], channelData[2:]

        if msg == Message.WORLD_GAMEREADY:
            HTTPLogger().writeContent(LoggerError.SUCCESS, self.name + " Received World is Game Ready")
            self.gameState = GameState("GAMEREADY")
            properties = msgdata[0]
            self.world = channel

            if self.aiplayer:
                self.aiplayer.onGameReady(properties)

        elif msg == Message.PLAYER_TURN:
            self.currentPlayerTurn = msgdata[0]

            if self.currentPlayerTurn == self.channel:
                HTTPLogger().writeContent(LoggerError.NONE, self.name + " Taking My Turn")

                # Run AI if we have it
                if self.aiplayer:
                    self.aiplayer.onTurn()

                # Request a card
                channel.send((self.channel, Message.REQUEST_CARD))
            else:
                # Check to see if this player owes us money from pipelines
                pipelineCash = 0
                propertyPipelineCount = self.getPipelineCountIntoPlayer(self.currentPlayerTurn)

                for property in propertyPipelineCount:
                    for propertyTarget in propertyPipelineCount[property]:
                        pipelineCount = propertyPipelineCount[property][propertyTarget]
                        targetDerrickCount = propertyTarget.getDerrickCount()
                        if pipelineCount == 1:
                            pipelineCash += targetDerrickCount * 1000
                        elif pipelineCount == 2:
                            pipelineCash += targetDerrickCount * 3000
                        elif pipelineCount == 3:
                            pipelineCash += targetDerrickCount * 5000
                        else:
                            HTTPLogger().writeContent(LoggerError.ERROR, self.name + ": Pipeline count must be off, I received that I have %i on property" % (pipelineCount))
                        HTTPLogger().writeContent(LoggerError.ERROR, self.name + ": Target had %i derricks, I had %i pipelines, I gained a total of %i cash" % (targetDerrickCount, pipelineCount, pipelineCash))

                if pipelineCash > 0:
                    self.currentPlayerTurn.send((self.channel, Message.PLAYER_CASH_DELTA, -pipelineCash, True))

                    HTTPLogger().writeContent(LoggerError.DEBUG, self.name + ": Other player owed me a total of %i for my pipelines" % (pipelineCash))

        elif msg == Message.REQUEST_PROPERTIES:
            channel.send((self.channel, Message.RECEIVE_PROPERTIES, self.properties))

        elif msg == Message.RECEIVE_PROPERTIES:
            if self.aiplayer:
                self.aiplayer.onReceivedProperties(msgdata[0])

        elif msg == Message.PLAYER_CASH_DELTA:
            cashDelta = msgdata[0]
            self.cash += cashDelta
            self.cash = Foundation.f_clamp(self.cash, 0, 10000000)

            HTTPLogger().writeContent(LoggerError.DEBUG, self.name + " Delta Cash: %i, Total Cash: %i" % (msgdata[0], self.cash))

            # If we got positive change, reduce the sender's by that amount
            if (msgdata[1]):
                channel.send((self.channel, Message.PLAYER_CASH_DELTA, -cashDelta, False))

        elif msg == Message.REQUEST_PURCHASE:
            requestedProperty = msgdata[0]

            if requestedProperty.owner == self.channel:
                # Release our property to the player
                requestedProperty.owner = None
                self.properties.remove(requestedProperty)
                channel.send((self.channel, Message.PLAYER_PURCHASE, True, requestedProperty))
            else:
                # Send available properties that can be purchased from me instead
                channel.send((self.channel, Message.PLAYER_PURCHASE, False, self.properties))

        elif msg == Message.RP_FORFEIT_GAME:
            playerForfeited = msgdata[0]

            if playerForfeited == self.channel:
                HTTPLogger().writeContent(LoggerError.NONE, self.name + ": I successfully forfeited the game. Do auction here.")
                self.shutdownFlag = True
            else:
                HTTPLogger().writeContent(LoggerError.NONE, self.name + ": Player forfeited from game.")

        elif msg == Message.PLAYER_PURCHASE:
            Success = msgdata[0]
            Property = msgdata[1]

            if Success:
                HTTPLogger().writeContent(LoggerError.NONE, self.name + ": %s purchased. Value = %i" % (Property.name, Property.getCost()))
                Property.owner = self.channel
                self.properties.append(Property)

                # Pay the purchasee
                channel.send((self.channel, Message.PLAYER_CASH_DELTA, Property.getCost(), True))

                # If this is the initial property tell the World we're ready to play
                if self.gameState.state == "GAMEREADY":
                    self.world.send((self.channel, Message.PLAYER_GAMEREADY))
                    self.gameState.state = "PLAYERREADY"
            else:
                HTTPLogger().writeContent(LoggerError.NONE, self.name + " Property could not be purchased (returned %i alternatives)" % (len(Property)))

                newPurchaseProperty = Property[random.randint(0, len(Property) - 1)]
                self.bank.send((self.channel, Message.REQUEST_PURCHASE, newPurchaseProperty))

            # Send it off to the AI
            if self.aiplayer:
                self.aiplayer.onPurchaseReply(Success, Property)

        elif msg == Message.GAME_START:
            HTTPLogger().writeContent(LoggerError.NONE, self.name + ": Received Game Start")
            self.gameState.state = "GAMESTART"

        elif msg == Message.RECEIVE_CARD:
            self.card = msgdata[0]

            HTTPLogger().writeContent(LoggerError.NONE, self.name + " Received card, monetary value of %i, disaster value of %i" % (self.card.monetaryValue, self.card.disaster))

            # Determine what we're owed
            cashValue = 0
            if self.card.disaster == Disaster.NONE:
                cashValue = self.card.monetaryValue * self.getDerrickCount()
            elif self.card.disaster == Disaster.DEPLETION:
                cashValue = self.card.monetaryValue

            # Check if we have wells available to drill in or not, if not
            wellCount = 0
            for property in self.properties:
                for wellIndex in property.wells:
                    if not property.wells[wellIndex].isTapped():
                        wellCount += 1

            if wellCount == 0:
                HTTPLogger().writeContent(LoggerError.DEBUG, self.name + ": I have no wells available to drill. I'm paying the $10,000 fine.")
                cashValue -= 10000

            # Request funds from bank
            self.bank.send((self.channel, Message.PLAYER_CASH_DELTA, -cashValue, True))

            # Check if we have wells available to drill in or not, if not
            wellCount = 0
            for property in self.properties:
                for wellIndex in property.wells:
                    if not property.wells[wellIndex].isTapped():
                        wellCount += 1

            # Insure we have cash (plus expected cash value) to operate
            if self.cash + cashValue <= 0:
                HTTPLogger().writeContent(LoggerError.DEBUG, self.name + ": I'm out of cash, I've lost")
                self.world.send((self.channel, Message.RQ_FORFEIT_GAME))

                if self.aiplayer:
                    self.aiplayer.onForfeitGameRequested()

            # Run AI if we have it
            if self.aiplayer:
                self.aiplayer.onReceiveCard()

        elif msg == Message.GAME_DONE:
            HTTPLogger().writeContent(LoggerError.DEBUG, self.name + ": I've won the game with %i cash left" % (self.cash))

            # Shut me down
            self.shutdownFlag = True

        else:
            HTTPLogger().writeContent(LoggerError.DEBUG, self.name + " Received unhandled message: 0x%x" % (msg))

    def drillWell(self, well):
        HTTPLogger().writeContent(LoggerError.NONE, self.name + " Attemping to drill well (%s)." % (well.name))
        if well.propertyOwner.owner == self.channel:
            if not well.isTapped():
                cost = well.getDrillCostMin()
                if cost <= self.cash:
                    well.drill()

                    # Send the bank our drilling cost, he will subtract it from us
                    self.bank.send((self.channel, Message.PLAYER_CASH_DELTA, well.getDrillCost(), True))
                else:
                    HTTPLogger().writeContent(LoggerError.ERROR, self.name + " Cannot drill, player doesn't have minimum funds (2000).")
            else:
                HTTPLogger().writeContent(LoggerError.ERROR, self.name + " Cannot drill, well is already tapped.")
        else:
            HTTPLogger().writeContent(LoggerError.ERROR, self.name + " Attempted to drill on unowned property.")

        return 0

    def getDerrickCount(self):
        derrickCount = 0
        for property in self.properties:
            derrickCount += property.getDerrickCount()
            
        return derrickCount

    def getPipelineCountIntoPlayer(self, player):
        propertyPipelineCount = {}
        for property in self.properties:
            propertyPipelineCount[property] = property.getPipelineCountIntoPlayer(player)

        return propertyPipelineCount

    def payBank(self, amount):
        self.bank.send((self.channel, Message.PLAYER_CASH_DELTA, amount, True))
    

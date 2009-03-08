# --------------------------------------------------
# Python Libs
import stackless
import random

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation

from Log.HTTPLogger import HTTPLogger

from Common.Common import Message
from Entity.Actor import Actor
from Entity.Player import *
from Entity.Property import *
from Entity.Card import *
from AI.AI import AIPlayer

# --------------------------------------------------
# Unit
class World(Actor):

    def __init__(self, propertyCount, playerCount):
        Actor.__init__(self, self.__handleTasklet)

        self.properties = []
        self.players = []
        self.bank = None
        self.playerTurnIndex = 0
        self.cardDeck = Deck()
        self.readyCheck = {}
        self.aiplayers = []

        # Shuffle deck
        self.cardDeck.shuffle()

        # Populate properties
        for k in range(0, propertyCount):
            newProperty = Property(random.randint(7, 13), "Property" + str(k))
            self.properties.append(newProperty)

        # Create some random neighbors for testing
        for property in self.properties:
            randProperty = property
            while randProperty == property:
                randProperty = self.properties[random.randint(0, len(self.properties) - 1)]
            property.addNeighbor(randProperty)

        # Create the bank
        self.bank = Player("Bank").channel
        self.bank.send((self.channel, Message.PLAYER_CASH_DELTA, 1000000, False))

        # Populate players
        for k in range(0, playerCount):
            newPlayer = Player("Player" + str(k), self.bank)
            newPlayerChannel = newPlayer.channel
            newPlayerChannel.send((self.channel, Message.PLAYER_CASH_DELTA, 80000, False))
            self.players.append(newPlayerChannel)
            self.readyCheck[newPlayerChannel] = False

            # For each player, test an AI out
            aiPlayer = AIPlayer(newPlayer, self.channel)
            self.aiplayers.append(aiPlayer)

        # Give properties to the bank
        for property in self.properties:
            self.bank.send((self.channel, Message.PLAYER_PURCHASE, True, property))

        # Send our players that the world is ready
        for player in self.players:
            player.send((self.channel, Message.WORLD_GAMEREADY, self.properties))

        stackless.tasklet(self.__handleNonblockingTasklet)()

    def __handleNonblockingTasklet(self):
        while True:
            if self.shutdownFlag:
                raise TaskletExit

            #print "World.update", Foundation.TimeManager().getTime()
            stackless.schedule()

    def __handleTasklet(self, channelData):
        channel, msg, msgdata = channelData[0], channelData[1], channelData[2:]

        if msg == Message.PLAYER_GAMEREADY:
            HTTPLogger().writeContent(LoggerError.NONE, "World Received Player is Ready")
            self.readyCheck[channel] = True
            
            if self.__checkAllPlayersReady():
                HTTPLogger().writeContent(LoggerError.SUCCESS, "All players ready")
                self.__handleGameStart()

        elif msg == Message.REQUEST_CARD:
            requestedCard = self.cardDeck.pop()

            channel.send((self.channel, Message.RECEIVE_CARD, requestedCard))

        elif msg == Message.PLAYER_TURN_DONE:
            HTTPLogger().writeContent(LoggerError.SUCCESS, "Player ended turn")

            self.__doPlayerTurn()

        elif msg == Message.RQ_FORFEIT_GAME:
            self.__handlePlayerForfeit(channel)

            if len(self.players) == 1:
                self.__handleGameDone(self.players[0])
                self.shutdownFlag = True
                print "Game Done"
                raise KeyboardInterrupt
            else:
                print "%i Players Remaining" % len(self.players)

    def __handleGameStart(self):
        for player in self.players:
            player.send((self.channel, Message.GAME_START))
        self.__handlePlayerTurn()

    def __handlePlayerTurn(self):
        for player in self.players:
            player.send((self.channel, Message.PLAYER_TURN, self.players[self.playerTurnIndex]))

    def __checkAllPlayersReady(self):
        ready = True
        for player in self.players:
            if self.readyCheck.has_key(player):
                if not self.readyCheck[player]:
                    ready = False
                    break

        return ready

    def __handlePlayerForfeit(self, playerForfeited):
        self.players.remove(playerForfeited)
        for player in self.players:
            player.send((self.channel, Message.RP_FORFEIT_GAME, playerForfeited))

    def __handleGameDone(self, playerWinner):
        playerWinner.send((self.channel, Message.GAME_DONE, playerWinner))

    def __doPlayerTurn(self):
        self.playerTurnIndex += 1
        if self.playerTurnIndex >= len(self.players):
            self.playerTurnIndex = 0

        self.__handlePlayerTurn()

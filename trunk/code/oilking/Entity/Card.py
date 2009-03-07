# --------------------------------------------------
# Python Libs
import stackless
import random

# --------------------------------------------------
# Foundation Libs
import FoundationPython as Foundation

from Log.HTTPLogger import HTTPLogger, LoggerError

from Common.Common import Message

# --------------------------------------------------
# Disasters
class Disaster:
    NONE        = 0x0
    DEPLETION   = 0x1
    FIRE        = 0x2
    
# --------------------------------------------------
# Card
class Card():

    def __init__(self, name='', property=False, drillMin=0, drillMax=0, monetaryValue=0, disaster=Disaster.NONE):
        self.name = name
        self.property = property
        self.monetaryValue = monetaryValue
        self.drillMin = drillMin
        self.drillMax = drillMax
        self.disaster = disaster

# --------------------------------------------------
# Deck
class Deck():

    def __init__(self):
        self.cards = []

        # Generate high end cards
        for drillCount in range(0, 4):
            for earnings in range(3, 5):
                newCard = Card("", False, 1)
                newCard.drillMax = drillCount + 1
                newCard.monetaryValue = earnings * 1000
                self.cards.append(newCard)

        # Generate the low end cards
        ## $500 Range
        for i in range(0, 2):
            newCard = Card("", 1, 1, 2, 500)
            self.cards.append(newCard)

        for i in range(0, 3):
            newCard = Card("", 0, 1, 2, 500)
            self.cards.append(newCard)
            
        newCard = Card("", 0, 1, 3, 500)
        self.cards.append(newCard)

        newCard = Card("", 0, 1, 4, 500)
        self.cards.append(newCard)
        newCard = Card("", 1, 1, 4, 500)
        self.cards.append(newCard)

        ## $1000 Range
        for i in range(1, 4):
            newCard = Card("", 1, 1, i, 1000)
            self.cards.append(newCard)

        ## $2000 Range
        newCard = Card("", 1, 1, 1, 2000)
        self.cards.append(newCard)
        newCard = Card("", 0, 1, 2, 2000)
        self.cards.append(newCard)
        for i in range(0, 2):
            newCard = Card("", 0, 1, 3, 1000)
            self.cards.append(newCard)
        for i in range(0, 2):
            newCard = Card("", 0, 1, 4, 1000)
            self.cards.append(newCard)

        # Generate disaster cards
        ## Fire
        for i in range(0, 2):
            newFireCard = Card("", 0, 0, 0, 0, Disaster.FIRE)
            self.cards.append(newFireCard)

        ## Depletion
        for i in range(0, 5):
            newDepletionCard = Card("", 0, 1, 1, 500, Disaster.DEPLETION)
            self.cards.append(newDepletionCard)

        # And dump a debug
        HTTPLogger().writeContent(LoggerError.SUCCESS, "Deck created %i cards" % (self.size()))

    def shuffle(self):
        self.shuffled = []

        while len(self.cards):
            randIndex = random.randint(0, len(self.cards) - 1)
            randCard = self.cards[randIndex]
            self.cards.remove(randCard)
            self.shuffled.append(randCard)

        self.cards = self.shuffled

    def size(self):
        return len(self.cards)

    def pop(self):
        if len(self.cards):
            topCard = self.cards[0]
            self.cards.remove(topCard)

            # Reinsert it at the end
            self.cards.append(topCard)

            return topCard
        
        HTTPLogger().writeContent(LoggerError.ERROR, "No cards on deck to hand out.")
        return None


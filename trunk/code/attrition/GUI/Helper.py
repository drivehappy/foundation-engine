# --------------------------------------------------
# GUI
# --------------------------------------------------

import FoundationPython as Foundation

GUIManager      = Foundation.GUIManager()
GraphicManager  = Foundation.GraphicManager()

class Helper:
    # -----------------------------------
    # Game UI
    def createGameUI(self):
        GUIManager.createWindow("", "Window_Stats", 10, 600, 350, 160, "Stats")
        GUIManager.setObjectColor("Window_Stats", 1, 1, 1)
        GUIManager.createStaticText("Window_Stats", "Text_FPS", 10, 10, 150, 20, "FPS: 0.0")
        GUIManager.setObjectColor("Text_FPS", 1, 1, 1)
        GUIManager.createStaticText("Window_Stats", "Text_DeltaTime", 10, 30, 150, 20, "dTime: 0.0")
        GUIManager.setObjectColor("Text_DeltaTime", 1, 1, 1)
        GUIManager.createStaticText("Window_Stats", "Text_TotalTime", 10, 50, 150, 20, "tTime: 0.0")
        GUIManager.setObjectColor("Text_TotalTime", 1, 1, 1)
        GUIManager.createStaticText("Window_Stats", "Text_EntityCount", 10, 70, 150, 20, "Entities: 0")
        GUIManager.setObjectColor("Text_EntityCount", 1, 1, 1)
        GUIManager.createStaticText("Window_Stats", "Text_BucketSize", 10, 90, 150, 20, "BucketSize: 3")
        GUIManager.setObjectColor("Text_BucketSize", 1, 1, 1)

    def updateGameUI(self, _nDeltaTime, _nTotalTime, _nFPS, _nEntityCount, _nBucketSize):
        GUIManager.setWidgetCaption("Text_FPS", ("FPS: %3.2f" % _nFPS))
        GUIManager.setWidgetCaption("Text_DeltaTime", ("dTime: %2.6f" % _nDeltaTime))
        GUIManager.setWidgetCaption("Text_TotalTime", ("tTime: %2.6f" % _nTotalTime))
        GUIManager.setWidgetCaption("Text_EntityCount", ("Entities: %i" % _nEntityCount))
        GUIManager.setWidgetCaption("Text_BucketSize", ("BucketSize: %i" % _nBucketSize))

    def cleanupGameUI(self):
        GUIManager.destroyObject("Window_Stats")

    # ------------------------------------
    # Entity UI
    def createEntityUI(self):
        GUIManager.createWindow("", "Window_Entity", 370, 600, 500, 160, "Entity")
        GUIManager.setObjectColor("Window_Entity", 1, 1, 1)
        GUIManager.createStaticText("Window_Entity", "Text_Entity_Name", 10, 10, 250, 20, "Name: None")
        GUIManager.setObjectColor("Text_Entity_Name", 1, 1, 1)
        GUIManager.createStaticText("Window_Entity", "Text_Entity_CreationList", 10, 30, 250, 20, "Creates: None")
        GUIManager.setObjectColor("Text_Entity_CreationList", 1, 1, 1)
        GUIManager.createStaticText("Window_Entity", "Text_Entity_BuildQueue", 10, 50, 450, 20, "Build Queue: None")
        GUIManager.setObjectColor("Text_Entity_BuildQueue", 1, 1, 1)
        GUIManager.createStaticText("Window_Entity", "Text_Entity_BuildProgress", 10, 70, 450, 20, "Build Progress: 0%")
        GUIManager.setObjectColor("Text_Entity_BuildProgress", 1, 1, 1)
        GUIManager.createStaticText("Window_Entity", "Text_Entity_WeaponList", 10, 90, 450, 20, "Weapons: None")
        GUIManager.setObjectColor("Text_Entity_WeaponList", 1, 1, 1)

        # Create Creation Buttons
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create0", 300, 10, 60, 20, "0")
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create1", 370, 10, 60, 20, "1")
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create2", 440, 10, 60, 20, "2")
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create3", 300, 30, 60, 20, "3")
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create4", 370, 30, 60, 20, "4")
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create5", 440, 30, 60, 20, "5")
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create6", 300, 50, 60, 20, "6")
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create7", 370, 50, 60, 20, "7")
        GUIManager.createButton("Window_Entity", "Btn_Entity_Create8", 440, 50, 60, 20, "8")
        GUIManager.setObjectColor("Btn_Entity_Create0", 1, 1, 1)
        GUIManager.setObjectColor("Btn_Entity_Create1", 1, 1, 1)
        GUIManager.setObjectColor("Btn_Entity_Create2", 1, 1, 1)
        GUIManager.setObjectColor("Btn_Entity_Create3", 1, 1, 1)
        GUIManager.setObjectColor("Btn_Entity_Create4", 1, 1, 1)
        GUIManager.setObjectColor("Btn_Entity_Create5", 1, 1, 1)
        GUIManager.setObjectColor("Btn_Entity_Create6", 1, 1, 1)
        GUIManager.setObjectColor("Btn_Entity_Create7", 1, 1, 1)
        GUIManager.setObjectColor("Btn_Entity_Create8", 1, 1, 1)

    def updateEntityUI(self, _nDeltaTime, _uEntity):
        if _uEntity:
            GUIManager.setWidgetCaption("Text_Entity_Name", ("Name: %s" % _uEntity.name))

            # Update creation ability text, and update our buttons accordingly
            GUIManager.setWidgetCaption("Text_Entity_CreationList", ("Creates: %s" % _uEntity.creationAbilities))
            sUnitList = _uEntity.creationAbilities
            nIndex = 0
            if sUnitList:
                for nIndex, sEntity in enumerate(sUnitList):
                    GUIManager.setWidgetCaption("Btn_Entity_Create" + str(nIndex), ("%s" % sEntity))
                    GUIManager.setWidgetVisible("Btn_Entity_Create" + str(nIndex), True)
                nIndex += 1
            for i in range(nIndex, 9):
                GUIManager.setWidgetVisible("Btn_Entity_Create" + str(i), False)

            # Update build queue
            sBuildQueue = ""
            for uEntityType in _uEntity.creationQueue:
                sBuildQueue += uEntityType["Name"] + " "
            GUIManager.setWidgetCaption("Text_Entity_BuildQueue", ("Build Queue: %s" % sBuildQueue))

            # Update build progress
            nProgress = 0.0
            nTimeRemaining = 0.0
            if _uEntity.creatingUnit and _uEntity.creationTime > 0.0:
                nProgress = _uEntity.creationTimer.getTime() / _uEntity.creationTime * 100.0
                nTimeRemaining = _uEntity.creationTime - _uEntity.creationTimer.getTime()
            GUIManager.setWidgetCaption("Text_Entity_BuildProgress", ("Build Progress: %i%% (%2.1fs)" % (nProgress, nTimeRemaining)))

            # Update current weapos on the entity
            sWeaponList = ""
            for uWeapon in _uEntity.weaponList:
                sWeaponList += uWeapon.name + " "
            GUIManager.setWidgetCaption("Text_Entity_WeaponList", "Weapons: " + (sWeaponList))

        else:
            GUIManager.setWidgetCaption("Text_Entity_Name", ("Name: None"))
            GUIManager.setWidgetCaption("Text_Entity_CreationList", ("Name: None"))
            GUIManager.setWidgetCaption("Text_Entity_WeaponList", ("Weapons: None"))

    def cleanupEntityUI(self):
        GUIManager.destroyObject("Window_Entity")


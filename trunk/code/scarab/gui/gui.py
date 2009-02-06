# --------------------------------------------------
# GUI
# --------------------------------------------------

import FoundationPython as Foundation

GUIManager      = Foundation.GUIManager()
GraphicManager  = Foundation.GraphicManager()

class GUIHelper:
    def createGameUI(self):
        GUIManager.createWindow("", "Window_Stats", 10, 600, 350, 160, "Stats")
        GUIManager.setObjectColor("Window_Stats", 1, 1, 1)
        GUIManager.createStaticText("Window_Stats", "Text_FPS", 10, 10, 150, 20, "FPS: 0.0")
        GUIManager.setObjectColor("Text_FPS", 1, 1, 1)
        GUIManager.createStaticText("Window_Stats", "Text_DeltaTime", 10, 30, 150, 20, "dTime: 0.0")
        GUIManager.setObjectColor("Text_DeltaTime", 1, 1, 1)
        GUIManager.createStaticText("Window_Stats", "Text_TotalTime", 10, 50, 150, 20, "tTime: 0.0")
        GUIManager.setObjectColor("Text_TotalTime", 1, 1, 1)

    def updateGameUI(self, _nDeltaTime, _nTotalTime, _nFPS):
        GUIManager.setWidgetCaption("Text_FPS", ("FPS: %3.2f" % _nFPS))
        GUIManager.setWidgetCaption("Text_DeltaTime", ("dTime: %2.6f" % _nDeltaTime))
        GUIManager.setWidgetCaption("Text_TotalTime", ("tTime: %2.6f" % _nTotalTime))

    def cleanupGameUI(self):
        GUIManager.destroyObject("Button_Start")


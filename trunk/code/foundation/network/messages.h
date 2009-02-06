#if !defined(__F_MESSAGES_H_)
#define __F_MESSAGES_H_

// Internal messages used in communicator
#define MSG_OK              0x0000  // General ok
#define MSG_ERR             0x0001  // General error
#define MSG_NEWCONN         0x0002  // Player connection added
#define MSG_DISCONN         0x0003  // Player connection removed
#define MSG_CONNECT         0x0004  // Client connected to server
#define MSG_DISCONNECT      0x0005  // Client disconnected to server

#define MSG_MKGAME          0x0010  // Request create game
#define MSG_RMGAME          0x0011  // Request remove game
#define MSG_JNGAME          0x0012  // Join game
#define MSG_LVGAME          0x0013  // Leave game
#define MSG_STGAME          0x0014  // Start game
#define MSG_SDGAME          0x0015  // Join side

#define MSG_GMENTITYCREATE  0x0020  // Create entity
#define MSG_GMENTITYDESTROY 0x0021  // Destroy entity
#define MSG_GMENTITYMOVE    0x0022  // Move entity
#define MSG_GMENTITYHALT    0x0023  // Stop entity movement and rotation
#define MSG_GMENTITYORIENT  0x0024  // Orientate entity
#define MSG_GMENTITYATTACK  0x0025  // Attack enemy entity
#define MSG_GMENTITYTYPES   0x0026  // Type list, sent at start of game

#define MSG_GMENTITYUPDATE  0x0030  // Server->Client send entities seen by player
#define MSG_GMINITIALDATA   0x0031  // Server->Client send player names/ids/sideids

#define MSG_GMSERVERMSG     0x0040  // Server message
#define MSG_GMPLAYERMSG     0x0041  // Player message

 /* Format specs for my sanity
 (Client->Server)
    GMSERVERMSG:     NONE
    GMPLAYERMSG:     PLAYERID[6],TYPE[1]{0:Broadcast,1:Side,2:Private},TARGET[6]{NONE/PLAYERSIDE/PLAYERID},MESSAGELEN[1],MESSAGE[MESSAGELEN]

    GMENTITYUPDATE:  NONE
    GMINITIALDATA:   PLAYERID[6],PLAYERCOUNT[2],PLAYERCOUNT * (PLAYERID[6]+SIDEID[2])

    GMENTITYCREATE:  PLAYERID[6],TYPEID[4],POSITIONX[COORDINATE],POSITIONY[COORDINATE] ***(COORDINATE size = 12)
    GMENTITYDESTROY: PLAYERID[6],ENTITYID[4]
    GMENTITYMOVE:    PLAYERID[6],ENTITYID[4],POSITIONX[COORDINATE],POSITIONY[COORDINATE] ***(COORDINATE size = 12)
    GMENTITYHALT:    PLAYERID[6],ENTITYID[4]
    GMENTITYORIENT:  PLAYERID[6],ENTITYID[4],ORIENTATION[FLOAT] ***(FLOAT size = 4)
    GMENTITYATTACK:  PLAYERID[6],ENTITYID[4],ENTITYTARGETID[4]
    GMENTITYTYPES:   NONE

    MKGAME:  PLAYERID[6],GAMENAME[GAMENAMELEN],PASSWORD[GAMEPASSLEN],MAXPLAYERS[1]
    RMGAME:  PLAYERID[6],GAMENAME[GAMENAMELEN]
    JNGAME:  PLAYERID[6],GAMENAME[GAMENAMELEN],PASSWORD[GAMEPASSLEN]
    LVGAME:  PLAYERID[6]
    STGAME:  PLAYERID[6],GAMENAME[GAMENAMELEN]
    SDGAME:  PLAYERID[6],SIDE[2]

    NEWCONN: PLAYERID[6]
    DISCONN: PLAYERID[6]

 (Server->Client)
    GMSERVERMSG:     MESSAGELEN[1],MESSAGE[MESSAGELEN]
    GMPLAYERMSG:     PLAYERIDSOURCE[6],MESSAGELEN[1],MESSAGE[MESSAGELEN]

    GMENTITYUPDATE:  RETURN(ENTITYCOUNT[2] + ENTITY * ENTITYCOUNT)

    GMENTITYCREATE:  NONE
    GMENTITYDESTROY: RETURN(0x00:OK, 0x01:ERROR)
    GMENTITYMOVE:    RETURN(0x00:OK, 0x01:ERROR)
    GMENTITYHALT:    RETURN(0x00:OK, 0x01:ERROR)
    GMENTITYORIENT:  RETURN(0x00:OK, 0x01:ERROR)
    GMENTITYATTACK:  RETURN(0x00:OK, 0x01:ERROR)
    GMENTITYTYPES:   RETURN(ENTITYTYPECOUNT[2] + ENTITYTYPE * ENTITYTYPECOUNT)

    MKGAME:  ERRORCODE(0x00:OK, 0x01:ERROR, 0x02:EXISTING_NAME)
    RMGAME:  ERRORCODE(0x00:OK, 0x01:ERROR, 0x02:INVALID_NAME, 0x03:INVALID_PERMISSION)
    JNGAME:  ERRORCODE(0x00:OK, 0x01:ERROR, 0x02:INVALID_NAME, 0x03:INVALID_PASSWORD, 0x04:PLAYERLIMIT_FULL, 0x05:ALREADY_INGAME, 0x06:NOT_IN_LOBBY)
    LVGAME:  ERRORCODE(0x00:OK, 0x01:ERROR, 0x02:INVALID_NAME)
    STGAME:  ERRORCODE(0x00:OK, 0x01:ERROR, 0x02:INVALID_NAME, 0x03:INVALID_PERMISSION)
    SDGAME:  ERRORCODE(0x00:OK, 0x01:ERROR) + SIDE(2)

    CONNECT: SERVERID[6]
 */

// External network messages used in server<->client
#define MSG_RQGAMELIST      0x8000

/* Format specs for my sanity
 (Client->Server)
    RQGAMELIST:	 NONE

 (Server->Client)
    RQGAMELIST:	 RETURN(GAMELISTSIZE(2) + [GAMELISTSIZE*(GAMENAME(16)+PLAYERLIMIT(1)+PASSWORDREQUIRED(1)))
 */

#define MSG_GFX_MOUSE           0x0100  // Mouse params on callbackPick in the graphics system
#define MSG_GFX_ENTITYPOS       0x0110  // Update entity position
#define MSG_GFX_ENTITYCREATE    0x0111  // Create entity

/* Format specs for my sanity
 (Gfx->Game)
    MSG_GFX_MOUSE:          OBJECTID(8), WORLDLOCATION[0](4), WORLDLOCATION[1](4)
    MSG_GFX_ENTITYPOS:      None
    MSG_GFX_ENTITYCREATE:   None

 (Game->Gfx)
    MSG_GFX_MOUSE:          None
    MSG_GFX_ENTITYPOS:      OBJECTID(8), WORLDLOCATION[0](4), WORLDLOCATION[1](4)
    MSG_GFX_ENTITYCREATE:   OBJECTID(8), SIDE(4), WORLDLOCATION[0](4), WORLDLOCATION[1](4)
 */

#endif	// __F_MESSAGES_H_

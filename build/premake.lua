---------------------------------------------------------------------------------------
-- This file contains all the build options for the foundation library and additional
-- projects. Aside from 3rd party libraries everything pertaining to the compilation
-- and linking is in here.
--
-- (06/26/08 mharmer)
-- (01/10/09 mharmer - Updates to the new foundation engine)
---------------------------------------------------------------------------------------

-- Edit these vars as needed
--- Enable or disable 64 bit libraries
BUILD_LIB_64BIT = false

--- Enable or disable components of the engine
ENABLE_GRAPHIC  = true
ENABLE_NETWORK  = true
ENABLE_INPUT    = true
ENABLE_PHYSIC   = true
ENABLE_AUDIO    = true

if (linux) then
    GRAPHIC_LIB_INCDIR  = "/usr/local/include/OGRE"
    GUI_LIB_INCDIR      = "/usr/local/include/MYGUI"
    NETWORK_LIB_INCDIR  = "/usr/local/include/raknet"
    INPUT_LIB_INCDIR    = "/usr/local/include/OIS"
    PHYSIC_LIB_INCDIR   = { "/usr/local/include/LinearMath", "/usr/local/include/BulletCollision", "/usr/local/include/BulletDynamics" }
    AUDIO_LIB_INCDIR    = "/usr/local/include/fmodex"
elseif (windows) then
    GRAPHIC_LIB_INCDIR  = "../../libs/ogre/inc"
    GUI_LIB_INCDIR      = "../../libs/mygui/inc"
    NETWORK_LIB_INCDIR  = "../../libs/raknet/inc"
    INPUT_LIB_INCDIR    = "../../libs/ois/inc"
    PHYSIC_LIB_INCDIR   = "../../libs/bullet/inc"
    AUDIO_LIB_INCDIR    = "../../libs/fmod/inc"
end

--- 32bit Library paths
if (linux) then
    GRAPHIC_LIB_BINDIR_32BIT    = "/usr/local/lib"
    INPUT_LIB_BINDIR_32BIT      = "/usr/local/lib"
    PHYSIC_LIB_BINDIR_32BIT     = "/usr/local/lib"
    AUDIO_LIB_BINDIR_32BIT      = "/usr/local/lib"
elseif (windows) then
    GRAPHIC_LIB_BINDIR_32BIT    = "../../libs/ogre/lib"
    INPUT_LIB_BINDIR_32BIT      = "../../libs/ois/lib"
    PHYSIC_LIB_BINDIR_32BIT     = "../../libs/bullet/lib"
    AUDIO_LIB_BINDIR_32BIT      = "../../libs/fmod/lib"
end

--- 64bit Library paths
GRAPHIC_LIB_BINDIR_64BIT        = "."
INPUT_LIB_BINDIR_64BIT          = "."
PHYSIC_LIB_BINDIR_64BIT         = "."
AUDIO_LIB_BINDIR_64BIT          = "."

--- Library names
if (linux) then
    GRAPHIC_LIB_NAME_DEBUG      = "OgreMain"
    INPUT_LIB_NAME_DEBUG        = "OIS"
    GUI_LIB_NAME_DEBUG          = "mygui"
    NETWORK_LIB_NAME_DEBUG      = "raknet"
    PHYSIC_LIB_NAME_DEBUG       = { "BulletCollision", "LinearMath" }
    AUDIO_LIB_NAME_DEBUG        = "fmodex"

    GRAPHIC_LIB_NAME            = "OgreMain"
    INPUT_LIB_NAME              = "OIS"
    GUI_LIB_NAME                = "mygui"
    NETWORK_LIB_NAME            = "raknet"
    PHYSIC_LIB_NAME             = { "BulletCollision", "LinearMath" }
    AUDIO_LIB_NAME              = "fmodex"
elseif (windows) then
    GRAPHIC_LIB_NAME_DEBUG      = "OgreMain_d"
    GUI_LIB_NAME_DEBUG          = "MyGUI_d"
    NETWORK_LIB_NAME_DEBUG      = "RakNetLibStaticDebug"
    PHYSIC_LIB_NAME_DEBUG       = { "libbulletcollision_d", "libbulletdynamics_d", "libbulletmath_d" }
    AUDIO_LIB_NAME_DEBUG        = "fmodex_vc"
    INPUT_LIB_NAME_DEBUG        = "OIS_d"
    
    GRAPHIC_LIB_NAME            = "OgreMain"
    GUI_LIB_NAME                = "MyGUI"
    NETWORK_LIB_NAME            = "RakNetLibStatic"
    PHYSIC_LIB_NAME             = { "libbulletcollision", "libbulletdynamics", "libbulletmath" }
    AUDIO_LIB_NAME              = "fmodex_vc"
    INPUT_LIB_NAME              = "OIS"
end

--- Generate proper paths for RakNet based on our OS
if (windows) then
    NETWORK_LIB_BINDIR_32BIT    = "../../libs/raknet/lib/win32"
    NETWORK_LIB_BINDIR_64BIT    = "../../libs/raknet/lib/win32"
elseif (linux) then
    NETWORK_LIB_BINDIR_32BIT    = "../../libs/raknet/lib/linux/32"
    NETWORK_LIB_BINDIR_64BIT    = "../../libs/raknet/lib/linux/64"
end

-- Foundation: Generate proper include paths depending upon what is enabled from above
if (linux) then
    incpathExternalLibs = { "/usr/local/include/python2.5",
                            "../../libs/boost/inc", 
                            "/usr/local/include/ET",
                            "../../libs" }
elseif (windows) then
    incpathExternalLibs = { "../../libs/python/inc",
                            "../../libs/python/inc/stackless",
                            "../../libs/boost/inc",
                            "../../libs/etl/inc",
                            "../../libs/btogre/inc",
                            "../../libs" }
end

-- FoundationPython: Generate proper include paths depending upon what is enabled from above
if (linux) then
    LIBDIR_FOUNDATION_DEBUG = { 
                            "../../libs", 
                            PHYSIC_LIB_BINDIR_32BIT,
                            AUDIO_LIB_BINDIR_32BIT }
    LIBDIR_FOUNDATION_RELEASE = { 
                            "../../libs", 
                            PHYSIC_LIB_BINDIR_32BIT,
                            AUDIO_LIB_BINDIR_32BIT }
                            
    LIBDIR_FOUNDATIONPYTHON_DEBUG = { 
                            "../../bin/debug",
                            "/usr/local/lib/python2.5",
                            GRAPHIC_LIB_BINDIR_32BIT,
                            PHYSIC_LIB_BINDIR_32BIT,
                            AUDIO_LIB_BINDIR_32BIT }
    LIBDIR_FOUNDATIONPYTHON_RELEASE = { 
                            "../../bin/release",
                            "/usr/local/lib/python2.5",
                            GRAPHIC_LIB_BINDIR_32BIT,
                            PHYSIC_LIB_BINDIR_32BIT,
                            AUDIO_LIB_BINDIR_32BIT }
elseif (windows) then
    LIBDIR_FOUNDATION_DEBUG = { 
                            "../../libs", 
                            PHYSIC_LIB_BINDIR_32BIT,
                            AUDIO_LIB_BINDIR_32BIT }
    LIBDIR_FOUNDATION_RELEASE = { 
                            "../../libs", 
                            PHYSIC_LIB_BINDIR_32BIT,
                            AUDIO_LIB_BINDIR_32BIT }

    LIBDIR_FOUNDATIONPYTHON_DEBUG = { 
                            "../../bin/debug",
                            "../../libs/python/lib",
                            "../../libs/ogre/lib",
                            "../../libs/bullet/lib",
                            "../../libs/mygui/lib/",
                            "../../libs/raknet/Lib",
                            "../../libs/ois/lib",
                            "../../libs/etl/lib",
                            "../../libs/btogre/lib",
                            PHYSIC_LIB_BINDIR_32BIT,
                            AUDIO_LIB_BINDIR_32BIT }
    LIBDIR_FOUNDATIONPYTHON_RELEASE = { 
                            "../../bin/release",
                            "../../libs/python/lib",
                            "../../libs/ogre/lib",
                            "../../libs/bullet/lib",
                            "../../libs/mygui/lib/",
                            "../../libs/raknet/lib",
                            "../../libs/ois/lib",
                            "../../libs/etl/lib",
                            "../../libs/btogre/lib",
                            PHYSIC_LIB_BINDIR_32BIT,
                            AUDIO_LIB_BINDIR_32BIT }
end

if (ENABLE_GRAPHIC) then
    table.insert(incpathExternalLibs, GRAPHIC_LIB_INCDIR)
    table.insert(incpathExternalLibs, GUI_LIB_INCDIR)
end
if (ENABLE_NETWORK) then
    table.insert(incpathExternalLibs, NETWORK_LIB_INCDIR)
end
if (ENABLE_INPUT) then
    table.insert(incpathExternalLibs, INPUT_LIB_INCDIR)
end
if (ENABLE_PHYSIC) then
    table.insert(incpathExternalLibs, PHYSIC_LIB_INCDIR)
end
if (ENABLE_AUDIO) then
    table.insert(incpathExternalLibs, AUDIO_LIB_INCDIR)
end

-- Do not edit below this
incpathFoundation = "../../code/foundation"
 
if (windows) then
    if (target == "vs2003") then
        project.path        = "./msvc_v7"
        libfileBoost_DEBUG  = "boost_python-vc71-mt-gd-1_36"
        libfileBoost        = "boost_python-vc71-mt-1_36"
    elseif (target == "vs2005") then
        project.path        = "./msvc_v8"
        libfileBoost_DEBUG  = "boost_python-vc80-mt-gd-1_36"
        libfileBoost        = "boost_python-vc80-mt-1_36"
    elseif (target == "vs2008") then
        project.path        = "./msvc_v9"
        libfileBoost_DEBUG  = "boost_python-vc90-mt-gd-1_36"
        libfileBoost        = "boost_python-vc90-mt-1_36"
    end
    
    libfileRaknet_DEBUG = "RakNetLibStaticDebug"
    libfileRaknet       = "RakNetLibStatic"
    libfilePython       = "python26"
    libfileETL_DEBUG    = "etl_d"
    libfileETL          = "etl"
    libfileBtOgre_DEBUG = "btogre_d"
    libfileBtOgre       = "btogre"
    
    libfileAllDebug = { libfileRaknet_DEBUG, libfileETL_DEBUG, libfileBtOgre_DEBUG, libfilePython }
    libfileAllRelease = { libfilePython, libfileETL, libfileBtOgre, libfilePython }
elseif (linux) then
    project.path = "./gnuc"
    libfilePython = "python2.5"
    libfileUUID = "uuid"
    libfileTerrain = "EditableTerrain";

    libfileAllDebug = { libfilePython, libfileUUID, libfileTerrain }
    libfileAllRelease = { libfilePython, libfileUUID, libfileTerrain }
end

if (ENABLE_GRAPHIC) then
    table.insert(libfileAllDebug, GRAPHIC_LIB_NAME_DEBUG)
    table.insert(libfileAllRelease, GRAPHIC_LIB_NAME)
    table.insert(libfileAllDebug, GUI_LIB_NAME_DEBUG)
    table.insert(libfileAllRelease, GUI_LIB_NAME)
end
if (ENABLE_NETWORK) then
    table.insert(libfileAllDebug, NETWORK_LIB_NAME_DEBUG)
    table.insert(libfileAllRelease, NETWORK_LIB_NAME)
end
if (ENABLE_INPUT) then
    table.insert(libfileAllDebug, INPUT_LIB_NAME_DEBUG)
    table.insert(libfileAllRelease, INPUT_LIB_NAME)
end
if (ENABLE_PHYSIC) then
    table.insert(libfileAllDebug, PHYSIC_LIB_NAME_DEBUG)
    table.insert(libfileAllRelease, PHYSIC_LIB_NAME)
end
if (ENABLE_AUDIO) then
    table.insert(libfileAllDebug, AUDIO_LIB_NAME_DEBUG)
    table.insert(libfileAllRelease, AUDIO_LIB_NAME)
end

project.name = "Foundation"
project.configs = { "Debug", "Release" }
project.config["Debug"].bindir = "../bin/debug"
project.config["Release"].bindir = "../bin/release"
project.config["Debug"].libdir = "../bin/debug"
project.config["Release"].libdir = "../bin/release"

-- Python Interface
package = newpackage()
    package.name = "FoundationPython"
    package.path = project.path
    package.kind = "dll"
    package.targetprefix = ""
    package.language = "c++"
    package.includepaths = { incpathFoundation, "../../code/python", incpathExternalLibs }

    package.files = {
        matchrecursive( "../../code/python/*.cpp", "../../code/python/*h",
                        "../../libs/boost/src/*.cpp"
                      )
    }

    if (windows) then
        package.targetextension                 = "pyd"
        package.defines                         = { "WIN32" }
        package.links                           = { "ws2_32" }
        
        package.config["Debug"].buildoptions    = { "/D_CRT_SECURE_NO_DEPRECATE" } 
        package.config["Release"].buildoptions  = { "/D_CRT_SECURE_NO_DEPRECATE", "/MP" }
        
        package.config["Debug"].linkoptions     = { "/NODEFAULTLIB:LIBCMTD" }
        package.config["Release"].linkoptions   = { "/NODEFAULTLIB:LIBCMT" }
    elseif (linux) then
        package.targetextension                 = "so"
        package.defines                         = { "LINUX" }
        package.links                           = { "pthread" }
        package.config["Debug"].buildoptions    = { "-fno-threadsafe-statics -Wall" }
    elseif (macosx) then
        package.defines                         = { "MACOSX" }
    end

    package.config["Debug"].libpaths        = { LIBDIR_FOUNDATIONPYTHON_DEBUG }
    package.config["Debug"].defines         = { "_DEBUG" }
    package.config["Debug"].links           = { "Foundation", libfileAllDebug }
    package.config["Debug"].target          = "FoundationPython"
    package.config["Debug"].objdir          = "../objs/debug/foundationPython"

    package.config["Release"].libpaths      = { LIBDIR_FOUNDATIONPYTHON_RELEASE }
    package.config["Release"].links         = { "Foundation", libfileAllRelease }
    package.config["Release"].target        = "FoundationPython"
    package.config["Release"].objdir        = "../objs/release/foundationPython"
    package.config["Release"].buildflags    = { "optimize", "no-frame-pointer" }

	if (windows) then
		table.insert(package.config["Release"].postbuildcommands, "..\\tools\\upx\\upx.exe ..\\..\\bin\\release\\FoundationPython.pyd")
	end

    --- Generate defines based on the enable options at the top
    if (ENABLE_GRAPHIC) then
        table.insert(package.defines, "S_ENABLE_GRAPHIC")
    end
    if (ENABLE_NETWORK) then
        table.insert(package.defines, "S_ENABLE_NETWORK")
    end
    if (ENABLE_INPUT) then
        table.insert(package.defines, "S_ENABLE_INPUT")
    end
    if (ENABLE_PHYSIC) then
        table.insert(package.defines, "S_ENABLE_PHYSIC")
    end
    if (ENABLE_AUDIO) then
        table.insert(package.defines, "S_ENABLE_AUDIO")
    end

-- Our true engine, handles scenegraphs, graphics, audio, networking, input, etc.
package = newpackage()
    package.name = "Foundation"
    package.path = project.path
    package.kind = "lib"
    package.language = "c++"
    package.includepaths = { incpathFoundation, incpathExternalLibs }
    
    package.files = {
        matchrecursive("../../code/foundation/*.cpp", "../../code/foundation/*.h", "../../code/foundation/*.hpp")
    }
  
    if (windows) then
        package.defines                         = { "WIN32" }
        package.config["Debug"].buildoptions    = { "/D_CRT_SECURE_NO_DEPRECATE" } 
        package.config["Release"].buildoptions  = { "/D_CRT_SECURE_NO_DEPRECATE", "/MP" }
    elseif (linux) then
        package.defines                         = { "LINUX" }
        package.config["Debug"].links           = { "pthread", libfileAllDebug }
        package.config["Release"].links         = { "pthread", libfileAllRelease }
        package.config["Debug"].buildoptions    = { "-fno-threadsafe-statics -Wall" }
    elseif (macosx) then
        package.defines                         = { "MACOSX" }
    end

    package.config["Debug"].libpaths =      { LIBDIR_FOUNDATION_DEBUG }
    package.config["Debug"].defines =       { "_DEBUG", "BOOST_ALL_NO_LIB" }
    package.config["Debug"].target =        "foundationd"
    package.config["Debug"].objdir =        "../objs/debug/foundation"

    if (windows) then
        package.config["Debug"].postbuildcommands = {
            "mkdir ..\\..\\bin\\debug\\yaml",
            "mkdir ..\\..\\bin\\debug\\psyco",
            
            "copy ..\\..\\libs\\ogre\\bin\\debug\\*.dll ..\\..\\bin\\debug",
            "copy ..\\..\\libs\\python\\bin\\python26.dll ..\\..\\bin\\debug",
            "copy ..\\..\\libs\\python\\bin\\python.exe ..\\..\\bin\\debug",
            "copy ..\\..\\libs\\fmod\\bin\\*.dll ..\\..\\bin\\debug",
            "copy ..\\..\\libs\\mygui\\bin\\Debug\\*.dll ..\\..\\bin\\debug",
            "copy ..\\..\\bin\\common\\* ..\\..\\bin\\debug",
            "copy ..\\..\\bin\\common\\yaml\\*.* ..\\..\\bin\\debug\\yaml",
            "copy ..\\..\\bin\\common\\psyco\\*.* ..\\..\\bin\\debug\\psyco",
            "copy ..\\..\\bin\\common\\plugins\\plugins_win_debug.cfg ..\\..\\bin\\debug\\plugins_win.cfg",
        }
    end
    
    package.config["Release"].libpaths =    { LIBDIR_FOUNDATION_RELEASE }
    package.config["Release"].defines =     { "BOOST_ALL_NO_LIB" }
    package.config["Release"].target =      "foundation"
    package.config["Release"].objdir =      "../objs/release/foundation"
    package.config["Release"].buildflags =  { "optimize", "no-frame-pointer" }

    if (windows) then
        package.config["Release"].postbuildcommands = {
            "mkdir ..\\..\\bin\\release\\yaml",
            "mkdir ..\\..\\bin\\release\\psyco",
            
            "copy ..\\..\\libs\\ogre\\bin\\release\\*.dll ..\\..\\bin\\release",
            "copy ..\\..\\libs\\python\\bin\\python26.dll ..\\..\\bin\\release",
            "copy ..\\..\\libs\\python\\bin\\python.exe ..\\..\\bin\\release",
            "copy ..\\..\\libs\\fmod\\bin\\*.dll ..\\..\\bin\\release",
            "copy ..\\..\\libs\\mygui\\bin\\Release\\*.dll ..\\..\\bin\\release",
            "copy ..\\..\\bin\\common\\* ..\\..\\bin\\release",
            "copy ..\\..\\bin\\common\\yaml\\*.* ..\\..\\bin\\release\\yaml",
            "copy ..\\..\\bin\\common\\psyco\\*.* ..\\..\\bin\\release\\psyco",
            "copy ..\\..\\bin\\common\\plugins\\plugins_win.cfg ..\\..\\bin\\release\\plugins_win.cfg",
        }

        table.insert(package.config["Release"].postbuildcommands, "..\\tools\\upx\\upx.exe ..\\..\\bin\\release\\*.dll")
        table.insert(package.config["Release"].postbuildcommands, "..\\tools\\upx\\upx.exe ..\\..\\bin\\release\\*.exe")
    end


    --- Generate defines based on the enable options at the top
    if (ENABLE_GRAPHIC) then
        print "> GraphicSystem ENABLED"
        table.insert(package.defines, "S_ENABLE_GRAPHIC")
    else
        print "> GraphicSystem DISABLED"
    end
    if (ENABLE_NETWORK) then
        print "> NetworkSystem ENABLED"
        table.insert(package.defines, "S_ENABLE_NETWORK")
    else
        print "> NetworkSystem DISABLED"
    end
    if (ENABLE_INPUT) then
        print "> InputSystem ENABLED"
        table.insert(package.defines, "S_ENABLE_INPUT")
    else
        print "> InputSystem DISABLED"
    end
    if (ENABLE_PHYSIC) then
        print "> PhysicSystem ENABLED"
        table.insert(package.defines, "S_ENABLE_PHYSIC")
    else
        print "> PhysicSystem DISABLED"
    end
    if (ENABLE_AUDIO) then
        print "> AudioSystem ENABLED"
        table.insert(package.defines, "S_ENABLE_AUDIO")
    else
        print "> AudioSystem DISABLED"
    end

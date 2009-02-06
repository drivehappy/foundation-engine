// S_ENABLE_AUDIO is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_AUDIO)

#include "audioSystem.h"

using namespace Foundation;
using namespace Foundation::Audio;


AudioManager::AudioManager()
{
    m_pSystem = NULL;

    taskUpdate = new Task(this, AudioManager::doTaskUpdate_Wrapper);
    //taskUpdate->_this = this;
    //taskUpdate->_functionPointer = AudioManager::doTaskUpdate_Wrapper;
}

AudioManager::~AudioManager()
{
    destroy();
}


AudioManager& AudioManager::getSingleton()
{
    static AudioManager m_uSingleton;
    return m_uSingleton;
}

bool AudioManager::initialize()
{
    m_nResult = FMOD::System_Create(&m_pSystem);
    if (m_nResult != FMOD_OK) {
        f_printf(" - [AudioManager] Error creating FMOD system: %s\n", FMOD_ErrorString(m_nResult));
        return false;
    }

#if FOUNDATION_PLATFORM == FOUNDATION_PLATFORM_LINUX
    m_nResult = m_pSystem->setOutput(FMOD_OUTPUTTYPE_ALSA);
    if (m_nResult != FMOD_OK) {
        f_printf(" - [AudioManager] Error setting output type: %s\n", FMOD_ErrorString(m_nResult));
        return false;
    }
#endif

    m_nResult = m_pSystem->init(100, FMOD_INIT_NORMAL, 0);
    if (m_nResult != FMOD_OK) {
        f_printf(" - [AudioManager] Error initializing FMOD system: %s\n", FMOD_ErrorString(m_nResult));
        return false;
    }

    return true;
}

void AudioManager::destroy()
{
    if (m_pSystem) {
        m_nResult = m_pSystem->release();
        m_pSystem = NULL;
        if (m_nResult != FMOD_OK) {
            f_printf("[AudioManager] Error releasing FMOD system: %s\n", FMOD_ErrorString(m_nResult));
            return;
        }
    } else {
        //f_printf("[AudioManager] Error: The FMOD system has not been initialized.\n");
    }
}

Sound* AudioManager::createSound(const char *_sFilename, const bool _bEnableLoop)
{
    Sound *pSound = NULL;

    if (m_pSystem) {
        m_nResult = m_pSystem->createSound(_sFilename, FMOD_DEFAULT, 0, (FMOD::Sound **)&pSound);
        if (m_nResult != FMOD_OK) {
            f_printf("[AudioManager] Error creating sound (%s): %s.\n", _sFilename, FMOD_ErrorString(m_nResult));
            return NULL;
        }
        if (_bEnableLoop)
            pSound->setMode(FMOD_LOOP_NORMAL);
    } else {
        f_printf("[AudioManager] Error: The FMOD system has not been initialized.\n");
    }

    return pSound;
}

Foundation::Audio::Channel* AudioManager::playSound(Sound *_pSound)
{
    if (m_pSystem) {
        FMOD::Channel *pChannel;
        m_nResult = m_pSystem->playSound(FMOD_CHANNEL_FREE, (FMOD::Sound *)_pSound, false, &pChannel);
        if (m_nResult != FMOD_OK) {
            f_printf("[AudioManager] Error playing sound (%s).\n", FMOD_ErrorString(m_nResult));
            return (Foundation::Audio::Channel*)pChannel;
        } else {
            m_pChannelList.push_back((Channel *)pChannel);
        }
        
    } else {
        f_printf("[AudioManager] Error: The FMOD system has not been initialized.\n");
    }

    return NULL;
}

void AudioManager::stopSound(Audio::Channel *_pChannel)
{
    _pChannel->stop();
}

void AudioManager::stopAllSounds()
{
    vector<Channel *>::iterator itrChannel;

    for (itrChannel = m_pChannelList.begin(); itrChannel != m_pChannelList.end(); itrChannel++) {
        (*itrChannel)->stop();
    }

    m_pChannelList.clear();
}

int AudioManager::getDriverCount()
{
    if (m_pSystem) {
        int nDriverCount = 0;
        m_nResult = m_pSystem->getNumDrivers(&nDriverCount);
        if (m_nResult == FMOD_OK) {
            return nDriverCount;
        } else {
            f_printf("[AudioManager] Error: Could not retrieve driver count (%i).\n", m_nResult);
        }
    } else {
        f_printf("[AudioManager] Error: The FMOD system has not been initialized.\n");
    }

    return -1;
}

char* AudioManager::getDriverInfo(int _nId)
{
    char *sDriverName = new char[FOUNDATION_MAXSTRINGLEN];

    if (m_pSystem) {
        m_nResult = m_pSystem->getDriverInfo(_nId, sDriverName, FOUNDATION_MAXSTRINGLEN, NULL);
        if (m_nResult == FMOD_OK) {
            return sDriverName;
        } else {
            f_printf("[AudioManager] Error: Could not retrieve driver info (%i).\n", m_nResult);
        }
    } else {
        f_printf("[AudioManager] Error: The FMOD system has not been initialized.\n");
    }

    return NULL;
}

void* AudioManager::doTaskUpdate(void *_pArgs)
{
    float nDeltaTime = *(float *)_pArgs;
    (void) nDeltaTime;

    if (m_pSystem) {
        m_nResult = m_pSystem->update();
        if (m_nResult != FMOD_OK) {
            f_printf("[AudioManager] Error: Could not update FMOD system (%i).\n", m_nResult);
        }
    } else {
        f_printf("[AudioManager] Error: The FMOD system has not been initialized.\n");
    }

    return NULL;
}

void AudioManager::setDriver(int _nId)
{
    int nPreviousDriver;
    
    if (m_pSystem) {
        m_nResult = m_pSystem->getDriver(&nPreviousDriver);
        m_nResult = m_pSystem->setDriver(_nId);
        if (m_nResult != FMOD_OK) {
            f_printf("[AudioManager] Error: Could not set driver (%i), using default.\n", m_nResult);
            m_nResult = m_pSystem->setDriver(nPreviousDriver);
            if (m_nResult != FMOD_OK) {
                f_printf("[AudioManager] Error: Failed to use default driver, we have a big problem here (no sound device?).\n", m_nResult);
            }
        }
    } else {
        f_printf("[AudioManager] Error: The FMOD system has not been initialized.\n");
    }
}

#endif  // S_ENABLE_AUDIO

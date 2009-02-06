// S_ENABLE_AUDIO is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_AUDIO)

#include "py_audioSystem.h"

using namespace Foundation;
using namespace Foundation::Audio;


// Audio
AudioManager_PythonWrapper::AudioManager_PythonWrapper()
{
}

AudioManager_PythonWrapper::~AudioManager_PythonWrapper()
{
}

bool AudioManager_PythonWrapper::initialize()
{
    return AudioManager::getSingleton().initialize();
}

void AudioManager_PythonWrapper::destroy()
{
    AudioManager::getSingleton().destroy();
}

Task *AudioManager_PythonWrapper::getTaskUpdate() const
{
    return AudioManager::getSingleton().getTaskUpdate();
}

Sound_PythonWrapper* AudioManager_PythonWrapper::createSound(const char *_sFilename, const bool _bEnableLoop)
{
    return (Sound_PythonWrapper *)AudioManager::getSingleton().createSound(_sFilename, _bEnableLoop);
}

Foundation::Audio::Channel_PythonWrapper* AudioManager_PythonWrapper::playSound(Sound_PythonWrapper *_pSound)
{
    return (Foundation::Audio::Channel_PythonWrapper*)AudioManager::getSingleton().playSound((Sound *)_pSound);
}

void AudioManager_PythonWrapper::stopSound(Audio::Channel *_pChannel)
{
    AudioManager::getSingleton().stopSound(_pChannel);
}

void AudioManager_PythonWrapper::stopAllSounds()
{
    AudioManager::getSingleton().stopAllSounds();
}

int AudioManager_PythonWrapper::getDriverCount()
{
    return AudioManager::getSingleton().getDriverCount();
}

str AudioManager_PythonWrapper::getDriverInfo(int _nId)
{
    char *sAudioDeviceName = AudioManager::getSingleton().getDriverInfo(_nId);
    return str(sAudioDeviceName, strlen(sAudioDeviceName));
}

void AudioManager_PythonWrapper::setDriver(int _nId)
{
    AudioManager::getSingleton().setDriver(_nId);
}

#endif  // S_ENABLE_AUDIO

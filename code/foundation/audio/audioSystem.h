// S_ENABLE_AUDIO is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_AUDIO)

#if !defined(__F_AUDIOSYSTEM_H_)
#define __F_AUDIOSYSTEM_H_

// Foundation
#include "kernel/kernel.h"

// FMOD
#include "fmod.hpp"
#include "fmod_errors.h"


namespace Foundation 
{
    namespace Audio 
    {
        /** Wrapper to expose the sound class.
         */
        class Sound : public FMOD::Sound
        {
        };

        /** Wrapper to expose the channel class.
         */
        class Channel : public FMOD::Channel
        {
        };


        /** Class provides a clean interface to different underlying audio systems.
            @note
                If adding a new audio library, that libraries interface should be
                exposed <b>only</b> inside the AudioManager.
            @par
                Maintenence:
                If more functionality is needed for the engine the AudioManager's
                interface should be updated and inherited around any underlying new functions
                within a library.
         */
        class AudioManager
        {
        public:
            /** Constructor.
             */
            AudioManager();
            /** Destructor, implicitly calls destroy().
             */
            ~AudioManager();

            /** Returns a singleton.
             */
            static AudioManager& getSingleton();

            /** Initialize the AudioManager.
             */
            bool initialize();

            /** Destroy the AudioManager.
             */
            void destroy();

            // Tasks
            Task *taskUpdate;
            Task *getTaskUpdate() const {
                return taskUpdate;
            }
    
            /** A task wrapper for the receiving task.
                @param
                    The network client that does the receiving.
                @param
                    Arguments to the task.
            */
            inline static void doTaskUpdate_Wrapper(void* _class, void *_args) {
                reinterpret_cast<AudioManager*>(_class)->doTaskUpdate(_args);
            };

            /** Create a sound from file.
             */
            Sound* createSound(const char *_sFilename, const bool _bEnableLoop);

            /** Play a sound object.
             */
            Channel* playSound(Sound *_pSound);

            /** Stop a single sound channel.
             */
            void stopSound(Channel *_pChannel);

            /** Stop all sound channels.
             */
            void stopAllSounds();


            /** Get the number of sound drivers (devices).
                @return
                    The number of sound devices.
             */
            int getDriverCount();

            /** Get the driver (device) name for a specific ID.
                @param
                    The ID of the driver to retrieve. Between 0 and getDriverCount().
                @return
                    The device name.
             */
            char* getDriverInfo(int _nId);

            /** Set the driver index to use.
                @param
                    The ID of the driver to use.
             */
            void setDriver(int _nId);

        private:
            FMOD_RESULT             m_nResult;
            FMOD::System            *m_pSystem;
            vector<Channel *>       m_pChannelList;

            /** Task function for receiving data across the network.
                @param
                    Arguments to the task.
                @return
                    TODO
            */
            void* doTaskUpdate(void*);
        };
    };
};
    

#endif  // __F_AUDIOSYSTEM_H_

#endif  // S_ENABLE_AUDIO

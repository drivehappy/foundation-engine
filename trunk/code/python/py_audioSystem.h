// S_ENABLE_AUDIO is defined in premake.lua, it should only be altered there
// to allow for easy stripping/adding of libraries actually needed to be compiled
#if defined(S_ENABLE_AUDIO)

#if !defined(_PY__F_AUDIOSYSTEM_H_)
#define _PY__F_AUDIOSYSTEM_H_

// Boost
#include <boost/python.hpp>
using namespace boost::python;

// Foundation
#include "foundation.h"

#include "audio/audioSystem.h"


namespace Foundation
{
    namespace Audio
    {
        /**
         */
        class Sound_PythonWrapper : Sound
        {
        };

        /**
         */
        class Channel_PythonWrapper : Channel
        {
        };

        /**
         */
        class AudioManager_PythonWrapper : AudioManager
        {
        public:
            /**
             */
            AudioManager_PythonWrapper();
            /**
             */
            ~AudioManager_PythonWrapper();

            /**
             */
            bool initialize();

            /**
             */
            void destroy();

            /** Return our wrapped task.
             */
            Task *getTaskUpdate() const;

            /**
             */
            Sound_PythonWrapper* createSound(const char *_sFilename, const bool _bEnableLoop);

            /**
             */
            Foundation::Audio::Channel_PythonWrapper* playSound(Sound_PythonWrapper *_pSound);

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
            str getDriverInfo(int _nId);

            /** Set the driver index to use.
                @param
                    The ID of the driver to use.
             */
            void setDriver(int _nId);
            
        private:

        };
    };
};

#endif  // _PY__F_AUDIOSYSTEM_H_

#endif  // S_ENABLE_AUDIO

#if !defined(__F_COMMUNICATOR_H_)
#define __F_COMMUNICATOR_H_

// Foundation
#include "memoryManager.h"
#include "stringTable.h"

// STL
#include <vector>
using std::vector;
#include <map>
using std::map;


namespace Foundation {

    /** Base class to provide communicator channel functionality.
     */
    class Channel {
        /** Struct to provide event functions.
            @remarks
            Callback system for when a channel receives a message.
         */ 
        struct EventFunction {
            void (*_functionPointer)(void *_pThis, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize);
            void *_this;
            unsigned long _nChannelID;
        };

        /** Struct to provide message.
            @remarks
            Message for system consumption.
         */
        struct Message {
            unsigned int    _size;
            unsigned short  _header;
            char            *_message;

            Message()                   { _message = NULL; _size = 0; }
            Message(unsigned int nSize) { _message = new char[nSize]; _size = nSize; }
            ~Message()                  { if (_message) { delete[] _message; _size = 0; } }
        };

        /** Class to provide Communicator.
        @remarks
        Singleton class to manage message passing, layers on top of the replayer.
         */
        class Communicator {
            public:
                /** Get the singleton class.
                */
                static Communicator& getSingleton();

                /** Join a channel.
                        @param
                        Channel name.
                        @param
                        Event function.
                        @note
                        Join a specified channel, will create a channel if it doesn't exist.
                */
                static void joinChannel(const char *_pcChannelName, EventFunction *_pEventFunction);

                /** Leave a channel.
                        @param
                        Channel name.
                        @param
                        Event function.
                        @note
                        Leave a specified channel, event function is required in case multiple
                        functions register to the same channel.
                */
                static void leaveChannel(const char *_pcChannelName, EventFunction *_pEventFunction);

                /** Send to a channel.
                        @param
                        Channel name.
                        @param
                        Message to send.
                        @param
                        Event function.
                        @note
                        Internally makes a copy of message, callee can delete safely, any called 
                        function must not write to the space or delete it.
                */
                static void sendChannel(const char *_pcChannelName, Message *_message, EventFunction *_evtFunction);

            protected:
                Communicator();
                Communicator(const Communicator&);
                Communicator& operator=(const Communicator&);
                ~Communicator();

            private:
                static map< unsigned long, vector<EventFunction*> > channelList;
        };

        public:
            /** Default constructor
            */
            Channel();
            /** Destructor
            */
            ~Channel();

            /** Create a new message
                @param
                    Size in bytes of the message.
                @note
                    If a previously allocated message exists it will be deleted.
            */
            void Channel_MessageCreate(unsigned int _nSize);

            /** Destroy an existing message
            */
            void Channel_MessageDestroy();
            
            /** Set the message header
            */
            void Channel_MessageHeader(unsigned short _nHeader);
            
            /** Set the message data
                @param
                    Pointer to the data.
                @param
                    Size of the data in bytes.
                
                @remarks
                    A warning will occur if the size of the data to be written is greater
                    than the allocated message size.
            */
            void Channel_MessageData(const char *_pData, unsigned int _nSize);

            /** Join a new or existing channel
                @param
                    The channel name to join.
                @param
                    The function that is used to receive messages on.
            */
            void Channel_Join(const char *_pChannelName, void (*_pFunction)(void *_pThis, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize));
            
            /** Send the message across a channel
                @param
                    Then channel to send the message on.
            */
            void Channel_Send(const char *_pChannelName);
            
            /** Leave a channel
                @param
                    The channel name to leave.
                @param
                    The function specified when joining the channel.

                @remarks
                    The function is needed here because multiple functions in each class
                    are allowed in the same channel, it would be impossible to determine
                    explicitly if the API user wants all functions to leave the channel.
            */
            void Channel_Leave(const char *_pChannelName, void (*_pFunction)(void *_pThis, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize));

            // Testing, put back into priv
            //Message                 *m_pChannelMessage;

        private:
            void                    *m_pChannelReceive(void *);
            Message                 *m_pChannelMessage;
            unsigned int            m_nChannelMessageIndex;
            vector<EventFunction *> m_pChannelEvent;
    };

};

#endif  // __F_COMMUNICATOR_H_


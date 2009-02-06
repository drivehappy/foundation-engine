#if !defined(__F_CLIENT_H_)
#define __F_CLIENT_H_

// Foundation
#include "kernel/kernel.h"
#include "network/messages.h"

// RakNet
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "RakNetStatistics.h"
using namespace RakNet;

#define RECV_BUFFER_SIZE 16384


namespace Foundation {

    namespace Network {

        /** Class that provides the network client implementation.
        */
        class Client : public Channel {
        public:
            Client();
            ~Client();
    
            /** Connect to a server.
                @param
                    The host to connect to.
                @param
                    The port to connect on.
            */
            void connect(const char *_sHostname, unsigned int _nPort);
    
            /** Close the connection. NOT CURRENTLY IMPLEMENTED.
                @param
                    TODO.
            */
            inline void close(SystemAddress addr) {
                // TODO
            };
    
            /** Send data to the server.
                @param
                    Pointer to the data to be sent.
                @param
                    Length of the data to be sent.
            */
            void send(const char *_data, unsigned int _length);
    
            /** Get the local port that we are connected on.
             */
            unsigned short getLocalPort() {
                return mClient->GetInternalID().port;
            };

            /** Get RakNet statistics.
             */
            double getBitsPerSecond();
            
            // Tasks
            Task *taskReceive;
            Task const* getTaskReceive() const {
                return taskReceive;
            }
    
            /** A task wrapper for the receiving task.
                @param
                    The network client that does the receiving.
                @param
                    Arguments to the task.
            */
            inline static void doTaskReceive_Wrapper(void* _class, void *_args) {
                reinterpret_cast<Client*>(_class)->doTaskReceive(_args);
            };
    
            /** Channel event function for receiving data.
                @param
                    The network client that does the receiving.
                @param
                    Arguments to the channel.
            */
            inline static void doChannelReceive_Wrapper(void *_class, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize) {
                reinterpret_cast<Client*>(_class)->doChannelReceive(_nChannelID, _nHeader, _pData, _nSize);
            };
    
        private:
            /** Task function for receiving data across the network.
                @param
                    Arguments to the task.
                @return
                    TODO
            */
            void* doTaskReceive(void*);
    
            /** Task function for receiving data internally from a channel.
                @param
                    Arguments to the task.
                @return
                    TODO
            */
            void* doChannelReceive(unsigned long _nChannelID, unsigned short _nHeader, void *_pData, unsigned int _nSize);
    
            /** Function to operate on packets received.
                @param
                    Pointer to a valid RakNet packet.
            */
            void packetReceive(Packet *_packet);
    
            // Client members
            RakPeerInterface    *mClient;
        };

    };

};

#endif  // __F_CLIENT_H_

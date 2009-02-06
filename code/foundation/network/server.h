#if! defined(__F_SERVER_H_)
#define __F_SERVER_H_

// Foundation
#include "kernel/kernel.h"

// STL
#include <vector>
using std::vector;
#include <map>
using std::map;

// RakNet
#include "RakNetStatistics.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
using namespace RakNet;


// Defines match those in messages.h
#define MSG_NEWCONN 0x0002	// Player connection added
#define MSG_DISCONN 0x0003	// Player connection removed

#define RECV_BUFFER_SIZE 16384


namespace Foundation {

    namespace Network {

        /** Struct to hold server side client information.
        */
        struct ClientInfo {
            ClientInfo()
            {
                m_pSystemAddress = new SystemAddress();
            };
    
            void setAddress(const char *_pData)
            {
                memcpy(m_pSystemAddress, _pData, 6);
            };

            unsigned long getAddress()
            {
                return *(unsigned long *)(m_pSystemAddress);
            };

            bool operator==(const ClientInfo *_RHS)
            {
                int nRetVal = memcmp(m_pSystemAddress, _RHS->m_pSystemAddress, 6);
                if (nRetVal == 0)
                    return true;
                return false;
            };

            // Stats
            unsigned long   m_nTotalRecvData;
            unsigned long   m_nTotalSentData;
    
            // Track who our client is
            SystemAddress   *m_pSystemAddress;
        };
    
        /** Class that provides the network server implementation.
        */
        class Server : public Channel {
        public:
            Server();
            ~Server();
    
            /** Listen for incoming connections.
                @param
                    Port to listen on.
                @param
                    Maximum number of connections.
            */
            void listen(unsigned int Port, unsigned int MaxConnections);
    
            /** Close a specific client connection.
                @param
                    The client address to close.
            */
            inline void close(SystemAddress addr) {
                mServer->CloseConnection(addr, true, 0);
            };

            /** Close all client connections.
             */
            inline void shutdown() {
                mServer->Shutdown(1);
            };

            /** Send data to a client.
                @param
                    The client address to send to.
                @param
                    Data to be sent.
                @param
                    Length of the data in bytes.
            */
            void send(SystemAddress _addr, const char *_pData, unsigned int _nLength);

            /** Send data to a client.
                @param
                    The client info struct to send to.
                @param
                    Data to be sent.
                @param
                    Length of the data in bytes.
            */
            inline void send(ClientInfo _client, const char *_pData, unsigned int _nLength)
            {
                send(*_client.m_pSystemAddress, _pData, _nLength);
            }

            /** Get RakNet statistics.
             */
            double getBitsPerSecond();

            /** Get the number of connections.
             */
            unsigned int getClientCount();
    
            /** TODO - IS THIS STILL IMPLEMENTED?
                @return
                    TODO
            */
            vector<Packet*> pullPackets();
    
            // Tasks
            Task *taskListen;
            Task *getTaskListen() const {
                return taskListen;
            }
    
            /** A task wrapper for the listening task.
                @param
                    The network server that does the listening.
                @param
                    Arguments to the task.
            */
            inline static void doTaskListen_Wrapper(void *_class, void *_args) {
                reinterpret_cast<Server*>(_class)->doTaskListen(_args);
            };
    
            /** Channel event function for receiving data.
                @param
                    The network server that does the receiving.
                @param
                    Arguments to the channel.
            */
            inline static void doChannelReceive_Wrapper(void *_class, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize) {
                reinterpret_cast<Server*>(_class)->doChannelReceive(_nChannelID, _nHeader, _pData, _nSize);
            };
    
        private:
            /** Task function for listening for new connections.
                @param
                    Arguments to the task.
                @return
                    TODO
            */
            void* doTaskListen(void *);
            
            /** Channel for receiving data.
                @param
                    Arguments to the task.
                @return
                    TODO
            */
            void* doChannelReceive(unsigned long _nChannelID, unsigned short _nHeader, void *_pData, unsigned int _nSize);
    
            /** Add a new client to the network server.
                @param
                    Packet data containing the client information.
            */
            void addClient(Packet *_packet);
            
            /** Received a packet.
                @param
                    Packet data containing the data.
            */
            void packetReceive(Packet *_packet);
    
            // Server members
            RakPeerInterface        *mServer;
            SystemAddress           noconnAddr;
            vector<ClientInfo*>     *clientList;
            unsigned int            totalClients;
        };

    };

};

#endif  // __F_SERVER_H_

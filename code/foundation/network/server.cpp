#include "server.h"
using namespace Foundation;
using namespace Foundation::Network;

Server::Server()
{
    // Task inits
    taskListen = new Task(this, Server::doTaskListen_Wrapper);
    //taskListen->_this = this;
    //taskListen->_functionPointer = Server::doTaskListen_Wrapper;

    // Raknet inits
    noconnAddr.binaryAddress = 0xffffffff;
    noconnAddr.port = 0xffff;

    //tcpServer = new TCPInterface();
    mServer = RakNetworkFactory::GetRakPeerInterface();
    clientList = new vector<ClientInfo*>();

    // Communicator channel
    Channel_Join("Network_Server_Data", Server::doChannelReceive_Wrapper);
    Channel_Join("Network_Server_OnConnected", Server::doChannelReceive_Wrapper);
    Channel_Join("Network_Server_OnDisconnected", Server::doChannelReceive_Wrapper);
}

Server::~Server()
{
    Channel_Leave("Network_Server_Data", Server::doChannelReceive_Wrapper);
    Channel_Leave("Network_Server_OnConnected", Server::doChannelReceive_Wrapper);
    Channel_Leave("Network_Server_OnDisconnected", Server::doChannelReceive_Wrapper);

    RakNetworkFactory::DestroyRakPeerInterface(mServer);

    vector<ClientInfo*>::iterator itr;
    for (itr = clientList->begin(); itr != clientList->end(); itr++) {
        delete (*itr)->m_pSystemAddress;
        delete (*itr);
    }
    clientList->clear();
    delete clientList;
}

void Server::listen(unsigned int _port, unsigned int _maxConnections)
{
    SocketDescriptor sdTemp = SocketDescriptor(_port, NULL);
    mServer->Startup(_maxConnections, 10, &sdTemp, 1);
    mServer->SetMaximumIncomingConnections(_maxConnections);
}

void Server::send(SystemAddress _addr, const char *_data, unsigned int _length)
{
    // Create a temp buffer, push data down to make room for size header
    //This should be looked at again, could use optimization
    unsigned char *newData = new unsigned char[_length + 1];
    newData[0] = (unsigned char)ID_USER_PACKET_ENUM;
    memcpy(&newData[1], _data, _length);
    
    // Unicast
    if (_addr.binaryAddress != 0x0) {
        mServer->Send((const char*)newData, _length + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, _addr, false);
        //f_printf("Server sent %i bytes of data to player %s\n", _length+2, _addr.ToString());
    }
    // Multicast
    else {
        vector<ClientInfo*>::iterator itr;
        for (itr = clientList->begin(); itr != clientList->end(); itr++) {
            mServer->Send((const char*)newData, _length + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, *((*itr)->m_pSystemAddress), false);
            //f_printf("Broadcast to %s\n", (*((*itr)->systemAddress)).ToString());
        }
        //f_printf("Server broadcasted %i bytes of data to %i players\n", _length+2, clientList->size());
    }

    delete newData;
    //totalSentData += _length;
}

// Channel message handling
void* Server::doChannelReceive(unsigned long _nChannelID, unsigned short _nHeader, void *_pData, unsigned int _nSize)
{
    SystemAddress addrPlayer;
    _nSize = _nSize - 6 + 2;

    // Create our player's system address
    memcpy(&addrPlayer, _pData, 6);
    char *data = new char[_nSize];
    
    memcpy(data, &_nHeader, 2);
    memcpy(data + 2, (char *)_pData + 6, _nSize - 2);

    send(addrPlayer, data, _nSize);
    delete data;

    return 0;
}

// Scheduler task
void* Server::doTaskListen(void *_null) 
{
    Packet *packet = mServer->Receive();

    while (packet) {
        switch (packet->data[0]) {
            case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                f_printf("Another client has disconnected.\n");

                Channel_MessageCreate(6);
                Channel_MessageHeader(MSG_DISCONN);
                Channel_MessageData((char *)&packet->systemAddress, 6);
                Channel_Send("Network_Server_OnDisconnected");
                break;
            case ID_REMOTE_CONNECTION_LOST:
                f_printf("Another client has lost the connection.\n");

                Channel_MessageCreate(6);
                Channel_MessageHeader(MSG_DISCONN);
                Channel_MessageData((char *)&packet->systemAddress, 6);
                Channel_Send("Network_Server_OnDisconnected");
                break;
            case ID_REMOTE_NEW_INCOMING_CONNECTION:
                f_printf("Another client has connected.\n");
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                f_printf("Our connection request has been accepted.\n");
                break;
            case ID_NEW_INCOMING_CONNECTION:
                //f_printf("New connection: %s\n", packet->systemAddress.ToString());
                addClient(packet);
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                f_printf("The server is full.\n");
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                f_printf("A client has disconnected: %s\n", packet->systemAddress.ToString());

                Channel_MessageCreate(6);
                Channel_MessageHeader(MSG_DISCONN);
                Channel_MessageData((char *)&packet->systemAddress, 6);
                Channel_Send("Network_Server_OnDisconnected");
                break;
            case ID_CONNECTION_LOST:
                f_printf("A client lost the connection.\n");

                Channel_MessageCreate(6);
                Channel_MessageHeader(MSG_DISCONN);
                Channel_MessageData((char *)&packet->systemAddress, 6);
                Channel_Send("Network_Server_OnDisconnected");
                break;
            case ID_USER_PACKET_ENUM:
                packetReceive(packet);
                break;
            default:
                f_printf("Unknown header: 0x%x\n", packet->data[0]);
                break;
        }

        mServer->DeallocatePacket(packet);
        packet = mServer->Receive();
    }
    
    return 0;
}

void Server::addClient(Packet *_packet)
{
    Channel_MessageCreate(6);

    Channel_MessageHeader(MSG_NEWCONN);
    Channel_MessageData((char *)&_packet->systemAddress, 6);
    Channel_Send("Network_Server_OnConnected");

    // Add client to our list
    ClientInfo *newClient = new ClientInfo();
    memcpy(newClient->m_pSystemAddress, &_packet->systemAddress, sizeof(SystemAddress));
    clientList->push_back(newClient);
}

void Server::packetReceive(Packet *_packet)
{
    unsigned short  nPacketSize;
    
    nPacketSize = (_packet->bitSize / 8) - 3;
    //f_printf(" Packet size = %i, data size = %i\n", _packet->bitSize, nPacketSize);

    // Create message for our gameManager, send it across our communicator channel Network_Data
    Channel_MessageCreate(6 + nPacketSize);
    unsigned short nHeader;
    memcpy(&nHeader, &_packet->data[1], 2);

    Channel_MessageHeader(nHeader);
    Channel_MessageData((char *)(&_packet->systemAddress), 6);
    Channel_MessageData((char *)(&_packet->data[3]), nPacketSize);
    Channel_Send("Network_Server_Data");
}

double Server::getBitsPerSecond()
{
    double nBitsPerSecondTotal = 0.0f;

    for (unsigned int x = 0; x < mServer->NumberOfConnections(); x++) {
        nBitsPerSecondTotal += mServer->GetStatistics(mServer->GetSystemAddressFromIndex(x))->bitsPerSecond;
    }

    return nBitsPerSecondTotal;
}

unsigned int Server::getClientCount()
{
    return mServer->NumberOfConnections();
}

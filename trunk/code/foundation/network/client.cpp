#include "client.h"
using namespace Foundation;
using namespace Foundation::Network;

Client::Client()
{
    // Task inits
    taskReceive = new Task(this, Client::doTaskReceive_Wrapper);
    //taskReceive->_this = this;
    //taskReceive->_functionPointer = Client::doTaskReceive_Wrapper;

    mClient = RakNetworkFactory::GetRakPeerInterface();

    // Communicator channel
    Channel_Join("Network_Client_Data", doChannelReceive_Wrapper);
    Channel_Join("Network_Client_OnConnected", doChannelReceive_Wrapper);
    Channel_Join("Network_Client_OnDisconnected", doChannelReceive_Wrapper);
}

Client::~Client()
{
    Channel_Leave("Network_Client_Data", doChannelReceive_Wrapper);
    Channel_Leave("Network_Client_OnConnected", doChannelReceive_Wrapper);
    Channel_Leave("Network_Client_OnDisconnected", doChannelReceive_Wrapper);

    if (mClient->IsConnected(UNASSIGNED_SYSTEM_ADDRESS))
        mClient->CloseConnection(UNASSIGNED_SYSTEM_ADDRESS, true);

    RakNetworkFactory::DestroyRakPeerInterface(mClient);
}

void Client::connect(const char *_sHostname, unsigned int _nPort)
{
    SocketDescriptor sdTemp = SocketDescriptor(NULL, NULL);
    mClient->Startup(1, 10, &sdTemp, 1);
    mClient->Connect(_sHostname, _nPort, NULL, 0);
}

void Client::send(const char* _data, unsigned int _length) 
{
    unsigned char *newData = new unsigned char[_length + 1];
    newData[0] = (unsigned char)ID_USER_PACKET_ENUM;
    memcpy(&newData[1], _data, _length);
    
    mClient->Send((const char*)newData, _length + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, mClient->GetSystemAddressFromIndex(0), false);
    delete newData;
    
    
    //totalSentData += _length;
}

// Channel message handling
void* Client::doChannelReceive(unsigned long _nChannelID, unsigned short _nHeader, void *_pData, unsigned int _nSize)
{
    // placeholder

    return NULL;
}

void* Client::doTaskReceive(void *_null) 
{
    Packet *packet = mClient->Receive();

    while (packet) {
        switch (packet->data[0]) {
            case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                f_printf("Another client has disconnected.\n");
                break;
            case ID_REMOTE_CONNECTION_LOST:
                f_printf("Another client has lost the connection.\n");
                break;
            case ID_REMOTE_NEW_INCOMING_CONNECTION:
                f_printf("Another client has connected.\n");
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                //f_printf("Connected to server.\n");

                Channel_MessageCreate(1);
                Channel_MessageHeader(MSG_CONNECT);
                Channel_Send("Network_Client_OnConnected");
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                f_printf("Our connection request has failed.\n");
                break;
            case ID_NEW_INCOMING_CONNECTION:
                f_printf("A connection is incoming.\n");
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                f_printf("The server is full.\n");
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                f_printf("A client has disconnected.\n");

                Channel_MessageCreate(1);
                Channel_MessageHeader(MSG_DISCONNECT);
                Channel_Send("Network_Client_OnDisconnected");
                break;
            case ID_CONNECTION_LOST:
                f_printf("A client lost the connection.\n");

                Channel_MessageCreate(1);
                Channel_MessageHeader(MSG_DISCONNECT);
                Channel_Send("Network_Client_OnDisconnected");
                break;
            case ID_ALREADY_CONNECTED:
                f_printf("Already conencted.\n");
                break;
            case ID_USER_PACKET_ENUM:
                packetReceive(packet);
                break;
            case ID_OPEN_CONNECTION_REPLY:
                f_printf("Open connection reply\n");
                break;
            default:
                f_printf("Unknown header: %i (0x%x)\n", packet->data[0],  packet->data[0]);
                break;
        }

        mClient->DeallocatePacket(packet);
        packet = mClient->Receive();
    }

    return 0;
}

void Client::packetReceive(Packet *_packet)
{
    unsigned short nPacketSize;
    unsigned short nHeader, nPort;

    nPacketSize = (_packet->bitSize / 8) - 3;

    // Create message for our gameManager, send it across our communicator channel Network_Data
    Channel_MessageCreate(8 + nPacketSize);

    memcpy(&nHeader, &_packet->data[1], 2);
    nPort = getLocalPort();

    Channel_MessageHeader(nHeader);
    //Channel_MessageData((char *)&nPort, 2);
    Channel_MessageData((char *)&_packet->systemAddress, 6);
    Channel_MessageData((char *)&_packet->data[3], nPacketSize);
    Channel_Send("Network_Client_Data");
}

double Client::getBitsPerSecond()
{
    RakNetStatistics *stats = mClient->GetStatistics(mClient->GetSystemAddressFromIndex(0));

//    return stats->bitsPerSecond;
    return 0;
}

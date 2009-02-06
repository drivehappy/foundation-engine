#include "communicator.h"

using namespace Foundation;

// Static defs
map< unsigned long, vector<Channel::EventFunction*> > Channel::Communicator::channelList;


Channel::Communicator::Communicator()
{
}

Channel::Communicator::~Communicator()
{
}


Channel::Communicator& Channel::Communicator::getSingleton()
{
    static Communicator m_pSingleton;
    return m_pSingleton;
}

void Channel::Communicator::joinChannel(const char *_pcChannelName, EventFunction *_pEventFunction)
{
#ifdef FOUNDATION_DEBUG
    // Check string for length conditions
    if (strlen(_pcChannelName) > FOUNDATION_MAXSTRINGLEN) {
        f_error("Channel name length is greater than supported, rename it.");
        return;
    }
#endif

    unsigned long nChannelID = StringTable::getSingleton().getStringID((const char*)_pcChannelName);
    channelList[nChannelID].push_back(_pEventFunction);

#ifdef FOUNDATION_DEBUG
    f_printf("Object joined channel (callback: %p, this: %p) (String: %s, StringID: %lx)\n", _pEventFunction->_functionPointer, _pEventFunction->_this, _pcChannelName, nChannelID);
#endif
}

void Channel::Communicator::leaveChannel(const char* _pcChannelName, EventFunction* _pEventFunction)
{
    unsigned long nChannelID = StringTable::getSingleton().getStringID(_pcChannelName);
    map< unsigned long, vector<EventFunction*> >::iterator itr;
    vector<EventFunction*>::iterator itrFunctions;

#ifdef FOUNDATION_DEBUG
    f_printf("Object left channel (callback: %p, this: %p) (String: %s, StringID: %lx)\n", _pEventFunction->_functionPointer, _pEventFunction->_this, _pcChannelName, nChannelID);
#endif

    for (itr = channelList.begin(); itr != channelList.end(); itr++) {
        if ((*itr).first == nChannelID) {
            for (itrFunctions = (*itr).second.begin(); itrFunctions != (*itr).second.end(); itrFunctions++) {
                if ((*itrFunctions)->_this == _pEventFunction->_this && (*itrFunctions)->_functionPointer == _pEventFunction->_functionPointer) {
                    (*itr).second.erase(itrFunctions);
                    if ((*itr).second.size() == 0)
                        channelList.erase(itr);
                    return ;
                }
            }
        }
    }
}

void Channel::Communicator::sendChannel(const char *_pcChannelName, Message *_message, EventFunction *_evtFunction) 
{
    Message *pMessageCopy = new Message(_message->_size);
    unsigned long nChannelID = StringTable::getSingleton().getStringID(_pcChannelName);
    map< unsigned long, vector<EventFunction*> >::iterator itr;
    vector<EventFunction*>::iterator itrFunctions;

    memcpy(pMessageCopy->_message, _message->_message, _message->_size);
    pMessageCopy->_header = _message->_header;

    for (itrFunctions = channelList[nChannelID].begin(); itrFunctions != channelList[nChannelID].end(); itrFunctions++) {
        if ((*itrFunctions)->_this != _evtFunction->_this) {
            if ((*itrFunctions)->_functionPointer) {
                (*itrFunctions)->_functionPointer((*itrFunctions)->_this, nChannelID, pMessageCopy->_header, pMessageCopy->_message, pMessageCopy->_size);
            }
        }
    }

    delete pMessageCopy;
}

Channel::Channel()
{
    m_pChannelMessage = NULL;
    m_nChannelMessageIndex = 0;
}

Channel::~Channel()
{
    Channel_MessageDestroy();

    // Cannot leave any open channels implicitly, because the communicator would be unaware,
    //  report it to the application programmer as an error
#ifdef FOUNDATION_DEBUG
    if (m_pChannelEvent.size()) {
        f_printf("Error: %i channel event functions were left open.\n", m_pChannelEvent.size());
    }
#endif
}

void Channel::Channel_MessageCreate(unsigned int _nSize)
{
    Channel_MessageDestroy();
    //f_printf("Creating new message size = %i\n", _nSize);
    
    m_pChannelMessage = new Message(_nSize);
    m_pChannelMessage->_header = 0xC0DE;
    m_nChannelMessageIndex = 0;
    
    //f_printf("(0) %p %p %i\n", &m_pChannelMessage, m_pChannelMessage, m_pChannelMessage->_size);
}

void Channel::Channel_MessageDestroy()
{
    if (m_pChannelMessage)
        delete m_pChannelMessage;
    m_nChannelMessageIndex = 0;
    m_pChannelMessage = NULL;
}

void Channel::Channel_MessageHeader(unsigned short _nHeader)
{
    if (m_pChannelMessage) {
        m_pChannelMessage->_header = _nHeader;
        //f_printf("(1) %p %p %i\n", &m_pChannelMessage, m_pChannelMessage, m_pChannelMessage->_size);
    } else {
        f_printf("Warning: Attempting to set message header on invalid message.\n");
    }
}

void Channel::Channel_MessageData(const char *_pData, unsigned int _nSize)
{
    if (m_pChannelMessage) {
        if (_nSize > (m_pChannelMessage->_size - m_nChannelMessageIndex)) {
            f_printf("Message size is too large (new data size: %i) (channel message size: %i) (channel message index: %i)\n", _nSize, m_pChannelMessage->_size, m_nChannelMessageIndex);
#ifdef FOUNDATION_DEBUG
            f_breakpoint();
#endif
        }

        memcpy(&m_pChannelMessage->_message[m_nChannelMessageIndex], _pData, _nSize);
        m_nChannelMessageIndex += _nSize;
        
        //f_printf("(2) Message size = %i\n", m_pChannelMessage->_size);
    } else {
        f_printf("Warning: Attempting to set message data on invalid message.\n");
    }
}

void Channel::Channel_Join(const char *_pChannelName, void (*_pFunction)(void *_pThis, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize))
{
    EventFunction *newEvent = new EventFunction;
    newEvent->_this = this;
    newEvent->_functionPointer = _pFunction;
    newEvent->_nChannelID = StringTable::getSingleton().getStringID(_pChannelName);
    m_pChannelEvent.push_back(newEvent);

    Communicator::getSingleton().joinChannel(_pChannelName, newEvent);
}

void Channel::Channel_Send(const char *_pChannelName)
{
    if (!m_pChannelMessage) {
        f_printf("Error: Channel message is not allocated.\n");
#ifdef FOUNDATION_DEBUG
        f_breakpoint();
#endif
        return;
    }

    vector<EventFunction *>::iterator itrChannel;
    unsigned long nChannelID = StringTable::getSingleton().getStringID(_pChannelName);

    for (itrChannel = m_pChannelEvent.begin(); itrChannel != m_pChannelEvent.end(); itrChannel++) {
        if ((*itrChannel)->_nChannelID == nChannelID) {
            Communicator::getSingleton().sendChannel(_pChannelName, m_pChannelMessage, (*itrChannel));
        }
    }
}

void Channel::Channel_Leave(const char *_pChannelName, void (*_pFunction)(void *_pThis, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize))
{
    vector<EventFunction *>::iterator itrChannel;
    unsigned long nChannelID = StringTable::getSingleton().getStringID(_pChannelName);

    for (itrChannel = m_pChannelEvent.begin(); itrChannel != m_pChannelEvent.end(); itrChannel++) {
        if ((*itrChannel)->_functionPointer == _pFunction) {
            if ((*itrChannel)->_nChannelID == nChannelID) {
                Communicator::getSingleton().leaveChannel(_pChannelName, (*itrChannel));
                m_pChannelEvent.erase(itrChannel);
                break;
            }
        }
    }
}

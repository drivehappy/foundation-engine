#include "py_communicator.h"
using namespace Foundation;

Channel_PythonWrapper::Channel_PythonWrapper()
{
}

Channel_PythonWrapper::~Channel_PythonWrapper()
{
}

void Channel_PythonWrapper::Channel_Join(const char *_pChannelName, PyObject *_pFunction)
{
    unsigned long nChannelID = StringTable::getSingleton().getStringID((const char*)_pChannelName);

    Channel::Channel_Join(_pChannelName, Channel_PythonWrapper::doChannelReceive_Wrapper);
    m_pChannelEvent_GlobalFunction[nChannelID].push_back(_pFunction);
}

void Channel_PythonWrapper::Channel_Leave(const char *_pChannelName, PyObject *_pFunction)
{
    unsigned long nChannelID = StringTable::getSingleton().getStringID((const char*)_pChannelName);
    map< unsigned long, vector<PyObject*> >::iterator itr;
    vector<PyObject*>::iterator itrFunctions;

    Channel::Channel_Leave(_pChannelName, Channel_PythonWrapper::doChannelReceive_Wrapper);

    for (itr = m_pChannelEvent_GlobalFunction.begin(); itr != m_pChannelEvent_GlobalFunction.end(); itr++) {
        if ((*itr).first == nChannelID) {
            for (itrFunctions = (*itr).second.begin(); itrFunctions != (*itr).second.end(); itrFunctions++) {
                if ((*itrFunctions) == _pFunction) {
                    (*itr).second.erase(itrFunctions);
                    if ((*itr).second.size() == 0)
                        m_pChannelEvent_GlobalFunction.erase(itr);
                    return ;
                }
            }
        }
    }
}

void Channel_PythonWrapper::Channel_Join(const char *_pChannelName, PyObject *_pClass, const char *_sMethodName)
{
    unsigned long nChannelID = StringTable::getSingleton().getStringID((const char*)_pChannelName);
    char *sMethodName = new char[FOUNDATION_MAXSTRINGLEN];

    memcpy(sMethodName, _sMethodName, FOUNDATION_MAXSTRINGLEN);
    Channel::Channel_Join(_pChannelName, Channel_PythonWrapper::doChannelReceive_Wrapper);
    m_pChannelEvent_MemberFunction[nChannelID][_pClass].push_back((char*)sMethodName);
}

void Channel_PythonWrapper::Channel_Leave(const char *_pChannelName, PyObject *_pClass, const char *_sMethodName)
{
    unsigned long nChannelID = StringTable::getSingleton().getStringID((const char*)_pChannelName);
    map< unsigned long, map<PyObject*, vector<char*> > >::iterator itrChannel;
    map<PyObject*, vector<char*> >::iterator itrClass;
    vector<char*>::iterator itrMethod;

    Channel::Channel_Leave(_pChannelName, Channel_PythonWrapper::doChannelReceive_Wrapper);

    for (itrChannel = m_pChannelEvent_MemberFunction.begin(); itrChannel != m_pChannelEvent_MemberFunction.end(); itrChannel++) {
        if ((*itrChannel).first == nChannelID) {
            for (itrClass = (*itrChannel).second.begin(); itrClass != (*itrChannel).second.end(); itrClass++) {
                if ((*itrClass).first == _pClass) {
                    for (itrMethod = (*itrClass).second.begin(); itrMethod != (*itrClass).second.end(); itrMethod++) {
                        if (!strcmp((*itrMethod), _sMethodName)) {
                            delete [] (*itrMethod);
                            (*itrClass).second.erase(itrMethod);
                            if ((*itrClass).second.size() == 0)
                                (*itrChannel).second.erase(itrClass);
                            if ((*itrChannel).second.size() == 0)
                                m_pChannelEvent_MemberFunction.erase(itrChannel);
                            return;
                        }
                    }
                }
            }
        }
    }
}

void* Channel_PythonWrapper::doChannelReceive(unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize)
{
    map< unsigned long, vector<PyObject*> >::iterator itrMemberChannel;
    vector<PyObject*>::iterator itrFunctions;
    map< unsigned long, map<PyObject*, vector<char*> > >::iterator itrChannel;
    map<PyObject*, vector<char*> >::iterator itrClass;
    vector<char*>::iterator itrMethod;
    char *sMethodName = new char[FOUNDATION_MAXSTRINGLEN];

    // Search through our global function list
    for (itrMemberChannel = m_pChannelEvent_GlobalFunction.begin(); itrMemberChannel != m_pChannelEvent_GlobalFunction.end(); itrMemberChannel++) {
        if ((*itrMemberChannel).first == _nChannelID) {
            for (itrFunctions = (*itrMemberChannel).second.begin(); itrFunctions != (*itrMemberChannel).second.end(); itrFunctions++) {
                PyObject *f = (*itrFunctions);
                call<void>(f, _nChannelID, _nHeader, str(_pData, _nSize), _nSize);
            }
        }
    }

    // Search through our member function list
    for (itrChannel = m_pChannelEvent_MemberFunction.begin(); itrChannel != m_pChannelEvent_MemberFunction.end(); itrChannel++) {
        if ((*itrChannel).first == _nChannelID) {
            for (itrClass = (*itrChannel).second.begin(); itrClass != (*itrChannel).second.end(); itrClass++) {
                for (itrMethod = (*itrClass).second.begin(); itrMethod != (*itrClass).second.end(); itrMethod++) {
                    memcpy(sMethodName, (*itrMethod), FOUNDATION_MAXSTRINGLEN);
                    PyObject *f = (*itrClass).first;
                    call_method<void>(f, sMethodName, _nChannelID, _nHeader, str(_pData, _nSize), _nSize);
                }
            }
        }
    }

    delete sMethodName;
    return NULL;
}

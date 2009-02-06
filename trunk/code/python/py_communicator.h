#if !defined(_PY__F_COMMUNICATOR_H_)
#define _PY__F_COMMUNICATOR_H_

// Boost
#include <boost/python.hpp>
using namespace boost::python;

// Foundation
#include "foundation.h"

namespace Foundation {

    class Channel_PythonWrapper : public Channel {
    public:
        /** Default constructor.
        */
        Channel_PythonWrapper();
        /** Destructor.
        */
        ~Channel_PythonWrapper();

        /** Join a new or existing channel.
            @param
                The channel name to join.
            @param
                An exposed python function to use as a callback.
        */
        void Channel_Join(const char *_pChannelName, PyObject *_pFunction);
        /** Leave an existing channel.
            @param
                The channel name to leave.
            @param
                An exposed python function to use as a callback.
        */
        void Channel_Leave(const char *_pChannelName, PyObject *_pFunction);

        /** Join a new or existing channel.
            @param
                The channel name to join.
            @param
                An exposed python class to use as a callback.
            @param
                The method name to call in instance of _pClass.
        */
        void Channel_Join(const char *_pChannelName, PyObject *_pClass, const char *_sMethodName);
        /** Leave an existing channel.
            @param
                The channel name to leave.
            @param
                An exposed python function to use as a callback.
            @param
                The method name to call in instance of _pClass.
        */
        void Channel_Leave(const char *_pChannelName, PyObject *pClass, const char *_sMethodName);

        /** Channel event function for receiving data.
            @param
                The channel python wrapper that does the receiving.
            @param
                Arguments to the channel.
        */
        inline static void doChannelReceive_Wrapper(void *_class, unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize) {
            reinterpret_cast<Channel_PythonWrapper*>(_class)->doChannelReceive(_nChannelID, _nHeader, _pData, _nSize);
        };
        

    private:
        void* doChannelReceive(unsigned long _nChannelID, unsigned short _nHeader, char *_pData, unsigned int _nSize);

        map< unsigned long, vector<PyObject*> >                 m_pChannelEvent_GlobalFunction;
        map< unsigned long, map<PyObject*, vector<char*> > >    m_pChannelEvent_MemberFunction;
    };

};

#endif  //_PY__F_COMMUNICATOR_H_

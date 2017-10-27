#pragma once
#include "Recv.h"
#include "Definitions.h"
#include "IClientNetworkable.h"

class ClientClass;

typedef IClientNetworkable*   (*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable*   (*CreateEventFn)();

class ClientClass
{
public:
    CreateClientClassFn      m_pCreateFn;
    CreateEventFn            m_pCreateEventFn;
    char*                    m_pNetworkName;
    RecvTable*               m_pRecvTable;
    ClientClass*             m_pNext;
    EClassIds                m_ClassID;
};
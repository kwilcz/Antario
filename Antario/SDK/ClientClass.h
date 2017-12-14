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
    CreateClientClassFn      pCreateFn;
    CreateEventFn            pCreateEventFn;
    char*                    pNetworkName;
    RecvTable*               pRecvTable;
    ClientClass*             pNext;
    EClassIds                ClassID;
};
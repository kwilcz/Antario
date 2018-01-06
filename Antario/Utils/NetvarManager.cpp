#include "NetvarManager.h"
#include "..\SDK\IBaseClientDll.h"

#undef GetProp  // fucnik recvtable and windoze usin the same namings :angery:

std::unique_ptr<NetvarTree> g_pNetvars;

/**
* NetvarTree - Constructor
NetvarTree
* Call populate_nodes on every RecvTable under client->GetAllClasses()
*/
NetvarTree::NetvarTree()
{
    const ClientClass* clientClass = g_pClientDll->GetAllClasses();

    while (clientClass != nullptr) 
    {
        const auto classInfo = std::make_shared<Node>(0);
        RecvTable* recvTable = clientClass->pRecvTable;

        this->PopulateNodes(recvTable, &classInfo->nodes);
        nodes.emplace(recvTable->GetName(), classInfo);

        clientClass = clientClass->pNext;
    }
}

/**
* PopulateNodes - Populate a node map with brances
* @recvTable:	Table the map corresponds to
* @mapType:	Map pointer
*
* Add info for every prop in the recv table to the node map. If a prop is a
* datatable itself, initiate a recursive call to create more branches.
*/
void NetvarTree::PopulateNodes(RecvTable* recvTable, MapType* mapType)
{
    for (auto i = 0; i < recvTable->GetNumProps(); i++) 
    {
        const RecvProp* prop = recvTable->GetProp(i);
        const auto  propInfo = std::make_shared<Node>(prop->GetOffset());

        if (prop->GetType() == SendPropType::DPT_DataTable)
            this->PopulateNodes(prop->GetDataTable(), &propInfo->nodes);

        mapType->emplace(prop->GetName(), propInfo);
    }
}
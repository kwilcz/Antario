#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "..\SDK\Recv.h"

class NetvarTree 
{
    struct Node;
    using MapType = std::unordered_map<std::string, std::shared_ptr<Node>>;

    struct Node 
    {
        Node(int offset) : offset(offset) {}
        MapType nodes;
        int offset;
    };

    MapType nodes;

public:
    NetvarTree();

private:
    void PopulateNodes(class RecvTable *recv_table, MapType *map);

    /**
    * GetOffsetRecursive - Return the offset of the final node
    * @map:	Node map to scan
    * @acc:	Offset accumulator
    * @name:	Netvar name to search for
    *
    * Get the offset of the last netvar from map and return the sum of it and accum
    */
    static int GetOffsetRecursive(MapType &map, int acc, const char *name)
    {
        return acc + map[name]->offset;
    }

    /**
    * GetOffsetRecursive - Recursively grab an offset from the tree
    * @map:	Node map to scan
    * @acc:	Offset accumulator
    * @name:	Netvar name to search for
    * @args:	Remaining netvar names
    *
    * Perform tail recursion with the nodes of the specified branch of the tree passed for map
    * and the offset of that branch added to acc
    */
    template<typename ...args_t>
    int GetOffsetRecursive(MapType &map, int acc, const char *name, args_t ...args)
    {
        const auto &node = map[name];
        return this->GetOffsetRecursive(node->nodes, acc + node->offset, args...);
    }

public:
    /**
    * GetOffset - Get the offset of a netvar given a list of branch names
    * @name:	Top level datatable name
    * @args:	Remaining netvar names
    *
    * Initiate a recursive search down the branch corresponding to the specified datable name
    */
    template<typename ...args_t>
    int GetOffset(const char *name, args_t ...args)
    {
        const auto &node = nodes[name];
        return this->GetOffsetRecursive(node->nodes, node->offset, args...);
    }
};
extern std::unique_ptr<NetvarTree> g_pNetvars;
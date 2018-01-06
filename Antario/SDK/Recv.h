#pragma once

typedef enum
{
    DPT_Int = 0,
    DPT_Float,
    DPT_Vector,
    DPT_VectorXY,
    DPT_String,
    DPT_Array,    // An array of the base types (can't be of datatables).
    DPT_DataTable,
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
    DPT_Quaternion,
#endif
    DPT_NUMSendPropTypes
} SendPropType;

class RecvProp;
class RecvTable;
class DVariant;

// This is passed into RecvProxy functions.
class CRecvProxyData;

//-----------------------------------------------------------------------------
// pStruct = the base structure of the datatable this variable is in (like C_BaseEntity)
// pOut    = the variable that this this proxy represents (like C_BaseEntity::SomeValue).
//
// Convert the network-standard-type value in Value into your own format in pStruct/pOut.
//-----------------------------------------------------------------------------
typedef void(*RecvVarProxyFn)(const CRecvProxyData *pData, void *pStruct, void *pOut);

// ------------------------------------------------------------------------ //
// ArrayLengthRecvProxies are optionally used to get the length of the 
// incoming array when it changes.
// ------------------------------------------------------------------------ //
typedef void(*ArrayLengthRecvProxyFn)(void *pStruct, int objectID, int currentArrayLength);


// NOTE: DataTable receive proxies work differently than the other proxies.
// pData points at the object + the recv table's offset.
// pOut should be set to the location of the object to unpack the data table into.
// If the parent object just contains the child object, the default proxy just does *pOut = pData.
// If the parent object points at the child object, you need to dereference the pointer here.
// NOTE: don't ever return null from a DataTable receive proxy function. Bad things will happen.
typedef void(*DataTableRecvVarProxyFn)(const RecvProp *pProp, void **pOut, void *pData, int objectID);

class RecvProp
{
    // This info comes from the receive data table.
public:
    RecvProp();

    void                        InitArray(int nElements, int elementStride);

    int                         GetNumElements() const;
    void                        SetNumElements(int nElements);

    int                         GetElementStride() const;
    void                        SetElementStride(int stride);

    int                         GetFlags() const;

    const char*                 GetName() const;
    SendPropType                GetType() const;

    RecvTable*                  GetDataTable() const;
    void                        SetDataTable(RecvTable *pTable);

    RecvVarProxyFn              GetProxyFn() const;
    void                        SetProxyFn(RecvVarProxyFn fn);

    DataTableRecvVarProxyFn     GetDataTableProxyFn() const;
    void                        SetDataTableProxyFn(DataTableRecvVarProxyFn fn);

    int                         GetOffset() const;
    void                        SetOffset(int o);

    // Arrays only.
    RecvProp*                   GetArrayProp() const;
    void                        SetArrayProp(RecvProp *pProp);

    // Arrays only.
    void                        SetArrayLengthProxy(ArrayLengthRecvProxyFn proxy);
    ArrayLengthRecvProxyFn      GetArrayLengthProxy() const;

    bool                        IsInsideArray() const;
    void                        SetInsideArray();

    // Some property types bind more data to the prop in here.
    const void*                 GetExtraData() const;
    void                        SetExtraData(const void *pData);

    // If it's one of the numbered "000", "001", etc properties in an array, then
    // these can be used to get its array property name for debugging.
    const char*                 GetParentArrayPropName();
    void                        SetParentArrayPropName(const char *pArrayPropName);

public:

    const char                  *pVarName;
    SendPropType                RecvType;
    int                         Flags;
    int                         StringBufferSize;


public:

    bool                        bInsideArray;        // Set to true by the engine if this property sits inside an array.

                                                   // Extra data that certain special property types bind to the property here.
    const void *pExtraData;

    // If this is an array (DPT_Array).
    RecvProp                    *pArrayProp;
    ArrayLengthRecvProxyFn      ArrayLengthProxy;

    RecvVarProxyFn              ProxyFn;
    DataTableRecvVarProxyFn     DataTableProxyFn;    // For RDT_DataTable.

    RecvTable                   *pDataTable;        // For RDT_DataTable.
    int                         Offset;

    int                         ElementStride;
    int                         nElements;

    // If it's one of the numbered "000", "001", etc properties in an array, then
    // these can be used to get its array property name for debugging.
    const char                  *pParentArrayPropName;
};

class CRecvDecoder;
class RecvTable
{
public:
 
    typedef RecvProp    PropType;
 
    RecvTable();
    RecvTable(RecvProp *pProps, int nProps, const char *pNetTableName);
    ~RecvTable();
 
    void                Construct(RecvProp *pProps, int nProps, const char *pNetTableName);
 
    int                 GetNumProps();
    RecvProp*           GetProp(int i);
 
    const char*         GetName();
 
    // Used by the engine while initializing array props.
    void                SetInitialized(bool bInitialized);
    bool                IsInitialized() const;
 
    // Used by the engine.
    void                SetInMainList(bool bInList);
    bool                IsInMainList() const;
 
 
public:
 
    // Properties described in a table.
    RecvProp           *pProps;
    int                 nProps;
 
    // The decoder. NOTE: this covers each RecvTable AND all its children (ie: its children
    // will have their own decoders that include props for all their children).
    CRecvDecoder       *pDecoder;
 
    const char         *pNetTableName;    // The name matched between client and server.
 
 
private:
 
    bool                bInitialized;
    bool                bInMainList;
};



inline int RecvTable::GetNumProps()
{
    return this->nProps;
}

inline RecvProp* RecvTable::GetProp(int i)
{
    return &this->pProps[i];
}

inline const char* RecvTable::GetName()
{
    return this->pNetTableName;
}

inline void RecvTable::SetInitialized(bool bInitialized)
{
    this->bInitialized = bInitialized;
}

inline bool RecvTable::IsInitialized() const
{
    return this->bInitialized;
}

inline void RecvTable::SetInMainList(bool bInList)
{
    this->bInMainList = bInList;
}

inline bool RecvTable::IsInMainList() const
{
    return this->bInMainList;
}


inline void RecvProp::InitArray(int nElements, int elementStride)
{
    this->RecvType = DPT_Array;
    this->nElements = nElements;
    this->ElementStride = elementStride;
}

inline int RecvProp::GetNumElements() const
{
    return this->nElements;
}

inline void RecvProp::SetNumElements(int nElements)
{
    this->nElements = nElements;
}

inline int RecvProp::GetElementStride() const
{
    return this->ElementStride;
}

inline void RecvProp::SetElementStride(int stride)
{
    this->ElementStride = stride;
}

inline int RecvProp::GetFlags() const
{
    return this->Flags;
}

inline const char* RecvProp::GetName() const
{
    return this->pVarName;
}

inline SendPropType RecvProp::GetType() const
{
    return this->RecvType;
}

inline RecvTable* RecvProp::GetDataTable() const
{
    return this->pDataTable;
}

inline void RecvProp::SetDataTable(RecvTable *pTable)
{
    this->pDataTable = pTable;
}

inline RecvVarProxyFn RecvProp::GetProxyFn() const
{
    return this->ProxyFn;
}

inline void RecvProp::SetProxyFn(RecvVarProxyFn fn)
{
    this->ProxyFn = fn;
}

inline DataTableRecvVarProxyFn RecvProp::GetDataTableProxyFn() const
{
    return this->DataTableProxyFn;
}

inline void RecvProp::SetDataTableProxyFn(DataTableRecvVarProxyFn fn)
{
    this->DataTableProxyFn = fn;
}

inline int RecvProp::GetOffset() const
{
    return this->Offset;
}

inline void RecvProp::SetOffset(int o)
{
    this->Offset = o;
}

inline RecvProp* RecvProp::GetArrayProp() const
{
    return this->pArrayProp;
}

inline void RecvProp::SetArrayProp(RecvProp *pProp)
{
    this->pArrayProp = pProp;
}

inline void RecvProp::SetArrayLengthProxy(ArrayLengthRecvProxyFn proxy)
{
    this->ArrayLengthProxy = proxy;
}

inline ArrayLengthRecvProxyFn RecvProp::GetArrayLengthProxy() const
{
    return this->ArrayLengthProxy;
}

inline bool RecvProp::IsInsideArray() const
{
    return this->bInsideArray;
}

inline void RecvProp::SetInsideArray()
{
    this->bInsideArray = true;
}

inline const void* RecvProp::GetExtraData() const
{
    return this->pExtraData;
}

inline void RecvProp::SetExtraData(const void *pData)
{
    this->pExtraData = pData;
}

inline const char* RecvProp::GetParentArrayPropName()
{
    return this->pParentArrayPropName;
}

inline void    RecvProp::SetParentArrayPropName(const char *pArrayPropName)
{
    this->pParentArrayPropName = pArrayPropName;
}
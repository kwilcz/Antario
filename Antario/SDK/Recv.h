#pragma once

#include <stdio.h>

enum class SendPropType
{
    DPT_Int = 0,
    DPT_Float,
    DPT_Vector,
    DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
    DPT_String,
    DPT_Array,     // An array of the base types (can't be of datatables).
    DPT_DataTable,
    DPT_Int64,
    DPT_NUMSendPropTypes
};

class RecvProp;
class RecvTable;
class DVariant;

// This is passed into RecvProxy functions.
class CRecvProxyData;

//-----------------------------------------------------------------------------
// pStruct = the base structure of the datatable this variable is in (like C_BaseEntity)
// pOut    = the variable that this this proxy represents (like C_BaseEntity::m_SomeValue).
//
// Convert the network-standard-type value in m_Value into your own format in pStruct/pOut.
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
public:
    RecvProp();

    virtual void					InitArray(int nElements, int elementStride) = 0;

    virtual int						GetNumElements() const = 0;
    virtual void					SetNumElements(int nElements) = 0;

    virtual int						GetElementStride() const= 0;
    virtual void					SetElementStride(int stride)= 0;

    virtual int						GetFlags() const= 0;

    virtual const char*				GetName() const= 0;
    virtual SendPropType			GetType() const= 0;

    virtual RecvTable*				GetDataTable() const= 0;
    virtual void					SetDataTable(RecvTable *pTable)= 0;

    virtual RecvVarProxyFn			GetProxyFn() const= 0;
    virtual void					SetProxyFn(RecvVarProxyFn fn)= 0;

    virtual  DataTableRecvVarProxyFn	GetDataTableProxyFn() const= 0;
    virtual void					SetDataTableProxyFn(DataTableRecvVarProxyFn fn)= 0;

    virtual int						GetOffset() const= 0;
    virtual void					SetOffset(int o)= 0;

    // Arrays only.
    virtual RecvProp*				GetArrayProp() const= 0;
    virtual void					SetArrayProp(RecvProp *pProp)= 0;

    // Arrays only.
    virtual void					SetArrayLengthProxy(ArrayLengthRecvProxyFn proxy)= 0;
    virtual ArrayLengthRecvProxyFn	GetArrayLengthProxy() const= 0;

    virtual bool					IsInsideArray() const= 0;
    virtual void					SetInsideArray()= 0;

    // Some property types bind more data to the prop in here.
    virtual const void*			GetExtraData() const= 0;
    virtual void				SetExtraData(const void *pData)= 0;

    // If it's one of the numbered "000", "001", etc properties in an array, then
    // these can be used to get its array property name for debugging.
    virtual const char*			GetParentArrayPropName()= 0;
    virtual void				SetParentArrayPropName(const char *pArrayPropName)= 0;

public:
    char*                    m_pVarName;
    SendPropType             m_RecvType;
    int                      m_Flags;
    int                      m_StringBufferSize;
    bool                     m_bInsideArray;
    const void*              m_pExtraData;
    RecvProp*                m_pArrayProp;
    void*                    m_ArrayLengthProxy;
    void*                    m_ProxyFn;
    void*                    m_DataTableProxyFn;
    RecvTable*               m_pDataTable;
    int                      m_Offset;
    int                      m_ElementStride;
    int                      m_nElements;
    const char*              m_pParentArrayPropName;
};

class RecvTable
{
public:
    typedef RecvProp	PropType;

    RecvTable();
    RecvTable(RecvProp *pProps, int nProps, const char *pNetTableName);
    ~RecvTable();

    virtual void		Construct(RecvProp *pProps, int nProps, const char *pNetTableName) = 0;

    virtual  int			GetNumProps() = 0;
    virtual RecvProp*	GetProp(int i) = 0;

    virtual const char*	GetName() = 0;

    // Used by the engine while initializing array props.
    virtual  void		SetInitialized(bool bInitialized) = 0;
    virtual bool		IsInitialized() const = 0;

    // Used by the engine.
    virtual void		SetInMainList(bool bInList) = 0;
    virtual bool		IsInMainList() const = 0;
public:
    RecvProp*                m_pProps;
    int                      m_nProps;
    void*                    m_pDecoder;
    char*                    m_pNetTableName;
    bool                     m_bInitialized;
    bool                     m_bInMainList;
};

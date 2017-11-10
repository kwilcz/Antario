#include "Utils\Interfaces.h"
#include "SDK\IClientMode.h"
#include "SDK\CInput.h"
#include <d3d9.h>

class VMTHook;
class Hooks
{
public:   
    // Initialization setup, called on injection
    static void Init(); 
    static void Restore();

public:
    /*---------------------------------------------*/
    /*-------------Hooked functions----------------*/
    /*---------------------------------------------*/

    static bool     __fastcall CreateMove(IClientMode* thisptr, void* edx, float sampleInputFrametime, CUserCmd* cmd);
    //static HRESULT  __stdcall EndScene  (IDirect3DDevice9* pDevice);
    //static HRESULT  __stdcall Reset     (IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
    //static HRESULT  __stdcall Present   (IDirect3DDevice9* pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);

private:
    /*---------------------------------------------*/
    /*-------------VMT Hook pointers---------------*/
    /*---------------------------------------------*/

    std::unique_ptr<VMTHook>    pD3DDevice9Hook;
    std::unique_ptr<VMTHook>    pClientModeHook;

private:
    /*---------------------------------------------*/
    /*-------------Hook prototypes-----------------*/
    /*---------------------------------------------*/

    typedef bool(__thiscall* CreateMove_t)  (IClientMode*, float, CUserCmd*);
    typedef long(__stdcall*  EndScene_t)    (IDirect3DDevice9*);
    typedef long(__stdcall*  Reset_t)       (IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
    typedef long(__stdcall*  Present_t)     (IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
};
extern Hooks g_Hooks;



class VMTHook
{
public:
    VMTHook(void* ppClass)
    {
        this->ppBaseClass = static_cast<std::uintptr_t**>(ppClass);

        // loop through all valid class indexes. When it will hit invalid (not existing) it will end the loop
        while (static_cast<std::uintptr_t*>(*this->ppBaseClass)[this->indexCount])
            ++this->indexCount;

        const std::size_t kSizeTable = this->indexCount * sizeof(std::uintptr_t);

       
        this->pOriginalVMT = *this->ppBaseClass;
        this->pNewVMT = std::make_unique<std::uintptr_t[]>(this->indexCount);
        std::memcpy(this->pNewVMT.get(), this->pOriginalVMT, kSizeTable); // replace original vtable pointer to pointer to our f-ction

        *this->ppBaseClass = this->pNewVMT.get();   
    };
    ~VMTHook() { *this->ppBaseClass = this->pOriginalVMT; };

    template <class Type>
    Type GetOriginal(std::size_t index)
    {
        return reinterpret_cast<Type>(this->pOriginalVMT[index]);
    };

    bool Hook(std::size_t index, void* fnNew)
    {
        if (index > this->indexCount)   // check if given index is valid
            return false;

        this->pNewVMT[index] = reinterpret_cast<std::uintptr_t>(fnNew);
        return true;
    };

    bool Unhook(std::size_t index)
    {
        if (index > this->indexCount)
            return false;

        this->pNewVMT[index] = this->pOriginalVMT[index];
        return true;
    };

private:
    std::unique_ptr<std::uintptr_t[]> pNewVMT = nullptr;    // Actual used pointer of VMT hook
    std::uintptr_t**    ppBaseClass  = nullptr;             // Saved pointer to our VMT hook
    std::uintptr_t*     pOriginalVMT = nullptr;             // Saved original pointer to the VMT
    std::size_t         indexCount = 0;                     // Count of indexes inside out f-ction
};
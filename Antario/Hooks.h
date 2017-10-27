#include "SDK\Interfaces.h"
#include "SDK\CInput.h"
#include <d3d9.h>

class VMTHook;

/*---------------------------------------------*/
/*-------------Hook prototypes-----------------*/
/*---------------------------------------------*/
typedef bool(__thiscall* CreateMove_t)  (IClientMode*, float, CUserCmd*);
typedef long(__stdcall* EndScene_t)     (IDirect3DDevice9* device);
typedef long(__stdcall* Reset_t)        (IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp);
typedef long(__stdcall* Present_t)      (IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);


class Hooks
{
public:    
    static void Init(); // Initialization setup, called on injection

public:
/*---------------------------------------------*/
/*-------------Hooked functions----------------*/
/*---------------------------------------------*/
    static bool     __stdcall CreateMove(float sample_input_frametime, CUserCmd* cmd);
    //static HRESULT  __stdcall EndScene  (IDirect3DDevice9* pDevice);
    //static HRESULT  __stdcall Reset     (IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
    //static HRESULT  __stdcall Present   (IDirect3DDevice9* pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);

private:
    std::unique_ptr<VMTHook> pD3DDevice9Hook;
    std::unique_ptr<VMTHook> pClientModeHook;
};
extern std::unique_ptr<Hooks> g_pHooks;

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
    std::unique_ptr<std::uintptr_t[]> pNewVMT = nullptr; // Actual used pointer of VMT hook
    std::uintptr_t** ppBaseClass  = nullptr;             // Saved pointer to our VMT hook
    std::uintptr_t*  pOriginalVMT = nullptr;             // Saved original pointer to the VMT
    std::size_t		 indexCount   = 0;                   // Count of indexes inside out f-ction
};
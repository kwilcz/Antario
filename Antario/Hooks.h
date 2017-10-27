#ifdef _WIN32
#pragma once
#endif

#include <Windows.h>
#include <memory>

class VMTHook;

class Hooks
{
public:
    Hooks();
    ~Hooks();
    static void Init();

private:
    std::unique_ptr<VMTHook> pD3DDevice9Hook;
    std::unique_ptr<VMTHook> pClientModeHook;
};
extern Hooks hooks;

class VMTHook
{
public:
    VMTHook(std::uintptr_t** ppClass)
    {
        this->ppBaseClass = ppClass;

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
        if (index > this->indexCount && index < 0 )   // check if given index is invalid
            return false;

        this->pNewVMT[index] = reinterpret_cast<std::uintptr_t>(fnNew);
        return true;
    };

    bool Unhook(std::size_t index)
    {
        if (index > this->indexCount && index < 0)
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
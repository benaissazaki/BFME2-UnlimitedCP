// dllmain.cpp : Définit le point d'entrée de l'application DLL.
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        FILE* file;
        fopen_s(&file, "UnlimitedCP.log", "w");

        DWORD oldProtection;

        // The address of the instruction that verifies that CP <= CPMax
        unsigned char* CP_limiter_location = (unsigned char*)0x006A74B9;

        BOOL worked = VirtualProtect((void*)CP_limiter_location, 3, PAGE_EXECUTE_READWRITE, &oldProtection);

        if (file)
        {

            fprintf(file, "%d Before %x \t%x \t%x\n", worked, *CP_limiter_location, *(CP_limiter_location + 1), *(CP_limiter_location + 2));
        }
        *CP_limiter_location = 0x90;    // Replace it with nops
        *(CP_limiter_location + 1) = 0x90;
        *(CP_limiter_location + 2) = 0x90;

        
        if (file)
        {
            fprintf(file, "After %x \t%x \t%x", *CP_limiter_location, *(CP_limiter_location + 1), *(CP_limiter_location + 2));
            fclose(file);
        }
    }

    return TRUE;
}

wchar_t* charArrayToLPCWSTR(LPCSTR charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

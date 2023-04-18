#include <stdio.h>
#include <windows.h>
#include <string>
#include <tchar.h>
wchar_t* charArrayToLPCWSTR(LPCSTR);


int main()
{
    LPCWSTR gamePath = charArrayToLPCWSTR("D:\\Program Files (x86)\\Electronic Arts\\The Battle for Middle-earth (tm) II\\lotrbfme2.exe");
    LPCTSTR lpCommandLine = TEXT("-mod \"D:\\Program Files (x86)\\Electronic Arts\\The Battle for Middle-earth (tm) II\\My Battle for Middle-earth (tm) II Files\\HDEdition.big\"");
    LPCSTR dllPath = "C:\\Users\\GEEKZONE\\source\\repos\\BFME2-UnlimitedCP\\Debug\\DLL Maker.dll";

    // Create a process with the game's executable
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    if (CreateProcess(gamePath, (LPTSTR)lpCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0)
    {
        printf("Failed to launch game: %d\n", GetLastError());
        return 1;
    }

    Sleep(10000);
    // Get a Handle to the game's process
    LPCWSTR windowName = charArrayToLPCWSTR("The Lord of the Rings(tm), The Battle for Middle-earth(tm) II");

    HWND hwnd = FindWindow(NULL, windowName);

    if (hwnd == NULL)
    {
        MessageBox(0, 0, charArrayToLPCWSTR("Game window not found"), 0);
        return 1;
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    HANDLE gameProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

    if (gameProcess == NULL)
    {
        printf("Could not open process: %d\n", GetLastError());
        return 1;
    }

    // Allocate memory for the DLL's path
    LPVOID dllPathAddress = VirtualAllocEx(gameProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (dllPathAddress == NULL)
    {
        printf("Failed to allocate memory: %d\n", GetLastError());
        return 1;
    }

    // Write the DLL's path in memory
    if (WriteProcessMemory(gameProcess, dllPathAddress, dllPath, strlen(dllPath) + 1, NULL) == 0)
    {
        printf("Failed to write to process memory: %d\n", GetLastError());
        return 1;
    }

    // Load kernel32.dll
    HINSTANCE kernel32dll = GetModuleHandle(charArrayToLPCWSTR("kernel32.dll"));
    if (kernel32dll == NULL)
    {
        printf("Failed to get kernel32.dll! %d\n", GetLastError());
        return 1;
    }

    // Get the address to kernel32's LoadLibraryA function
    LPVOID loadLibraryAddress = (LPVOID)GetProcAddress(kernel32dll, "LoadLibraryA");

    // Use LoadLibraryA to load our DLL using dllPathAddress
    if (CreateRemoteThread(gameProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddress, dllPathAddress, 0, NULL) == NULL)
    {
        printf("Failed to create remote thread: %d\n", GetLastError());
        return 1;
    }


    // Close handles and exit
    CloseHandle(gameProcess);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

wchar_t* charArrayToLPCWSTR(LPCSTR charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

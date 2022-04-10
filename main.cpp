#include <iostream>
#include <cstring>
#include <string>
#include <windows.h>
#include <winreg.h>

#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>

LPCTSTR g_subKey = TEXT("Software\\Valve\\Steam");

void killProgramProcess(const char *programmName) {
  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
  PROCESSENTRY32 pEntry;
  pEntry.dwSize = sizeof (pEntry);
  BOOL hRes = Process32First(hSnapShot, &pEntry);
  while (hRes) {
    if (strcmp(pEntry.szExeFile, programmName) == 0) {
      HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD) pEntry.th32ProcessID);
      if (hProcess != NULL) {
        TerminateProcess(hProcess, 9);
        CloseHandle(hProcess);
      }
    }
    hRes = Process32Next(hSnapShot, &pEntry);
  }
  CloseHandle(hSnapShot);
}

BOOL createProgramProcess(const char *program, char *args) {
  STARTUPINFOA si = { sizeof (STARTUPINFOA) };
  PROCESS_INFORMATION pi;

  BOOL res = CreateProcessA(program, args, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
  return res;
}

void regGetString(LPCTSTR valueToRead) {
  HKEY hKey = HKEY_CURRENT_USER;
  LPCTSTR subKey = g_subKey;
  LPCTSTR value = TEXT("SteamExe");

}

void setUser(LPCTSTR user) {
  HKEY hKey = HKEY_CURRENT_USER;
  LPCTSTR subKey = g_subKey;
  LPCTSTR value = TEXT("AutoLoginUser");

  LPCTSTR data;
  DWORD dataSize;

  data = user;
  dataSize = lstrlen(user);
  // std::cout << "data: " << data << " | dataSize: " << dataSize << std::endl;

  HKEY current_hKey;
  LONG openStatus = RegOpenKey(
    HKEY_CURRENT_USER,
    subKey,
    &current_hKey
  );

  LONG setStatus = RegSetValueEx(
    current_hKey,
    value,
    0,
    REG_SZ,
    (LPBYTE)data,
    dataSize
  );

  LONG closeStatus = RegCloseKey(
    current_hKey
  );
}

void setRememberPassword(DWORD num) {
  HKEY hKey = HKEY_CURRENT_USER;
  LPCTSTR subKey = g_subKey;
  LPCTSTR value = TEXT("RememberPassword");

  DWORD data;
  DWORD dataSize;

  data = num;
  dataSize = sizeof(data);
  // std::cout << "data: " << data << " | dataSize: " << dataSize << std::endl;

  HKEY current_hKey;
  LONG openStatus = RegOpenKey(
    HKEY_CURRENT_USER,
    subKey,
    &current_hKey
  );

  LONG setStatus = RegSetValueEx(
    current_hKey,
    value,
    0,
    REG_DWORD,
    (LPBYTE)&data,
    dataSize
  );

  LONG closeStatus = RegCloseKey(
    current_hKey
  );
}

int main(int argc, char const *argv[]) {

  if (argc <= 1) {
    std::cout << "usage:" << std::endl;
    std::cout << "\tswitchsteam login <steam_username>" << std::endl;
    std::cout << "\tswitchsteam new" << std::endl;
    return 0;
  } 

  killProgramProcess("steam.exe");

  if (strcmp(argv[1],"login")==0) {
    LPCTSTR user = TEXT(argv[2]);
    std::cout << "setting the user: " << user << std::endl;
    
    setUser(user);
    setRememberPassword(1);

  } else if (strcmp(argv[1],"new")==0) {
    std::cout << "setting new user..." << std::endl;

    setUser("enter_new_username");
    setRememberPassword(0);

  }

  DWORD dataLength = 1024;
  char data[1024];
  LONG getStatus = RegGetValue(HKEY_CURRENT_USER, TEXT("Software\\Valve\\Steam"), TEXT("SteamExe"), RRF_RT_REG_SZ, NULL, &data, &dataLength);

  BOOL ok = createProgramProcess(data, NULL);
  std::cout << "steam start: " << ok << std::endl;

  return 0;
}
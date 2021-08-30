#include <Windows.h>
#include <TlHelp32.h>


#include <iostream>


using PTR = uintptr_t;
using BYTE = unsigned char;


class Memory
{
public:
  Memory(const char* windowName, const char* executableName) {
    CreateHandle(windowName); // Connects with the game and gets a handle with the PROCESS_ALL_ACCESS flag
    GetModuleBaseAddress(executableName); // Finds the base address of the program
  }

  bool checkValidity() {
    bool isValid = IsWindow(Window);
    if(!isValid) { std::cout << "Lost Connect to the Application" << std::endl; }
    return isValid;
  }

  // template <typename Type>
  // Type writeValue(LPVOID Addr, Type Value) {
  //   WriteProcessMemory(Handle, (PBYTE*)Addr, &Value, sizeof(Value), 0);
  //   return Value;
  // }
  template <typename Type>
  Type writeValue(uintptr_t Addr, Type Value, bool OffsetByBase = false) {
    WriteProcessMemory(Handle, (PBYTE*)getAddress(Addr, OffsetByBase), &Value, sizeof(Value), 0);
    return Value;
  }

  template <typename Type>
  Type readValue(LPVOID Addr) { Type Value;
    ReadProcessMemory(Handle, (PBYTE*)Addr, &Value, sizeof(Value), 0);
    return Value;
  }
  template <typename Type>
  Type readValue(uintptr_t Addr, bool OffsetByBase = false) { Type Value;
    ReadProcessMemory(Handle, (PBYTE*)getAddress(Addr, OffsetByBase), &Value, sizeof(Value), 0);
    return Value;
  }

  template <typename Type>
  uintptr_t readPointer(LPVOID Addr) { uintptr_t Value;
    ReadProcessMemory(Handle, (PBYTE*)Addr, &Value, sizeof(Type), 0);
    return Value;
  }
  template <typename Type>
  uintptr_t readPointer(uintptr_t Addr) { uintptr_t Value;
    ReadProcessMemory(Handle, (PBYTE*)Addr, &Value, sizeof(Type), 0);
    return Value;
  }
  template <typename Type>
  uintptr_t readPointer (uintptr_t Addr, bool OffsetByBase) { uintptr_t Value;
    Addr = getAddress(Addr, OffsetByBase);
    ReadProcessMemory(Handle, (PBYTE*)((LPVOID)Addr), &Value, sizeof(Addr), 0);
    return Value;
  }

  template <typename Type> // Syntax -> Memory.readValueFromPointer<Type>(Addr,Based?,Offset)
  Type readValueFromPointer(uintptr_t Addr, bool OffsetByBase = false, uintptr_t LocalOffset = 0x0) {
    uintptr_t Pointer = readPointer<float>(getAddress(Addr, OffsetByBase));
    return readValue<Type>(getAddress(Pointer+LocalOffset));
  }
  template <typename Type> // Syntax -> Memory.readValueFromPointer<Type>(Addr,Based?,Offset)
  Type writeValueToPointer(uintptr_t Addr, Type Value, bool OffsetByBase = false, uintptr_t LocalOffset = 0x0) {
    // std::cout << "hi";
    uintptr_t Pointer = readPointer<float>(getAddress(Addr, OffsetByBase));
    return writeValue<Type>(getAddress(Pointer+LocalOffset), Value);
  }

  uintptr_t getAddress(uintptr_t Addr, bool OffsetByBase = false) {
    if(OffsetByBase) { return (BaseAddr + Addr); } // Offsets the address relative to the base address
    else { return Addr; } // Returns the int stored pointer as a void pointer
  }

//private:
  HWND Window;    // Racer Window
  DWORD ProcID;   // Racer Thread ID
  HANDLE Handle;  // Racer Process Handle
  uintptr_t BaseAddr; // Base Address of Racer

  bool CreateHandle(const char* windowName) {
    Window = FindWindowA(NULL, windowName);
    if(Window == NULL) {
      std::cout << "Could Not Connect, is the Game Open?" << std::endl;
    }
    else {
      std::cout << "[1/2] Connected to Window" << std::endl;

      GetWindowThreadProcessId(Window, &ProcID);
      Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcID);

      if(Handle == NULL)
        { std::cout << "Error Obtaining Process Handle, Try Running as Admin" << std::endl; }
      else{
        std::cout << "[2/2] Obtained Process Handle" << std::endl;

        return true; // Connection Successful
      }
    }
    return false; // Could not Connect to the Game
  }

  void GetModuleBaseAddress(const char* executableName)
  {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcID);
    if (hSnap != INVALID_HANDLE_VALUE) {
      MODULEENTRY32 modEntry;
      modEntry.dwSize = sizeof(modEntry);
      if (Module32First(hSnap, &modEntry)) {
        do {
          if (!_stricmp(modEntry.szModule, executableName)) {  // Checks input parameter with murrent module
            BaseAddr = (uintptr_t)modEntry.modBaseAddr; // Updates the Base Address Variable
            return;
          }
        } while (Module32Next(hSnap, &modEntry));
      }
    }
    CloseHandle(hSnap);
  }
};

#include <Windows.h>
#include <TlHelp32.h>


class Memory
{
public:
  Memory(std::string) { throw std::invalid_argument( "Dont use this constructor" ); }
  Memory(const char* windowName, const char* executableName) {
    CreateHandle(windowName); // Connects with the game and gets a handle with the PROCESS_ALL_ACCESS flag
    GetModuleBaseAddress(executableName); // Finds the base address of the program
  }

  bool checkValidity() {
    bool isValid = IsWindow(Window); // Checks if the window is still open and valid
    if(!isValid) { std::cout << "Lost Connect to the Application" << std::endl; }
    return isValid;
  }


// Read Functions
  template <typename Type> // Syntax -> Memory.readValue<Type>(Addr,Based?)
  Type readValue(uintptr_t Addr, bool Based = false) { Type Value;
    if(Based) Addr+=BaseAddr;
    ReadProcessMemory(Handle, (LPVOID)Addr, &Value, sizeof(Value), 0);
    return Value;
  }

  uintptr_t readPointer(uintptr_t Addr, bool Based = false) {
    if(Based) Addr+=BaseAddr;
    ReadProcessMemory(Handle, (LPVOID)Addr, &Addr, sizeof(unsigned int), 0);
    return Addr;
  }

  template <typename Type> // Syntax -> Memory.readValueFromPointer<Type>(Addr,Based?,Offset)
  Type readValueFromPointer(uintptr_t Addr, bool Based = false, uintptr_t LocalOffset = 0x0) {
    if(Based) Addr+=BaseAddr;
    uintptr_t Pointer = readPointer(Addr);
    return readValue<Type>(Pointer+LocalOffset);
  }


// Write Functions
  // template <typename Type>
  // Type writeValue(uintptr_t Addr, Type Value, bool OffsetByBase = false) {
  //   WriteProcessMemory(Handle, (PBYTE*)getAddress(Addr, OffsetByBase), &Value, sizeof(Value), 0);
  //   return Value;
  // }

  // template <typename Type> // Syntax -> Memory.readValueFromPointer<Type>(Addr,Based?,Offset)
  // Type writeValueToPointer(uintptr_t Addr, Type Value, bool OffsetByBase = false, uintptr_t LocalOffset = 0x0) {
  //   // std::cout << "hi";
  //   uintptr_t Pointer = readPointer<float>(getAddress(Addr, OffsetByBase));
  //   return writeValue<Type>(getAddress(Pointer+LocalOffset), Value);
  // }

  uintptr_t getBase() { return BaseAddr; }

private:
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

#include <Windows.h>
#include <TlHelp32.h>


class Memory
{
public:
  // Memory(std::string) { throw std::invalid_argument( "Dont use this constructor" ); }
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
  Type readValue(uintptr_t address, bool based = false) { Type output;
    if(based) address+=BaseAddr;
    ReadProcessMemory(Handle, (LPVOID)address, &Value, sizeof(Value), 0);
    return output;
  }

  uintptr_t readPointer(uintptr_t address, bool based = false) {
    if(based) address+=BaseAddr;
    ReadProcessMemory(Handle, (LPVOID)address, &address, sizeof(unsigned int), 0);
    return address;

    // return readValue<uintptr_t>(address, Based);
  }

  template <typename Type> // Syntax -> Memory.readValueFromPointer<Type>(Addr,Based?,Offset)
  Type readValueFromPointer(uintptr_t address, bool based = false, uintptr_t localOffset = 0x0) {
    if(based) address+=BaseAddr;
    uintptr_t pointer = readPointer(address);
    return readValue<Type>(pointer+localOffset);

    // uintptr_t pointer = readPointer(address,based);
    // return readValue<Type>(pointer+localOffset);
  }


  // template <typename Type>
  // void writeValue(Type input, uintptr_t address, bool based = false) {
  //   if(Based) Addr+=BaseAddr;
  //   WriteProcessMemory(Handle, (LPVOID)Addr, input, sizeof(input), 0);
  // }

  // template <typename Type>
  // void writeValueToPointer(Type input, uintptr_t address, bool based = false, uintptr_t localOffset = 0x0) {
  //   uintptr_t pointer = readPointer(address,based);
  //   writeValue<Type>(input, pointer+localOffset);
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

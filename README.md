# Windows Memory Manipulator

A single header file that can create handles to read and write from protected memory addresses within an application or game.

## Usage
- Include memory.h
- Create an instance with parameters:
  - Window name
  - Executable name
```cpp
Memory mem = Memory("Episode I Racer", "SWEP1RCR.EXE");
```
- Use the following functions to manip memory
  - readvalue / writevalue
  - readpointer / writepointer
  - readvaluefrompointer / writevaluefrompointer
```cpp
 // BYTE -> unsigned char
BYTE value = mem.readValue<BYTE>(0x0) // Reads unsigned char from mem address 0x0
float value2 = mem.readValueFromPointer<float>(0x0); // Reads float from the address that 0x0 points to

 // PTR -> uintptr_t  // This is not a pointer but an int big enough to store a pointer location
PTR pointer = mem.readPointer<float>(0x0); // Reads the pointer stored in 0x0 and saves it as a uintptr_t
```

- Compile and make sure to run as admin!

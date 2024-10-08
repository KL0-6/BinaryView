/*
* Define the structure for Windows PE
* Based on WinAPI (winnt.h)
*/

#pragma once
#include <cstdint>
#include <fstream>
#include <vector>
#include <imgui.h> 
#include <inttypes.h>

// MS Dos headers, most of the fields here are unused by modern Windows. Only exists for compatability. 
typedef struct _IMAGE_DOS_HEADER 
{
    uint16_t e_magic;      /* 00: MZ Header signature */
    uint16_t e_cblp;       /* 02: Bytes on last page of file */
    uint16_t e_cp;         /* 04: Pages in file */
    uint16_t e_crlc;       /* 06: Relocations */
    uint16_t e_cparhdr;    /* 08: Size of header in paragraphs */
    uint16_t e_minalloc;   /* 0a: Minimum extra paragraphs needed */
    uint16_t e_maxalloc;   /* 0c: Maximum extra paragraphs needed */
    uint16_t e_ss;         /* 0e: Initial (relative) SS value */
    uint16_t e_sp;         /* 10: Initial SP value */
    uint16_t e_csum;       /* 12: Checksum */
    uint16_t e_ip;         /* 14: Initial IP value */
    uint16_t e_cs;         /* 16: Initial (relative) CS value */
    uint16_t e_lfarlc;     /* 18: File address of relocation table */
    uint16_t e_ovno;       /* 1a: Overlay number */
    uint16_t e_res[4];     /* 1c: Reserved words */
    uint16_t e_oemid;      /* 24: OEM identifier (for e_oeminfo) */
    uint16_t e_oeminfo;    /* 26: OEM information; e_oemid specific */
    uint16_t e_res2[10];   /* 28: Reserved words */
    uint32_t e_lfanew;     /* 3c: Offset to extended header (Usually NT Headers) */
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_SECTION_HEADER 
{
    uint8_t Name[8];                  // 00: Section name
    union {
        uint32_t PhysicalAddress;
        uint32_t VirtualSize;
    } Misc;                            // 08: Physical address / Virtual size
    uint32_t VirtualAddress;           // 0C: Virtual address
    uint32_t SizeOfRawData;            // 10: Size of raw data in the file
    uint32_t PointerToRawData;         // 14: File pointer to raw data
    uint32_t PointerToRelocations;     // 18: File pointer to relocations
    uint32_t PointerToLinenumbers;     // 1C: File pointer to line numbers
    uint16_t NumberOfRelocations;      // 20: Number of relocations
    uint16_t NumberOfLinenumbers;      // 22: Number of line numbers
    uint32_t Characteristics;          // 24: Characteristics of the section
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

// Constants https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
enum class MachineArc : uint16_t
{
    UNKNOWN = 0x0000,              // The content of this field is assumed to be applicable to any machine type
    ALPHA = 0x0184,                // Alpha AXP, 32-bit address space
    ALPHA64 = 0x0284,              // Alpha 64, 64-bit address space
    AM33 = 0x01D3,                 // Matsushita AM33
    AMD64 = 0x8664,                // x64
    ARM = 0x01C0,                  // ARM little endian
    ARM64 = 0xAA64,                // ARM64 little endian
    ARMNT = 0x01C4,                // ARM Thumb-2 little endian
    AXP64 = 0x0284,                // AXP 64 (Same as Alpha 64)
    EBC = 0x0EBC,                  // EFI byte code
    I386 = 0x014C,                 // Intel 386 or later processors and compatible processors
    IA64 = 0x0200,                 // Intel Itanium processor family
    LOONGARCH32 = 0x6232,          // LoongArch 32-bit processor family
    LOONGARCH64 = 0x6264,          // LoongArch 64-bit processor family
    M32R = 0x9041,                 // Mitsubishi M32R little endian
    MIPS16 = 0x0266,               // MIPS16
    MIPSFPU = 0x0366,              // MIPS with FPU
    MIPSFPU16 = 0x0466,            // MIPS16 with FPU
    POWERPC = 0x01F0,              // Power PC little endian
    POWERPCFP = 0x01F1,            // Power PC with floating point support
    R4000 = 0x0166,                // MIPS little endian
    RISCV32 = 0x5032,              // RISC-V 32-bit address space
    RISCV64 = 0x5064,              // RISC-V 64-bit address space
    RISCV128 = 0x5128,             // RISC-V 128-bit address space
    SH3 = 0x01A2,                  // Hitachi SH3
    SH3DSP = 0x01A3,               // Hitachi SH3 DSP
    SH4 = 0x01A6,                  // Hitachi SH4
    SH5 = 0x01A8,                  // Hitachi SH5
    THUMB = 0x01C2,                // Thumb
    WCEMIPSV2 = 0x0169             // MIPS little-endian WCE v2
};

typedef struct _IMAGE_FILE_HEADER 
{
    MachineArc Machine;              /*00: */
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER64 
{
    uint16_t Magic;
    uint8_t MajorLinkerVersion;
    uint8_t MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint64_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint64_t SizeOfStackReserve;
    uint64_t SizeOfStackCommit;
    uint64_t SizeOfHeapReserve;
    uint64_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    struct {
        uint32_t VirtualAddress;
        uint32_t Size;
    } DataDirectory[16];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

// PE structure 
typedef struct _IMAGE_NT_HEADERS
{
    uint32_t Signature;                           /*00: PE Signature */
    IMAGE_FILE_HEADER FileHeader;                 /*02: Attributes */
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

#define IMAGE_SCN_MEM_DISCARDABLE		0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED		0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED			0x08000000
#define IMAGE_SCN_MEM_SHARED			0x10000000
#define IMAGE_SCN_MEM_EXECUTE			0x20000000
#define IMAGE_SCN_MEM_READ			    0x40000000
#define IMAGE_SCN_MEM_WRITE			    0x80000000

class PE 
{
public:
    PE(std::fstream& file) : file_(file) {}

    const char* get_arc_name(MachineArc);
    IMAGE_DOS_HEADER* get_dos();
    IMAGE_NT_HEADERS* get_nt();
    std::vector<IMAGE_SECTION_HEADER> get_sections();
    std::vector<std::string> get_rdata_strings();

    void render_sidebar();
    void render_main();

private:
    IMAGE_DOS_HEADER* dos = nullptr;
    IMAGE_NT_HEADERS* nt  = nullptr;
    std::vector<IMAGE_SECTION_HEADER> sections;
    std::vector<std::string> rdata_strings;

    std::fstream& file_;
};
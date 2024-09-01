#include <fstream>
#include <inttypes.h>

#include "PE/PE.h"

int main()
{
    std::fstream file("/home/user/Downloads/DiscordSetup.exe", std::ios::in | std::ios::binary);

    if(!file.is_open())
    {
        std::printf("Failed to open file!\n");

        return -1;
    }

    IMAGE_DOS_HEADER header;
    file.read(reinterpret_cast<char*>(&header), sizeof(IMAGE_DOS_HEADER)); // The first X bytes of a PE executeable is the DOS header

    if(header.e_magic != 0x5A4D) // 54 = 'M', 4D = 'Z' | DOS executeables have this signature to verify, Windows supports DOS for legacy compatability reasons.
    {
        std::printf("Executeable isn't in DOS format!\n");

        return -1;
    }

    // Move to where the extended header is (NT Headers)
    file.seekg(header.e_lfanew, std::ios::beg);

    IMAGE_NT_HEADERS nt_headers;
    file.read(reinterpret_cast<char*>(&nt_headers), sizeof(nt_headers)); // Read the NT headers

    if(nt_headers.Signature != 0x4550) // 45 = 'E', 50 = 'P' | PE executeables have this signature to verify.
    {
        std::printf("Executeable isn't in PE format!\n");

        return -1;
    }

    std::printf("%" PRIu32 "\n", nt_headers.FileHeader.TimeDateStamp);

    return 0;
}
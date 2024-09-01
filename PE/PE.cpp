#include "PE.h"
#include "../widgets/widgets.h"

const char* PE::get_arc_name(MachineArc machine)
{
    switch (machine) 
    {
        case MachineArc::UNKNOWN: return "Unknown";
        case MachineArc::ALPHA: return "Alpha AXP, 32-bit address space";
        case MachineArc::ALPHA64: return "Alpha/AXP 64, 64-bit address space";
        case MachineArc::AM33: return "Matsushita AM33";
        case MachineArc::AMD64: return "x64";
        case MachineArc::ARM: return "ARM little endian";
        case MachineArc::ARM64: return "ARM64 little endian";
        case MachineArc::ARMNT: return "ARM Thumb-2 little endian";
        case MachineArc::EBC: return "EFI byte code";
        case MachineArc::I386: return "x86";
        case MachineArc::IA64: return "Intel Itanium processor family";
        case MachineArc::LOONGARCH32: return "LoongArch 32-bit";
        case MachineArc::LOONGARCH64: return "LoongArch 64-bit";
        case MachineArc::M32R: return "Mitsubishi M32R little endian";
        case MachineArc::MIPS16: return "MIPS16";
        case MachineArc::MIPSFPU: return "MIPS with FPU";
        case MachineArc::MIPSFPU16: return "MIPS16 with FPU";
        case MachineArc::POWERPC: return "Power PC little endian";
        case MachineArc::POWERPCFP: return "Power PC with floating point support";
        case MachineArc::R4000: return "MIPS little endian";
        case MachineArc::RISCV32: return "RISC-V 32-bit";
        case MachineArc::RISCV64: return "RISC-V 64-bit";
        case MachineArc::RISCV128: return "RISC-V 128-bit";
        case MachineArc::SH3: return "Hitachi SH3";
        case MachineArc::SH3DSP: return "Hitachi SH3 DSP";
        case MachineArc::SH4: return "Hitachi SH4";
        case MachineArc::SH5: return "Hitachi SH5";
        case MachineArc::THUMB: return "Thumb";
        case MachineArc::WCEMIPSV2: return "MIPS little-endian WCE v2";
        default: return "Unknown Architecture";
    }

    return "Unknown Architecture";
}

IMAGE_DOS_HEADER* PE::get_dos()
{
    if(dos == nullptr)
    {
        dos = new IMAGE_DOS_HEADER;

        file_.read(reinterpret_cast<char*>(dos), sizeof(IMAGE_DOS_HEADER)); // The first X bytes of a PE executeable is the DOS header
    
        if(dos->e_magic != 0x5A4D) // 54 = 'M', 4D = 'Z' | DOS executeables have this signature to verify, Windows supports DOS for legacy compatability reasons.
            return nullptr;

        return dos;
    }

    return dos;
}

IMAGE_NT_HEADERS* PE::get_nt()
{
    if(nt == nullptr)
    {
        nt = new IMAGE_NT_HEADERS;

        file_.seekg(dos->e_lfanew, std::ios::beg);
        file_.read(reinterpret_cast<char*>(nt), sizeof(IMAGE_NT_HEADERS)); // Read the NT headers

        if(nt->Signature != 0x4550) // 45 = 'E', 50 = 'P' | PE executeables have this signature to verify.
            return nullptr;
    }

    return nt;
}

std::vector<IMAGE_SECTION_HEADER> PE::get_sections()
{
    if(sections.empty())
    {
        sections.reserve(nt->FileHeader.NumberOfSections);
        sections.resize(nt->FileHeader.NumberOfSections);

        file_.seekg(dos->e_lfanew + offsetof(IMAGE_NT_HEADERS, OptionalHeader) + nt->FileHeader.SizeOfOptionalHeader, std::ios::beg);
        file_.read(reinterpret_cast<char*>(sections.data()), sizeof(IMAGE_SECTION_HEADER) * sections.size());
    }

    return sections;
}

std::vector<std::string> PE::get_rdata_strings()
{
    if(!rdata_strings.empty())
        return rdata_strings;

    for (IMAGE_SECTION_HEADER& section : sections) 
    {
        char* section_name = reinterpret_cast<char*>(section.Name);
        if(strcmp(section_name, ".rdata") == 0)
        {
            std::vector<uint8_t> sectionData(section.SizeOfRawData);
            file_.seekg(section.PointerToRawData, std::ios::beg);
            file_.read(reinterpret_cast<char*>(sectionData.data()), section.SizeOfRawData);
            
            std::string string = "";
            for (int i = 0; i < section.SizeOfRawData; ++i)
            {
                char c = sectionData.data()[i];

                if(c == '\0')
                {
                    if(string.size() >= 4)
                        rdata_strings.emplace_back(std::move(string));

                    string.clear();
                }
                else
                    string += c;
            }
            
            if(string.size() >= 4)
            {
                rdata_strings.emplace_back(std::move(string));

                string.clear();
            }        
        }
    }

    return rdata_strings;
}

/*
* UI Elements
*/

static bool showIMAGE_DOS_HEADER = false;
static bool showIMAGE_FILE_HEADER = false;
static bool showIMAGE_SECTION_HEADER = false;
static bool showSTRINGS = false;
static std::string rdataSearchQuery = "";

#define NEW_TABLE_ENTRY(field, value, type) ImGui::TableNextRow(); \
                    ImGui::TableSetColumnIndex(0); \
                    ImGui::Text("%s", field); \
                    ImGui::TableSetColumnIndex(1); \
                    ImGui::Text("%" type, value);

void PE::render_sidebar()
{

    if (ImGui::Button("STRINGS", ImVec2(-1, 0)))
        showSTRINGS = !showSTRINGS;

    if (ImGui::Button("IMAGE_DOS_HEADER", ImVec2(-1, 0)))
        showIMAGE_DOS_HEADER = !showIMAGE_DOS_HEADER;

    if (ImGui::Button("IMAGE_FILE_HEADER", ImVec2(-1, 0)))
        showIMAGE_FILE_HEADER = !showIMAGE_FILE_HEADER;

    if (ImGui::Button("IMAGE_SECTION_HEADER", ImVec2(-1, 0)))
        showIMAGE_SECTION_HEADER = !showIMAGE_SECTION_HEADER;
}

void PE::render_main()
{
    if(showSTRINGS)
    {
        if (ImGui::TreeNode("STRINGS"))
        {
            ImGui::InputTextWithHintR("Search", rdataSearchQuery);

            for (std::string& string : get_rdata_strings()) 
            {
                if(string.find(rdataSearchQuery) != std::string::npos)
                    ImGui::Text("%s", string.c_str());
            }
           
            ImGui::TreePop();
        }
    }   

    if(showIMAGE_DOS_HEADER)
    {
        if (ImGui::TreeNode("IMAGE_DOS_HEADER"))
        {
            if (ImGui::BeginTable("IMAGE_DOS_HEADER", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) 
            {
                ImGui::TableSetupColumn("Field");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                NEW_TABLE_ENTRY("e_magic", dos->e_magic, PRIx16);
                NEW_TABLE_ENTRY("e_magic", dos->e_magic, PRIx16);
                NEW_TABLE_ENTRY("e_cblp", dos->e_cblp, PRIx16);
                NEW_TABLE_ENTRY("e_cp", dos->e_cp, PRIX16);
                NEW_TABLE_ENTRY("e_crlc", dos->e_crlc, PRIX16);
                NEW_TABLE_ENTRY("e_cparhdr", dos->e_cparhdr, PRIX16);
                NEW_TABLE_ENTRY("e_minalloc", dos->e_minalloc, PRIX16);
                NEW_TABLE_ENTRY("e_maxalloc", dos->e_maxalloc, PRIX16);
                NEW_TABLE_ENTRY("e_ss", dos->e_ss, PRIX16);
                NEW_TABLE_ENTRY("e_sp", dos->e_sp, PRIX16);
                NEW_TABLE_ENTRY("e_csum", dos->e_csum, PRIX16);
                NEW_TABLE_ENTRY("e_ip", dos->e_ip, PRIX16);
                NEW_TABLE_ENTRY("e_cs", dos->e_cs, PRIX16);
                NEW_TABLE_ENTRY("e_lfarlc", dos->e_lfarlc, PRIX16);
                NEW_TABLE_ENTRY("e_ovno", dos->e_ovno, PRIX16);
                NEW_TABLE_ENTRY("e_oemid", dos->e_oemid, PRIX16);
                NEW_TABLE_ENTRY("e_oeminfo", dos->e_oeminfo, PRIX16);
                NEW_TABLE_ENTRY("e_lfanew", dos->e_lfanew, PRIX16);

                ImGui::EndTable();
            }

            ImGui::TreePop();
        }
    }

    if(showIMAGE_FILE_HEADER)
    {
        if (ImGui::TreeNode("IMAGE_FILE_HEADER"))
        {
            ImGui::Text("%s: %s", "Machine", get_arc_name(nt->FileHeader.Machine));

            ImGui::TreePop();
        }
    }

    if(showIMAGE_SECTION_HEADER)
    {
        if (ImGui::TreeNode("IMAGE_SECTION_HEADER"))
        {
            for (IMAGE_SECTION_HEADER& section : sections) 
            {
                if(ImGui::TreeNode(reinterpret_cast<const char*>(section.Name)))
                {
                    if (ImGui::BeginTable(reinterpret_cast<const char*>(section.Name), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) 
                    {
                        ImGui::TableSetupColumn("Field");
                        ImGui::TableSetupColumn("Value");
                        ImGui::TableHeadersRow();

                        bool is_readonly = (section.Characteristics & IMAGE_SCN_MEM_READ) && !(section.Characteristics & IMAGE_SCN_MEM_WRITE);
                                
                        NEW_TABLE_ENTRY("IsReadonly", is_readonly ? "true" : "false", "s");
                        NEW_TABLE_ENTRY("VirtualAddress", section.VirtualAddress, PRIu32);
                        NEW_TABLE_ENTRY("SizeOfRawData", section.SizeOfRawData, PRIu32);
                        NEW_TABLE_ENTRY("PointerToRawData", section.PointerToRawData, PRIu32);
                        NEW_TABLE_ENTRY("PointerToRelocations", section.PointerToRelocations, PRIu32);
                        NEW_TABLE_ENTRY("PointerToLinenumbers", section.PointerToLinenumbers, PRIu32);
                        NEW_TABLE_ENTRY("NumberOfRelocations", section.NumberOfRelocations, PRIu16);
                        NEW_TABLE_ENTRY("NumberOfLinenumbers", section.NumberOfLinenumbers, PRIu16);
                        NEW_TABLE_ENTRY("Characteristics", section.Characteristics, PRIu32);

                        ImGui::EndTable();
                    }

                    if(ImGui::TreeNode("Misc"))
                    {
                        ImGui::Text("%s: %" PRIu32, "PhysicalAddress", section.Misc.PhysicalAddress);
                        ImGui::Text("%s: %" PRIu32, "VirtualSize", section.Misc.VirtualSize);

                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    } 
}
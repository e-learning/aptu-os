#include <elf.h>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

const std::map<Elf32_Half, std::string> types =     {   {0, "No ﬁle type"},
                                                        {1, "Relocatable ﬁle"},
                                                        {2, "Executable ﬁle"},
                                                        {3, "Shared object ﬁle"},
                                                        {4, "Core ﬁle"},
                                                        {0xff00, "Processor-speciﬁc"},
                                                        {0xffff, "Processor-speciﬁc" }
                                            };
const std::map<Elf32_Half, std::string> os =     {      {0x00, "System V"},
                                                        {0x01, "HP-UX"},
                                                        {0x02, "NetBSD"},
                                                        {0x03, "Linux"},
                                                        {0x06, "Solaris"},
                                                        {0x07, "AIX"},
                                                        {0x08, "IRIX" },
                                                        {0x09, "FreeBSD"},
                                                        {0x0C, "OpenBSD"}
                                                 };

const std::map<Elf32_Half, std::string> archs =     {   {0, "No machine"},
                                                        {3, "Intel 80386"},
                                                        {7, "Intel 80860"},
                                                        {9, "IBM System/370 Processor"},
                                                        {19, "Intel 80960"},
                                                        {20, "PowerPC"},
                                                        {21, "64-bit PowerPC" },
                                                        {50, "Intel IA-64 processor architecture"}
                                                    };

const std::map<Elf32_Word, std::string> stypes =     {   {0, "NULL"},
                                                        {1, "PROGBITS"},
                                                        {2, "SYMTAB"},
                                                        {3, "STRTAB"},
                                                        {4, "RELA"},
                                                        {5, "HASH"},
                                                        {6, "DYNAMIC"},
                                                        {7, "NOTE"},
                                                        {8, "NOBITS"},
                                                        {9, "REL"},
                                                        {10, "SHLIB"},
                                                        {11, "DYNSYM"},
                                                        {0x70000000, "LOPROC"},
                                                        {0x7fffffff, "HIPROC" },
                                                        {0x80000000, "LOUSER"},
                                                        {0xffffffff, "HIUSER" }
                                            };


const std::map<Elf32_Word, std::string> ptypes = 	{  {0, "NULL"},
                                                        {1, "LOAD"},
                                                        {2, "DYNAMIC"},
                                                        {3, "INTERP"},
                                                        {4, "NOTE"},
                                                        {5, "SHLIB"},
                                                        {6, "PHDR"},
                                                        {7, "TLS"},
                                                        {0x60000000, "LOOS"},
                                                        {0x6fffffff, "HIOS" },
                                                        {0x70000000, "LOPROC"},
                                                        {0x7fffffff, "HIPROC" },
                                                        {0x6474e550, "GNU_EH_FRAME"},
                                                        {0x6474e551, "GNU_STACK"},
                                                        {0x6474e552, "GNU_RELRO"}
                                            		};


size_t fileLength(std::ifstream& stream) 
{
    stream.seekg(0, stream.end);
    size_t length = stream.tellg();
    stream.seekg(0, stream.beg);
    return length;
}

void printElfHeader(Elf32_Ehdr& hdr) 
{
  std::cout << "Magic : ";
  for (size_t i = 0; i < EI_NIDENT; i++) {
    std::cout << std::hex << (int)hdr.e_ident[i] << " ";
  }
  std::cout << std::dec << "\n"
            << "Class : " << ((hdr.e_ident[EI_CLASS] == 1) ? "ELF32" : "ELF64") << "\n"
            << "Data : " << ((hdr.e_ident[EI_DATA] == 1) ? "little endian" : "big endian") << "\n"
            << "Version : " << (int)hdr.e_ident[EI_VERSION] << "\n"     
            << "OS/ABI : " << ((os.find(hdr.e_ident[EI_OSABI]) != os.end()) ? os.at(hdr.e_ident[EI_OSABI]) : "unknown") << "\n"      
            << "ABI version : " << (int)hdr.e_ident[EI_ABIVERSION] << "\n"
            << "Object file type : " << ((types.find(hdr.e_type) != types.end()) ? types.at(hdr.e_type) : "unknown type") << "\n"
            << "Architecture : " << ((archs.find(hdr.e_machine) != archs.end()) ? archs.at(hdr.e_machine) : "unknown architecture") << "\n"        
            << "Object file version : " << (int) hdr.e_version << "\n"    
            << "Entry point virtual address : 0x" << std::hex << hdr.e_entry << std::dec << "\n"         
            << "Program header table file offset : " << hdr.e_phoff << "\n"         
            << "Section header table file offset : " << hdr.e_shoff << "\n"         
            << "Processor-specific flags : " << hdr.e_flags << "\n"         
            << "ELF header size in bytes : " << hdr.e_ehsize << "\n"        
            << "Program header table entry size : " << hdr.e_phentsize << "\n"         
            << "Program header table entry count : " << hdr.e_phnum << "\n"         
            << "Section header table entry size : " << hdr.e_shentsize << "\n"       
            << "Section header table entry count : " << hdr.e_shnum << "\n"         
            << "Section header string table index : " << hdr.e_shstrndx << std::endl;      
}

std::string getString(std::vector<char>& strtab, size_t index)
{
    std::string name;
    while (index < strtab.size() && strtab[index] != '\0') {
        name += strtab[index];
        index++;
    }
    return name;
}
void printSections(Elf32_Shdr* stab, std::vector<char>& strtab, size_t size) 
{
    std::cout << "Section table : Name Type Addr" << "\n";
    for (size_t i = 0; i < size; i++) {
        std::cout << getString(strtab, stab[i].sh_name) << " "
        << ((stypes.find(stab[i].sh_type) != stypes.end()) ? stypes.at(stab[i].sh_type) : "unknown") 
        << " 0x" << std::hex << stab[i].sh_addr << std::dec << "\n";
    }
}

void printSegments(Elf32_Phdr* ptab, size_t size)
{
    std::cout << "Segment table : Type Virtual addr" << "\n";
    for (size_t i = 0; i < size; i++) {
        std::cout << ((ptypes.find(ptab[i].p_type) != ptypes.end()) ? ptypes.at(ptab[i].p_type) : "unknown") 
        << " 0x" << std::hex << ptab[i].p_vaddr << std::dec << "\n";
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "To few arguments" << std::endl;
        return 1;
    }
    Elf32_Shdr* sectionTable = nullptr;
    Elf32_Phdr* programTable = nullptr;
    std::ifstream input;
    input.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        input.open(argv[1], std::ios_base::in | std::ios_base::binary);
        Elf32_Ehdr hdr;
        input.read((char*) (&hdr), sizeof(hdr));
        printElfHeader(hdr);

        input.seekg(hdr.e_shoff, input.beg);
        Elf32_Shdr* sectionTable = new Elf32_Shdr[hdr.e_shnum];
        input.read((char*) sectionTable, hdr.e_shnum * sizeof(Elf32_Shdr));
        std::vector<char> stringTable;
        input.seekg(sectionTable[hdr.e_shstrndx].sh_offset, input.beg);
        stringTable.resize(sectionTable[hdr.e_shstrndx].sh_size);
        input.read((char*) (&stringTable[0]), sectionTable[hdr.e_shstrndx].sh_size);

        if (stringTable.size() == 0) {
            std::cerr << "error : read string table" << std::endl;
            return 1;
        }
        std::cout << std::endl;
        printSections(sectionTable, stringTable, hdr.e_shnum);
        std::cout << std::endl;
        Elf32_Phdr* programTable = new Elf32_Phdr[hdr.e_phnum];
        input.seekg(hdr.e_phoff, input.beg);
        input.read((char*) programTable, hdr.e_phnum * sizeof(Elf32_Phdr));
        printSegments(programTable, hdr.e_phnum);
        std::cout << std::endl;
    } catch (std::ios_base::failure e) {
        std::cerr << "incorrect ELF file" << std::endl;
    }
    if (sectionTable != nullptr) {
        delete sectionTable;
    }
    if (programTable != nullptr) {
        delete programTable;
    }
    input.close();

    return 0;
}

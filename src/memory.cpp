#include <stdint.h>
#include <string>
#include <vector>
#include <cstdio>

#include "elfio.hpp"

#include "defs.h"
#include "memory.h"

extern SimArgs * cli_args;

Memory::Memory(uint32_t max_addr)
{
    size = max_addr;

    // Allocate memory
    if(!(mem = new uint8_t[max_addr])) 
    {
        throwError("out of memory; memory allocation failed\n", true);
    }
}


Memory::~Memory()
{
    delete [] mem;
    size = 0;
}


uint32_t Memory::fetchWord(uint32_t addr)
{
    uint32_t byte0 = (uint32_t)fetchByte(addr);
    uint32_t byte1 = (uint32_t)fetchByte(addr+1);
    uint32_t byte2 = (uint32_t)fetchByte(addr+2);
    uint32_t byte3 = (uint32_t)fetchByte(addr+3);

    return ((byte3<<24) & 0xff000000) | ((byte2<<16) & 0x00ff0000) | ((byte1<<8) & 0x0000ff00) | (byte0 & 0x000000ff);
}


uint16_t Memory::fetchHalfWord(uint32_t addr)
{
    uint32_t byte0 = (uint32_t)fetchByte(addr);
    uint32_t byte1 = (uint32_t)fetchByte(addr+1);

    return ((byte1<<8) & 0xff00) | (byte0 & 0x00ff);
}


uint8_t Memory::fetchByte(uint32_t addr)
{
    if(!isValidAddress(addr))
    {
        char errmsg[40];
        sprintf(errmsg, "Address out of bounds : 0x%08x", addr);
        throwError(errmsg, true);
        return 0;
    }
    return (uint8_t) mem[addr];
}


void Memory::storeWord(uint32_t addr, uint32_t w)
{
    storeByte(addr,   (uint8_t) (w & 0x000000ff));
    storeByte(addr+1, (uint8_t) ((w & 0x0000ff00) >> 8));
    storeByte(addr+2, (uint8_t) ((w & 0x00ff0000) >> 16));
    storeByte(addr+3, (uint8_t) ((w & 0xff000000) >> 24));
}


void Memory::storeHalfWord(uint32_t addr, uint16_t hw)
{
    storeByte(addr, (uint8_t) (hw & 0x00ff));
    storeByte(addr+1, (uint8_t) ((hw & 0xff00) >> 8));
}


void Memory::storeByte(uint32_t addr, uint8_t byte)
{
    if(!isValidAddress(addr))
    {
        char errmsg[40];
        sprintf(errmsg, "Address out of bounds : 0x%08x", addr);
        throwError(errmsg, true);
        return;
    }
    mem[addr] = byte;
}


unsigned int Memory::initFromElf(std::string ifile, std::vector<int> flags_signatures)
	{
		// Initialize Memory object from input ELF File
		ELFIO::elfio reader;

		// Load file into elf reader
		if (!reader.load(ifile)) 
		{
			throwError("Can't find or process ELF file : " + ifile + "\n", true);
		}

		// Check ELF Class, Endiness & segment count
		if(reader.get_class() != ELFCLASS32)
			throwError("Elf file format invalid: should be 32-bit elf\n", true);
		if(reader.get_encoding() != ELFDATA2LSB)
			throwError("Elf file format invalid: should be little Endian\n", true);

		ELFIO::Elf_Half seg_num = reader.segments.size();

		if(seg_num == 0)
			throwError("Elf file format invalid: should consist of atleast one section\n", true);


		// Read elf and initialize memory
		if(cli_args->verbose_flag)
			std::cout << "Segments found : "<< seg_num <<"\n";

		unsigned int i = 0;
		while (i < seg_num) // iterate over all segments
		{
			const ELFIO::segment * seg = reader.segments[i];

			// Get segment properties
			
			if (seg->get_type() == SHT_PROGBITS)
			{
				int seg_flags = reader.segments[i]->get_flags();

				if(flags_signatures.end() != std::find(flags_signatures.begin(), flags_signatures.end(), seg_flags))	// Flag found in signature list
				{

					const char* seg_data = reader.segments[i]->get_data();
					const uint seg_size = reader.segments[i]->get_file_size();
					ELFIO::Elf64_Addr seg_strt_addr = reader.segments[i]->get_physical_address();

					if(cli_args->verbose_flag)
						printf("Loading Segment %d @ 0x%08x --- ", i, (unsigned int) reader.segments[i]->get_physical_address());
					
					long unsigned int offset = 0;
					while(offset<seg_size)
					{
						storeByte(seg_strt_addr + offset, seg_data[offset]);
						offset++;
					}

					if(cli_args->verbose_flag)
						printf("done\n");
				}
			}
			i++;
		}
		return (unsigned int) reader.get_entry();
	}
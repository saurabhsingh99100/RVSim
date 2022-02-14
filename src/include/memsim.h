#pragma once

#include <stdint.h>
#include "util.h"
#include "defs.h"

/**
 * @brief Memory class
 * This class is used to emulate the memories in simulation backend
 * 
 */
class Memory
{
	public:
	/**
	 * @brief pointer to memory array
	 * 
	 */
	uint8_t * mem;

	/**
	 * @brief Base address of memory
	 * 
	 */
	uint32_t base_addr;

	/**
	 * @brief size of memory
	 * 
	 */
	size_t size;

	/**
	 * @brief Memory access permissions
	*/
	
	bool re;
	bool we;
	bool xe;

	/**
	 * @brief Construct a new Memory object
	 */
	Memory(uint32_t base_addr, size_t size, bool re, bool we, bool xe);


	/**
	 * @brief Destroy the Memory object
	 * 
	 */
	~Memory();


	/**
	 * @brief Check if the address is valid
	 * 
	 * @param addr address
	 * @return true if address is within bounds
	 * @return false if address is out of bounds
	 */
	bool isValidAddress(uint32_t addr)
	{
		return (addr >= base_addr) && (addr < base_addr+size);
	}

	uint32_t global2local(uint32_t addr)
	{
		return addr - base_addr;
	}


	/**
	 * @brief Fetch a 32-bit word from memory
	 * 
	 * @param addr address
	 * @return uint32_t data
	 */
	uint32_t fetchWord(uint32_t addr);


	/**
	 * @brief Fetch a 16-bit half word from memory
	 * 
	 * @param addr address
	 * @return uint16_t data
	 */
	uint16_t fetchHalfWord(uint32_t addr);


	/**
	 * @brief Fetch a 8-bitbyte word from memory
	 * 
	 * @param addr address
	 * @return uint8_t data
	 */
	uint8_t fetchByte(uint32_t addr);


	/**
	 * @brief Store a 32-bit word to memory
	 * 
	 * @param addr address
	 * @param w Word
	 */
	void storeWord(uint32_t addr, uint32_t w);


	/**
	 * @brief Store a 16-bit half word from memory
	 * 
	 * @param addr address
	 * @param hw halfWord
	 */
	void storeHalfWord(uint32_t addr, uint16_t hw);


	/**
	 * @brief Store a 8-bit byte word from memory
	 * 
	 * @param addr address
	 * @param byte byte
	 */
	void storeByte(uint32_t addr, uint8_t byte);


	/**
	 * @brief Initialize memory from an elf file
	 * only sections that match flag signatures are loaded
	 * 
	 * @param ifile filename
	 * @param flags_signatures allowed flag signatures
	 */
	unsigned int initFromElf(std::string ifile, std::vector<int> flags_signatures);
};
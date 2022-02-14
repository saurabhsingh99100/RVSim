#pragma once
#include <vector>
#include <stdint.h>
#include "memsim.h"

class RVCore
{
    public:
    class RF
    {
        private:
        uint32_t regs[31];

        public:
        uint32_t get(uint8_t rnum)
        {
            return (rnum==0) ? 0 : regs[rnum-1];
        }

        void set(uint8_t rnum, uint32_t value)
        {
            if(rnum==0)
            {
                return;
            }
            regs[rnum-1] = value;
        }

        void clear()
        {
            for(int i=1; i<32; i++)
            {
                regs[i] = 0;
            }
        }
    };



    RVCore(uint32_t id, std::vector<Memory> * mem, uint32_t reset_addr);

    void reset();

    void tick();

    void run();

    bool is_halted()
    {
        return halted;
    }

    uint32_t get_id()
    {
        return id;
    }

    
    private:
    // core_id
    uint32_t id;

    // Register file
    RF reg_file;

    // program counter
    uint32_t pc;

    // reset address
    uint32_t reset_addr;

    // instruction register
    uint32_t ir;

    // Sim Memory
    std::vector<Memory> * sim_mem = nullptr;

    // Halted/Running
    bool halted;

    void _fetch();
    void _decode();
    void _execute();
    void _mem_access();
    void _writeback();
};
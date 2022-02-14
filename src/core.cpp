#include <vector>
#include <iostream>
#include <stdint.h>

#include "rvdefs.h"
#include "core.h"
#include "memory.h"

extern SimArgs * cli_args;

RVCore::RVCore(uint32_t id, std::vector<Memory> * sim_mem, uint32_t reset_addr) :
    id(id),
    pc(reset_addr),
    reset_addr(reset_addr),
    sim_mem(sim_mem),
    halted(false)
{}

void RVCore::reset()
{
    reg_file.clear();
    pc = reset_addr;
};


void RVCore::tick()
{
    _fetch();
    _decode();
    _execute();
    _mem_access();
    _writeback();
}


void RVCore::run()
{
    while(!halted)
    {
        tick();
    }
}

void RVCore::_fetch()
{
    if(!halted)
    {
        bool fetch_success = false;
        for(std::vector<Memory>::iterator it = (*sim_mem).begin(); it!=(*sim_mem).end(); it++)
        {
            if(it->isValidAddress(pc))
            {
                ir = it->fetchWord(pc);
                fetch_success = true;
                LOG_DUMP("core[" + std::to_string(id) + "] fetch [" + std::to_string(pc) + "]:" + std::to_string(ir));
                pc+=4;
            }
        }

        if(!fetch_success)
        {
            throwError("Core["+std::to_string(id)+"]: Tried to fetch from an address out of mem bounds [PC:"+std::to_string(pc)+"]", true);
        }
    }
    else
    {
        ir = RV_INSTR_NOP;
    }

}


void RVCore::_decode()
{

}


void RVCore::_execute()
{

}


void RVCore::_mem_access()
{

}


void RVCore::_writeback()
{

}





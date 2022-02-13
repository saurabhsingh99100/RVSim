#pragma once
#include <vector>

#define SIM_VERSION "v0.1"
#define SIM_SPLASH "******** RVSim ********"

struct SimArgs
{
    bool verbose_flag;
    bool debug_flag;

    std::string inp_file;
    uint64_t maxitr;
    std::string uart_port;
    uint16_t uart_baud;
    std::string log_file;
    std::string signature_file;
    std::string isa_string;
    std::string sim_config_json_file;
};


struct SimConfig
{
    struct Hart
    {
        uint32_t id;
    };

    struct MemBlk
    {
        std::string name;
        uint64_t base_addr;
        size_t size;
        
        struct MemPermission
        {
            bool r;
            bool w;
            bool x;
        } permission;
    };

    std::vector<Hart> harts;
    std::vector<MemBlk> memories;
};
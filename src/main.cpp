#include <string>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include <csignal>

#include "cxxopts.hpp"
#include "json.h"

#include "defs.h"
#include "util.h"
#include "memory.h"


SimArgs * cli_args = nullptr;

void exit_sim(int status)
{
    // Perform cleanup tasks


    std::exit(status);
}


/**
 * @brief Handle Termination by SIGINT (Ctrl+C)
 * 
 * @param signal_num 
 */
void sigint_handler(int signal_num)
{
	std::cerr << "\nSIGINT Recieved: Interrupted by user (" << std::to_string(signal_num) << ")" << std::endl;
	exit_sim(EXIT_FAILURE);
}



SimArgs *parse_cli_args(int argc, char ** argv, SimArgs * args, const SimArgs * default_args)
{
    if(argc==1)
    {
        std::cout << 
        SIM_SPLASH "\n"
        "too few args; for more info, try : $ rvsim --help"
        << std::endl;
        exit(EXIT_FAILURE);
    }

    try
    {
        // Usage Message Header
        cxxopts::Options options(argv[0], SIM_SPLASH "\nRISC-V ISA Simulator (" SIM_VERSION ")");
		
        options.positional_help("file").show_positional_help();

		// Adding CLI options
		options.add_options("General")
		("h,help", "Show this message")
		("version", "Show version information")
		("f,file", "Specify an input file (bin or hex)", cxxopts::value<std::string>(args->inp_file))
        ("v,verbose", "Turn on verbose output", cxxopts::value<bool>(args->verbose_flag)->default_value(BOOLSTRING(default_args->verbose_flag)))
		;

		options.add_options("Config")
		("maxitr", "Specify maximum simulation iterations", cxxopts::value<uint64_t>(args->maxitr)->default_value(std::to_string(default_args->maxitr)))
		("p,port", "Use provided serial device for uart", cxxopts::value<std::string>(args->uart_port)->default_value(default_args->uart_port))
		("b,baud", "Specify virtual uart port baudrate", cxxopts::value<uint16_t>(args->uart_baud)->default_value(std::to_string(default_args->uart_baud)))
        ("isa", "Specify RISC-V ISA to emulate", cxxopts::value<std::string>(args->isa_string)->default_value(default_args->isa_string))
        ("c,config", "Specify configuration file for RVSim", cxxopts::value<std::string>(args->sim_config_json_file)->default_value(default_args->sim_config_json_file))
        ;

		options.add_options("Debug")
		("d,debug", "Start in debug mode", cxxopts::value<bool>(args->debug_flag)->default_value(BOOLSTRING(default_args->debug_flag)))
		("l,log", "Generate a log of execution", cxxopts::value<std::string>(args->log_file))
		("signature", "Enable signature dump at hault (Used for riscv compliance tests)", cxxopts::value<std::string>(args->signature_file))
		;

        options.parse_positional({"file"});

        // parse CLI options
		auto result = options.parse(argc, argv);

		if(result.unmatched().size() != 0)
		{
			std::string unknown_args;
			for(unsigned int i=0; i<result.unmatched().size(); i++)
				unknown_args = unknown_args + result.unmatched()[i] + (i==result.unmatched().size()-1 ? "" :", ");
			throwError("Unrecognized arguments [" + unknown_args + "]", true);
		}
        
        if (result.count("help"))
		{
			std::cout << options.help() << std::endl;
			exit(EXIT_SUCCESS);
		}
		if (result.count("version"))
		{
			std::cout << SIM_VERSION << std::endl;
			exit(EXIT_SUCCESS);
		}
		if (result.count("file")>1)
		{
			throwError("Multiple input files specified", true);
		}
		if (result.count("file")==0)
		{
			throwError("No input files specified", true);
		}
    }
    catch(const cxxopts::OptionException& e)
    {
        throwError("Error while parsing command line arguments: " + (std::string)e.what(), true);
    }

    return args;
}


SimConfig *parse_sim_config(SimConfig * cfg, std::string json_file)
{
    // read json
    std::fstream fd;
    fd.open(json_file, std::ios::in);
    std::string fcontents;
    if (fd.is_open())
    {
        std::string line;
        while(getline(fd, line))
        {
           fcontents += (line+"\n");
        }
        fd.close(); //close the file object.
    }
    else
    {
        throwError("Unable to open configuration file ["+json_file+"]", true);
    }

    nlohmann::json jcfg;
    jcfg = nlohmann::json::parse(fcontents);
    DBG_PRINT("Parsed json: " << json_file);

    // Get Hart organisation
    DBG_PRINT("  CPUs:");
    for(nlohmann::json::iterator it=jcfg["CPU"].begin(); it!=jcfg["CPU"].end(); it++)
    {
        std::string hart_id_str = (*it)["id"];
        DBG_PRINT("    * " << hart_id_str);
        uint32_t hart_id;
        sscanf(hart_id_str.c_str(), "%x", &hart_id);

        SimConfig::Hart h = {.id = hart_id};
        cfg->harts.push_back((h));
    }

    // Get Memory Organisation
    DBG_PRINT("  MEMs:");
    for(nlohmann::json::iterator it=jcfg["MEM"].begin(); it!=jcfg["MEM"].end(); it++)
    {
        DBG_PRINT("    * " << (*it)["name"]);

        SimConfig::MemBlk m = {
            .name = (*it)["name"],
            .base_addr = (*it)["base"],
            .size = (*it)["size"],
            .permission = {.r=(*it)["re"], .w=(*it)["we"], .x=(*it)["xe"]}
        };
        cfg->memories.push_back((m));
    }
    return cfg;
}

int main(int argc, char **argv)
{
    // Define SIGINT handler
	signal(SIGINT, sigint_handler);

    // Default values of CLI args
    const SimArgs default_args =
    {
        .verbose_flag=false,
        .debug_flag=false,
        .inp_file="",
        .maxitr=1000000,
        .uart_port="/dev/null",
        .uart_baud=9600,
        .log_file="",
        .signature_file="",
        .sim_config_json_file="rvsim_default.json"
    };

    SimArgs args;
    cli_args = &args;

    // Process CLI Args
    parse_cli_args(argc, argv, &args, &default_args);

    if(args.verbose_flag)   // header
    {
        std::cout << SIM_SPLASH << std::endl;
    }

    // Parse RVSim config from json file
    SimConfig sim_configs;
    parse_sim_config(&sim_configs, args.sim_config_json_file);

    std::vector<Memory> sim_memory;
    sim_memory.reserve(sim_configs.memories.size());

    // Initialize memory
    for(int i=0; i<sim_configs.memories.size(); i++)
    {
        sim_memory.push_back(Memory(sim_configs.memories[i].size));
    }
    
    // Initialize processors

    // Run simulation
    
    // control must never reach here
    throwError("[INTERNAL]: Control fault in main.cpp", true);
    return 0;
}
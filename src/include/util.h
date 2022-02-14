#pragma once
#include <string>
#include <vector>

#define BOOLSTRING(x) \
    x ? "true" : "false"

#ifdef DBG_CODE
#   define DBG(x) x
#else
#   define DBG(x)
#endif

#define DBG_PRINT(X) \
    DBG(std::cout << X << std::endl)

#define LOG_DUMP(X) \
    if(cli_args->log_file.length()!=0)   \
        std::cout << "|" << X << std::endl

void throwError(std::string msg, bool exit=true);

void throwWarning(const std::string msg);

void throwSuccessMessage(std::string msg, bool exit = false);

// =============================== FILE READER =====================================
/**
 * @brief reads a binary file
 * 
 * @param memfile filepath
 * @return std::vector<char> contents
 */
std::vector<char> fReadBin(std::string memfile);

/**
 * @brief Reads a file and returns its contents
 * 
 * @param filepath Filepath
 * @return Vector of strings containing file contents
 */
std::vector<std::string> fRead (std::string filepath);
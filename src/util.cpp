#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

extern void exit_sim(int status);

// Bash color codes
const std::string  COLOR_RESET  = "\033[0m";
const std::string  COLOR_RED    =  "\033[31m";
const std::string  COLOR_GREEN  =  "\033[32m";
const std::string  COLOR_YELLOW =  "\033[33m";


void throwError(const std::string msg, bool exit)
{
    std::cerr << COLOR_RED <<"!ERROR: " << COLOR_RESET << msg << std::endl;
    if(exit)
    {
        exit_sim(EXIT_FAILURE);
    }
}

void throwWarning(const std::string msg)
{
    std::cerr << COLOR_YELLOW <<"!WARNING: " << COLOR_RESET << msg << std::endl;
}

void throwSuccessMessage(std::string msg, bool exit)
{
    std::cout << COLOR_GREEN <<"SUCCESS!: " << COLOR_RESET  << msg <<std::endl;
    if(exit)
    {
        exit_sim(EXIT_SUCCESS);
    }
}

// =============================== FILE READER =====================================
/**
 * @brief reads a binary file
 * 
 * @param memfile filepath
 * @return std::vector<char> contents
 */
std::vector<char> fReadBin(std::string memfile)
{        
    std::vector<char> fcontents;
    std::ifstream f (memfile, std::ios::out | std::ios::binary);
    
    if(!f)
    {
        throw "file access failed";
    }
    try
    {
        while(!f.eof())
        {    
            char byte;
            f.read((char *) &byte, 1);
            fcontents.push_back(byte);
        }
    }
    catch(...)
    {
        throw "file reading failed!";
    }
    f.close();
    return fcontents;
}

/**
 * @brief Reads a file and returns its contents
 * 
 * @param filepath Filepath
 * @return Vector of strings containing file contents
 */
std::vector<std::string> fRead (std::string filepath)
{
    // returns a vector of strings
    std::vector<std::string> text;

    // input file stream
    std::ifstream fin(filepath.c_str());
    if(!fin){
        throw "file access failed";
    }

    // reading file line by line and appending into the vector of strings
    std::string raw_line;
    while(getline(fin,raw_line))
    {
        text.push_back(raw_line);
    }

    // close file
    fin.close();
    return text;
}

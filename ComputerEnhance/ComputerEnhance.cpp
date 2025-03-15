// ComputerEnhance.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <bitset>
#include <array>
#include <utility> // for std::pair
#include <tuple>
#include <assert.h>

// git submodule add <repository_url> <path>

namespace fs = std::filesystem;

// This act as a simgle statement
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << #condition << "\n" \
                      << "Message: " << message << "\n" \
                      << "File: " << __FILE__ << "\n" \
                      << "Line: " << __LINE__ << std::endl; \
            std::abort(); \
        } \
    } while (false)

int main(int argc, char* argv[])
{
    // 1. Check for correct number of arguments
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <directory_name> <file_name> [optional_content]" << std::endl;
        return 1;
    }

    std::string directoryName = argv[1];
    std::string fileName = argv[2];
    std::string fileContent = "";

    // 2. Get file content (if provided)
    if (argc > 3) {
        for (int i = 3; i < argc; ++i) {
            fileContent += argv[i];
            if (i < argc - 1) fileContent += " "; // Add spaces between arguments
        }
    }
    else {
        fileContent = "Default content.  No arguments provided.";
    }


    // 3. Create directory (if it doesn't exist)
    if (!fs::exists(directoryName) || !fs::is_directory(directoryName)) {
        try {
            fs::create_directory(directoryName);
            std::cout << "Directory '" << directoryName << "' created." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error creating directory: " << e.what() << std::endl;
            return 1;
        }
    }

    // 4. Create and write to file
    std::string filePath = directoryName + "/" + fileName; // Use + for path concatenation
    //std::ofstream outputFile(filePath);

    // Open the binary file
    std::ifstream file(filePath, std::ios::binary);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file!" << std::endl;
        return 1;
    }

    // Get the size of the file
    file.seekg(0, std::ios::end); // Move to the end of the file
    std::streampos fileSize = file.tellg(); // Get the file size
    file.seekg(0, std::ios::beg); // Move back to the beginning of the file

    // Create a buffer to hold the file data
    std::vector<char> buffer(fileSize);

    // Read the entire file into the buffer
    file.read(buffer.data(), fileSize);

    // Check if the read operation was successful
    if (!file) {
        std::cerr << "Error: Failed to read the file!" << std::endl;
        return 1;
    }

    // Close the file
    file.close();

    // If the buffer will see 010001 because little endian, least significant bit at 0 index
    const std::bitset<6> moveOpcode("100010");
    const std::bitset<1> d("0"); // source destination
    std::bitset<1> w("1");

    std::bitset<2> modRegisterToRegister("11");
    const std::array<std::tuple<std::bitset<3>, std::string, std::string>, 8> registers{{
		            {std::bitset<3>("000")      ,"AL",      "AX"},
		            {std::bitset<3>("001")      ,"CL",      "CX"},
		            {std::bitset<3>("010")      ,"DL",      "DX"},
		            {std::bitset<3>("011")      ,"BL",      "BX"},
		            {std::bitset<3>("100")      ,"AH",      "SP"},
		            {std::bitset<3>("101")      ,"CH",      "BP"},
		            {std::bitset<3>("110")      ,"DH",      "SI"},
		            {std::bitset<3>("111")      ,"BH",      "DI"}
    }};

    // ( Opcode|| d(direction) || (w) word ) ( mod || reg || r/m )
    std::bitset<2> registerMod("11");
    
    printf("BITS 16\n");
    for (int i = 0; i < buffer.size();i++) {
        //std::cout << std::hex << static_cast<int>(buffer[i]) << " ";
        if (i % 2==0) {
			std::bitset<8> binary(buffer[i]);
            std::bitset<6> instructionOpcode = (binary.to_ulong() & 0b11111100) >> 2;
            ASSERT(moveOpcode == instructionOpcode, "This is not a MOV instruction");
            w = (binary.to_ulong() & 0b00000001);
        }
        else {
            std::bitset<8> binary(buffer[i]);
            const std::bitset<2> mod = (binary.to_ulong() & 0b11000000) >> 6;
            std::bitset<3> sourceReg = (binary.to_ulong() & 0b00111000) >> 3;
            std::bitset<3> desReg = (binary.to_ulong() & 0b00000111);
            // but assembly instruction is destination source
            std::string src = "";
            std::string dst = "";
            for (const std::tuple<std::bitset<3>
                , std::string
                , std::string>&reg 
                : registers) {
                auto [regCode, rIfwEqual0, rIfwEqual1] = reg;
                if (std::get<0>(reg) == sourceReg) {
                    src = w == std::bitset<1>("0")? rIfwEqual0: rIfwEqual1;
                }
                if (std::get<0>(reg) == desReg) {
                    dst = w == std::bitset<1>("0") ? rIfwEqual0 : rIfwEqual1;
                }
            }
            printf("MOV %s, %s\n", dst.c_str(), src.c_str());
        }
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

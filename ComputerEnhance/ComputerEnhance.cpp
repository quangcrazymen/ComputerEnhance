// ComputerEnhance.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <filesystem>
// git submodule add <repository_url> <path>

namespace fs = std::filesystem;

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
    std::ofstream outputFile(filePath);

    // Open the binary file
    std::ifstream file("example.bin", std::ios::binary);

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

    // Process the binary data (example: print the raw bytes)
    for (char byte : buffer) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
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

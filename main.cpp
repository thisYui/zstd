#include "zstd.h"

int main()
{
    while (true)
    {
        std::cout << "Choose your choice: \n";
        std::cout << "1. Compress\t\t";
        std::cout << "2. Decompress\t\t";
        std::cout << "3. Exit\n";
        std::cout << " > ";

        std::string input;
        std::cin >> input;

        // Kiểm tra đầu vào có phải số hay không
        int choice = 0;
        try
        {
            choice = std::stoi(input);
        }
        catch (const std::exception&)
        {
            std::cout << "Invalid choice\n";
            continue;
        }

        if (choice == 1)
        {
            std::cout << "Enter the file name to compress: ";
            std::string file_name;
            std::cin >> file_name;

            if (zstd::compress(file_name))
            {
                std::cout << "File compressed successfully\n";
            }
            else
            {
                std::cout << "---> Failed to compress file\n";
            }
        }
        else if (choice == 2)
        {
            std::cout << "Enter the file name to decompress: ";
            std::string file_name;
            std::cin >> file_name;

            if (zstd::decompress(file_name))
            {
                std::cout << "File decompressed successfully\n";
            }
            else
            {
                std::cout << "---> Failed to decompress file\n";
            }
        }
        else if (choice == 3)
        {
            std::cout << "Exiting...\n";
            break;
        }
        else
        {
            std::cout << "Invalid choice\n";
        }
    }

    return 0;
}
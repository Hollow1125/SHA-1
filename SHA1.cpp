#include "CryptoPP.h"
#include "IPP.h"
#include "WinAPI.h"
#include <iostream>
#include <future>

using namespace std;

int main(int argc, char* argv[])
{
    ifstream hash_file(argv[1], ios::binary);
    if (hash_file.is_open())
    {
        hash_file.seekg(0, ios::end);
        size_t file_size = hash_file.tellg();
        hash_file.seekg(0, ios::beg);

        size_t chunk = static_cast<size_t>(1) << 25;
        size_t number_of_chunks = file_size / chunk;
        size_t leftover = file_size % chunk;

        vector<vector<char>> buffer;
        vector<char> block(chunk, 0);

        for (int i = 0; i < number_of_chunks; i++)
        {
            hash_file.read(block.data(), chunk);
            buffer.push_back(block);
        }
        hash_file.read(block.data(), leftover);
        buffer.push_back(block);

        future<void> crypto_async = std::async(launch::async, &CryptoPP_hash, argv[1], &buffer, &file_size);
        cout << "Calculating hash..." << endl;
        crypto_async.get();

        cout << "**********************************************************************************************************" << endl;
        cout << "* Please clear the file from RAM (RAMMap -> Empty -> Empty standby list), then press any key to continue *" << endl;
        cout << "**********************************************************************************************************" << endl;
        cin.get();
        
        future<void> intel_future = std::async(launch::async, &IntelPP_hash, argv[1], &buffer, &file_size);
        cout << "Calculating hash..." << endl;
        intel_future.get();

        cout << "**********************************************************************************************************" << endl;
        cout << "* Please clear the file from RAM (RAMMap -> Empty -> Empty standby list), then press any key to continue *" << endl;
        cout << "**********************************************************************************************************" << endl;
        cin.get();

        future<void> cng_future = std::async(launch::async, &WinAPI_hash, argv[1], &buffer, &file_size);
        cout << "Calculating hash..." << endl;
        cng_future.get();
        
        hash_file.close();

        buffer.clear();
        buffer.shrink_to_fit();
    }
    else
    {
        cout << "Couldn't open file: " << argv[1] << endl;
    }

    return 0;
}

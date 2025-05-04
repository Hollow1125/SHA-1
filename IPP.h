#ifndef IPP_H
#define IPP_H

#pragma warning(disable : 4996)

#include <iostream>
#include <vector>
#include <cstdint>
#include <ippcp.h>
#include <ipp.h>
#include <fstream>
#include <chrono>
#include <iomanip>


using namespace std;
using namespace std::chrono;

void IntelPP_hash(const char* filename)
{
    ifstream hash_file(filename, ios::binary);
    if (hash_file.is_open())
    {
        hash_file.seekg(0, ios::end);
        size_t file_size = hash_file.tellg();
        hash_file.seekg(0, ios::beg);

        size_t chunk = static_cast<size_t>(1) << 25;
        size_t number_of_chunks = file_size / chunk;
        size_t leftover = file_size % chunk;
        vector<char> buffer(chunk, 0);

        const IppsHashMethod* pMethod = ippsHashMethod_SHA1();
        int buffer_size = 0;
        ippsHashGetSize_rmf(&buffer_size);
        IppsHashState_rmf* hash = (IppsHashState_rmf*)ippsMalloc_8u(buffer_size);
        ippsHashInit_rmf(hash, pMethod);  
        uint8_t digest[20];

        auto start = high_resolution_clock::now();
        for (int i = 0; i < number_of_chunks; i++)
        {
            hash_file.read(buffer.data(), chunk);
            ippsHashUpdate_rmf(reinterpret_cast<const Ipp8u*>(buffer.data()), chunk, hash);
        }
        hash_file.read(buffer.data(), leftover);
        ippsHashUpdate_rmf(reinterpret_cast<const Ipp8u*>(buffer.data()), leftover, hash);

        ippsHashFinal_rmf(digest, hash);
        auto stop = high_resolution_clock::now();

        // Очистка памяти
        ippsFree(hash);

        cout << "Intel IPP: " << endl;

        cout << "Hash sum of a file: ";
        for (auto i : digest)
        {
            cout << hex << setw(2) << setfill('0') << static_cast<int>(i);
        }
        cout << endl;

        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Digesting time: " << dec << duration.count() / 1e6 << " seconds" << endl;
        cout << "File size: " << dec << file_size / (1024 * 1024) << " MB" << endl;

        // Перевод B/mus в MB/s
        double speed = (file_size / 1e6) / (duration.count() / 1e6);
        cout << "Calculation speed: " << speed << " MB/s" << endl;

        hash_file.close();

        buffer.clear();
        buffer.shrink_to_fit();
        cout << endl;
    }
    else
    {
        cout << "Couldn't open file: " << filename << endl;
    }
};

#endif // IPP

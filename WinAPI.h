#ifndef WINAPI_H
#define WINAPI_H

#include <iostream>
//#include <iomanip>
#include <vector>
#include <cstdint>
#include <fstream>
#include <chrono>
#include <windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

#define SHA1LEN 20

using namespace std;
using namespace std::chrono;

void WinAPI_hash(const char* filename)
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

        // https://learn.microsoft.com/ru-ru/windows/win32/seccng/cng-cryptographic-primitive-functions

        BCRYPT_ALG_HANDLE hAlg = nullptr;
        BCRYPT_HASH_HANDLE hHash = nullptr;
        BYTE digest [SHA1LEN];

        BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA1_ALGORITHM, NULL, 0);
        BCryptCreateHash(hAlg, &hHash, NULL, NULL, 0, 0, 0);

        auto start = high_resolution_clock::now();

        for (int i = 0; i < number_of_chunks; i++)
        {
            hash_file.read(buffer.data(), chunk);
            BCryptHashData(hHash, reinterpret_cast<PUCHAR>(buffer.data()), static_cast<ULONG>(chunk), 0);
        }
        hash_file.read(buffer.data(), leftover);
        BCryptHashData(hHash, reinterpret_cast<PUCHAR>(buffer.data()), static_cast<ULONG>(leftover), 0);
        BCryptFinishHash(hHash, digest, SHA1LEN, 0);

        auto stop = high_resolution_clock::now();

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
        // Очистка heap
        buffer.shrink_to_fit();
        cout << endl;
    }
    else
    {
        cout << "Couldn't open file: " << filename << endl;
    }
};

#endif // WINAPI

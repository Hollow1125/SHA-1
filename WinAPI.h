#ifndef WINAPI_H
#define WINAPI_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <chrono>
#include <iomanip>
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
        NTSTATUS status;

        // Без return значений компилятор жалуется на игнорирование возвращаемого значения (С6031)
        status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA1_ALGORITHM, NULL, 0);
        if (status != 0)
        {
            cout << "Failed to open algorithm provider" << endl;
            return;
        }
        
        status = BCryptCreateHash(hAlg, &hHash, NULL, NULL, 0, 0, 0);
        if (status != 0)
        {
            cout << "Failed to create hash" << endl;
            return;
        }

        auto start = high_resolution_clock::now();

        for (int i = 0; i < number_of_chunks; i++)
        {
            hash_file.read(buffer.data(), chunk);
            status = BCryptHashData(hHash, reinterpret_cast<PUCHAR>(buffer.data()), static_cast<ULONG>(chunk), 0);
            if (status != 0)
            {
                cout << "Failed to digest chunk #" << i << endl;
                return;
            }
        }
        hash_file.read(buffer.data(), leftover);
        status = BCryptHashData(hHash, reinterpret_cast<PUCHAR>(buffer.data()), static_cast<ULONG>(leftover), 0);
        if (status != 0)
        {
            cout << "Failed to digest last chunk" << endl;
            return;
        }
        status = BCryptFinishHash(hHash, digest, SHA1LEN, 0);
        if (status != 0)
        {
            cout << "Failed to finish hash" << endl;
            return;
        }

        auto stop = high_resolution_clock::now();

        // Сворачивание алгоритма
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);

        cout << "Windows standard API: " << endl;

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

#endif // WINAPI

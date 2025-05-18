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

void WinAPI_hash(const char* filename, const vector<vector<char>>* buffer, const size_t* file_size)
{
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

        for (auto& block : *buffer)
        {
            status = BCryptHashData(hHash, const_cast<PUCHAR>(reinterpret_cast<const BYTE*>(block.data())), static_cast<ULONG>(block.size()), 0);
            if (status != 0)
            {
                cout << "Failed to digest chunk" << endl;
                return;
            }
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
        cout << "File size: " << dec << *file_size / (1024 * 1024) << " MB" << endl;

        // Перевод B/mus в MB/s
        double speed = (*file_size / 1e6) / (duration.count() / 1e6);
        cout << "Calculation speed: " << speed << " MB/s" << endl;

        cout << endl;
};

#endif // WINAPI

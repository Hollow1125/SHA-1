#ifndef CRYPTOPP_H
#define CRYPTOPP_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <cryptopp/sha.h>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <future>

using namespace std;
using namespace std::chrono;

void CryptoPP_hash(const char* filename, const vector<vector<char>> *buffer, const size_t *file_size)
{
        uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
        CryptoPP::SHA1 hash;

        auto start = high_resolution_clock::now();

        for (const auto &block : *buffer)
        {
            hash.Update(reinterpret_cast<const CryptoPP::byte*>(block.data()), block.size());
        }
        hash.Final(digest);

        auto stop = high_resolution_clock::now();

        cout << "CryptoPP: " << endl;
        cout << "Hash sum of a file: ";
        for (auto i : digest)
        {
            // Преобразование аналогично "chunk"
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

#endif // CRYPTOPP
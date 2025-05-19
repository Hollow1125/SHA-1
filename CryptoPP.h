/*
#ifndef CRYPTOPP_H
#define CRYPTOPP_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <cryptopp/sha.h>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <future>

using namespace std;
using namespace std::chrono;


void CryptoPP_hash(const char* filename)
{
    ifstream hash_file(filename, ios::binary);
    if (hash_file.is_open())
    {
        hash_file.seekg(0, ios::end);
        size_t file_size = hash_file.tellg();
        hash_file.seekg(0, ios::beg);

        // блоку "1 << 25" С++ неявно присваивает тип int
        // чтобы избежать переполнения нужно преобразовать в size_t
        size_t chunk = static_cast<size_t>(1) << 25;
        size_t number_of_chunks = file_size / chunk;
        size_t leftover = file_size % chunk;

        vector<char> buffer1(chunk, 0), buffer2(chunk, 0);
        bool current_buffer = true;

        mutex mtx;

        // Хэш из нескольких блоков данных
        // https://cryptopp.com/wiki/Hash_Functions
        // Расчет скорости:
        // https://www.geeksforgeeks.org/measure-execution-time-function-cpp/

        CryptoPP::SHA1 hash;

        future<void> hasher;

        auto start = high_resolution_clock::now();

        for (int i = 0; i < number_of_chunks; i++)
        {
            vector<char>& read_buf = current_buffer ? buffer1 : buffer2;
            vector<char>& hash_buf = current_buffer ? buffer2 : buffer1;

            hash_file.read(read_buf.data(), chunk);

            if (i > 0)
            {
                hasher.wait();
            }

            hasher = async(launch::async, [&hash, &hash_buf, &chunk, &mtx]()
                {
                    lock_guard<mutex> lock(mtx);
                    hash.Update(reinterpret_cast<const CryptoPP::byte*>(hash_buf.data()), chunk);
                });

            current_buffer = !current_buffer;
        }
        if (leftover > 0)
        {
            vector<char> read_buf = current_buffer ? buffer1 : buffer2;
            hash_file.read(read_buf.data(), leftover);
            hash.Update(reinterpret_cast<const CryptoPP::byte*>(read_buf.data()), leftover);
        }

        // Сборка всех кусков в один хэш
        uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
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
        cout << "File size: " << dec << file_size / (1024 * 1024) << " MB" << endl;

        // Перевод B/mus в MB/s
        double speed = (file_size / 1e6) / (duration.count() / 1e6);
        cout << "Calculation speed: " << speed << " MB/s" << endl;

        hash_file.close();

        cout << endl;
    }
    else
    {
        cout << "Couldn't open file: " << filename << endl;
    }
};

#endif // CRYPTOPP
*/

#ifndef CRYPTOPP_HASH_H
#define CRYPTOPP_HASH_H

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

void CryptoPP_hash(const char* filename)
{
    ifstream hash_file(filename, ios::binary);
    if (!hash_file.is_open())
    {
        cerr << "Couldn't open file: " << filename << endl;
        return;
    }

    // Get file size
    hash_file.seekg(0, ios::end);
    size_t file_size = static_cast<size_t>(hash_file.tellg());
    hash_file.seekg(0, ios::beg);

    // Chunk size: 32MB
    size_t chunk = static_cast<size_t>(1) << 25;
    size_t number_of_chunks = file_size / chunk;
    size_t leftover = file_size % chunk;

    // Double buffers
    vector<char> buffer1(chunk, 0), buffer2(chunk, 0);
    bool current_buffer = true;

    CryptoPP::SHA1 hash;
    future<void> hasher;

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < number_of_chunks; ++i)
    {
        vector<char>& read_buf = current_buffer ? buffer1 : buffer2;
        vector<char>& hash_buf = current_buffer ? buffer2 : buffer1;

        // Read next chunk into current buffer
        hash_file.read(read_buf.data(), chunk);

        // Wait for previous async hash update to complete
        if (hasher.valid())
            hasher.wait();

        // Launch hashing task on previous buffer
        hasher = async(launch::async, [&hash, &hash_buf, chunk]()
            {
                hash.Update(reinterpret_cast<const CryptoPP::byte*>(hash_buf.data()), chunk);
            });

        current_buffer = !current_buffer;
    }

    // Wait for the last hashing task
    if (hasher.valid())
        hasher.wait();

    // Process any leftover data (not full chunk)
    if (leftover > 0)
    {
        vector<char>& read_buf = current_buffer ? buffer1 : buffer2;
        hash_file.read(read_buf.data(), leftover);
        hash.Update(reinterpret_cast<const CryptoPP::byte*>(read_buf.data()), leftover);
    }

    // Finalize SHA-1 hash
    uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
    hash.Final(digest);
    auto stop = high_resolution_clock::now();

    // Output results
    cout << "CryptoPP (SHA-1):" << endl;
    cout << "Hash sum of the file: ";
    for (auto byte : digest)
    {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(byte);
    }
    cout << endl;

    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Digesting time: " << fixed << setprecision(3) << duration.count() / 1e6 << " seconds" << endl;
    cout << "File size: " << file_size / (1024 * 1024) << " MB" << endl;

    double speed = (file_size / 1e6) / (duration.count() / 1e6);
    cout << "Calculation speed: " << speed << " MB/s" << endl << endl;

    hash_file.close();
}

#endif // CRYPTOPP_HASH_H

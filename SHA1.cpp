#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cryptopp/sha.h>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

void calculate_and_print_digest()
{
    ifstream hash_file("D:\\VMs\\GOAD-DC01.ova", ios::binary);
    if (hash_file.is_open())
    {
        hash_file.seekg(0, ios::end);
        size_t file_size = hash_file.tellg();
        hash_file.seekg(0, ios::beg);

        size_t chunk = 1 << 25;
        size_t number_of_chunks = file_size / chunk;
        size_t leftover = file_size % chunk;
        //size_t orig = (number_of_chunks * chunk) + leftover;
        //size_t pos = 0;
        vector<char> buffer(chunk, 0);
        
        // Хэш из нескольких блоков данных
        // https://cryptopp.com/wiki/Hash_Functions
        // Расчет скорости:
        // https://www.geeksforgeeks.org/measure-execution-time-function-cpp/

        uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
        CryptoPP::SHA1 hash;

        auto start = high_resolution_clock::now();
        for (int i = 0; i < number_of_chunks; i++)
        {
            hash_file.read(buffer.data(), chunk);
            hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), chunk);
        }
        hash_file.read(buffer.data(), leftover);
        hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), chunk);
        hash.Final(digest);
        auto stop = high_resolution_clock::now();
        
        cout << "Hash sum of a file: ";
        for (auto i : digest)
        {
            cout << hex << setw(2) << setfill('0') << int(i);
        }
        cout << endl;

        // Если задать duration в секундах, то он выдаст 0 (время расчета меньше секунды)
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Digesting time: " << dec << duration.count() / 1e6 << " seconds" << endl;
        cout << "File size: " << dec << file_size / (1024 * 1024) << " MB" << endl;

        // Перевод B/mus в MB/s
        double speed = (file_size / 1e6) / (duration.count() / 1e6);
        cout << "Calculation speed: " << speed << " MB/s" << endl;

        cout << endl;
    };
};

int main()
{
    calculate_and_print_digest();
    return 0;
}




/*
 // Hash in chunks to avoid memory issues
    const size_t BUFFER_SIZE = 1 << 20;  // 1 MB == 00000000 00010000 00000000 00000000
    vector<char> buffer(BUFFER_SIZE);
    uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
    CryptoPP::SHA1 hash;

    auto start = high_resolution_clock::now();
    while (hash_file.read(buffer.data(), BUFFER_SIZE)) {
        hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), hash_file.gcount());
    }
    hash.Final(digest);
    auto stop = high_resolution_clock::now();
*/
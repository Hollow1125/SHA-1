#ifndef CRYPTOPP_H
#define CRYPTOPP_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cryptopp/sha.h>
#include <fstream>
#include <chrono>


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

        // ����� "1 << 25" �++ ������ ����������� ��� int
        // ����� �������� ������������ ����� ������������� � size_t
        size_t chunk = static_cast<size_t>(1) << 25;
        size_t number_of_chunks = file_size / chunk;
        size_t leftover = file_size % chunk;
        vector<char> buffer(chunk, 0);

        // ��� �� ���������� ������ ������
        // https://cryptopp.com/wiki/Hash_Functions
        // ������ ��������:
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
        hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), leftover);

        // ������ ���� ������ � ���� ���
        hash.Final(digest);
        auto stop = high_resolution_clock::now();

        cout << "Hash sum of a file: ";
        for (auto i : digest)
        {
            // �������������� ���������� "chunk"
            cout << hex << setw(2) << setfill('0') << static_cast<int>(i);
        }
        cout << endl;

        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Digesting time: " << dec << duration.count() / 1e6 << " seconds" << endl;
        cout << "File size: " << dec << file_size / (1024 * 1024) << " MB" << endl;

        // ������� B/mus � MB/s
        double speed = (file_size / 1e6) / (duration.count() / 1e6);
        cout << "Calculation speed: " << speed << " MB/s" << endl;

        hash_file.close();
        
        buffer.clear();
        // ������� heap
        buffer.shrink_to_fit();
        cout << endl;
    }
    else
    {
        cout << "Couldn't open file: " << filename << endl;
    }
};

#endif // CRYPTOPP
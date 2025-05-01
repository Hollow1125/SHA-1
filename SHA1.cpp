#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cryptopp/sha.h>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

//Библиотека cryptoPP была подключена для расчета скорости вычисления хэш-суммы
//заданного мною файла и оценки производительности данной бибилиотеки
void calculate_and_print_digest()
{
    ifstream hash_file("D:\\GOAD-SRV03.ova", ios::binary);
    if (hash_file.is_open())
    {
        hash_file.seekg(0, ios::end);
        int file_size = hash_file.tellg();
        hash_file.seekg(0, ios::beg);

        vector<char> hash_data(file_size, 0);
        hash_file.read(hash_data.data(), file_size);


        uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
        CryptoPP::SHA1 hash;

        // Расчет скорости:
        // https://www.geeksforgeeks.org/measure-execution-time-function-cpp/
        auto start = high_resolution_clock::now();
        hash.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(hash_data.data()), hash_data.size());
        auto stop = high_resolution_clock::now();

        cout << "Hash sum of a file: ";
        for (auto i : digest)
        {
            cout << hex << setw(2) << setfill('0') << int(i);
        }
        cout << endl;
        cout << "ferff" << endl;
        // Если задать duration в секундах, то он выдаст 0 (время расчета меньше секунды)
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Digesting time: " << dec << duration.count() / 1e6 << " seconds" << endl;
        //cout << dec << file_size << " bytes" << endl;

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
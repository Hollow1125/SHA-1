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


#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>



using namespace std;
using namespace std::chrono;



queue<vector<char>> buffer_queue;
mutex queue_mutex;
condition_variable queue_cv;
bool hashing_done = false;


void hashing_worker(CryptoPP::SHA1& hash) {
    while (true) {
        vector<char> buffer;
        {
            unique_lock<mutex> lock(queue_mutex);
            queue_cv.wait(lock, [] { return !buffer_queue.empty() || hashing_done; });

            if (buffer_queue.empty() && hashing_done)
                break;

            buffer = move(buffer_queue.front());
            buffer_queue.pop();
        }
        hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size());
    }
}


void CryptoPP_hash(const char* filename)
{
    ifstream hash_file(filename, ios::binary);

    hash_file.seekg(0, ios::end);
    size_t file_size = static_cast<size_t>(hash_file.tellg());
    hash_file.seekg(0, ios::beg);

    size_t chunk = static_cast<size_t>(1) << 25;
    size_t number_of_chunks = file_size / chunk;
    size_t leftover = file_size % chunk;

    vector<vector<char>> buffers(3, vector<char>(chunk, 0));
    //vector<char> big_buffer(chunk + leftover, 0);

    uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
    CryptoPP::SHA1 hash;

    future<void> hasher;

    thread worker(hashing_worker, ref(hash));

    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < number_of_chunks; ++i)
    {
        size_t current_buffer = i % buffers.size();

            hash_file.read(buffers[current_buffer].data(), chunk);


        // Чтение предыдущего чанка
            if (i > 0)
            {
                size_t previous_buffer = (i - 1) % buffers.size();
                {
                    lock_guard<mutex> lock(queue_mutex);
                    buffer_queue.push(buffers[previous_buffer]); // copy or move
                }
                queue_cv.notify_one();
            }
            else
            {
                continue;
            }
    }
    {
        lock_guard<mutex> lock(queue_mutex);
        hashing_done = true;
    }
    queue_cv.notify_one();
    worker.join();

    size_t last_chunk = (number_of_chunks - 1) % buffers.size();
    hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffers[last_chunk].data()), chunk);
    
    if (leftover > 0)
    {
        vector<char> buffer(leftover, 0);
        hash_file.read(buffer.data(), leftover);
        hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), leftover);
    }
    
    hash.Final(digest);
    auto stop = high_resolution_clock::now();

    cout << "CryptoPP:" << endl;
    cout << "Hash sum of the file: ";
    for (auto byte : digest)
    {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(byte);
    }
    cout << endl;

    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Digesting time: " << fixed << setprecision(3) << duration.count() / 1e6 << " seconds" << endl;
    cout << "File size: " << dec << file_size / (1024 * 1024) << " MB" << endl;

    double speed = (file_size / 1e6) / (duration.count() / 1e6);
    cout << "Calculation speed: " << speed << " MB/s" << endl << endl;

    hash_file.close();
}

#endif // CRYPTOPP_HASH_H

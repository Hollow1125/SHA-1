#include "CryptoPP.h"
#include "IPP.h"
#include "WinAPI.h"
#include <iostream>
#include <future>

using namespace std;

// Реализовать паттерн Producer-Consumer для многопоточности

int main(int argc, char* argv[])
{
   // future<void> crypto_async = std::async(launch::async, &CryptoPP_hash, argv[1]);
   // cout << "Calculating hash..." << endl;
   // crypto_async.get();
    CryptoPP_hash(argv[1]);
    cout << "******************************************************************************************************" << endl;
    cout << "Please clear the file from RAM (RAMMap -> Empty -> Empty Standby List), then press any key to continue" << endl;
    cout << "******************************************************************************************************" << endl;
    cin.get();

    //future<void> intel_future = std::async(launch::async, &IntelPP_hash, argv[1]);
    //cout << "Calculating hash..." << endl;
    //intel_future.get();
    IntelPP_hash(argv[1]);
    cout << "******************************************************************************************************" << endl;
    cout << "Please clear the file from RAM (RAMMap -> Empty -> Empty Standby List), then press any key to continue" << endl;
    cout << "******************************************************************************************************" << endl;
    cin.get();
    future<void> cng_future = std::async(launch::async, &WinAPI_hash, argv[1]);
    cout << "Calculating hash..." << endl;
    cng_future.get();
    return 0;
}

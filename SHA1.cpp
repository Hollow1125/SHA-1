#include "CryptoPP.h"
#include "IPP.h"
#include "WinAPI.h"
#include <iostream>


int main(int argc, char* argv[])
{
    cout << "CryptoPP: " << endl;
    CryptoPP_hash(argv[1]);
    cout << "*******************************************************" << endl;
    cout << "Please clear the file from RAM (e.g. with RAMMap), then press any key to continue" << endl;
    cout << "*******************************************************" << endl;
    cin.get();
    cout << "IPP: " << endl;
    IntelPP_hash(argv[1]);
    cout << "*******************************************************" << endl;
    cout << "Please clear the file from RAM (e.g. with RAMMap), then press any key to continue" << endl;
    cout << "*******************************************************" << endl;
    cin.get();
    cout << "Windows standard API: " << endl;
    WinAPI_hash(argv[1]);
    return 0;
}

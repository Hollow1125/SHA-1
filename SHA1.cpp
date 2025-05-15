#include "CryptoPP.h"
#include "IPP.h"
#include "WinAPI.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    CryptoPP_hash(argv[1]);
    cout << "*********************************************************************************" << endl;
    cout << "Please clear the file from RAM (e.g. with RAMMap), then press any key to continue" << endl;
    cout << "*********************************************************************************" << endl;
    cin.get();
    IntelPP_hash(argv[1]);
    cout << "*********************************************************************************" << endl;
    cout << "Please clear the file from RAM (e.g. with RAMMap), then press any key to continue" << endl;
    cout << "*********************************************************************************" << endl;
    cin.get();
    WinAPI_hash(argv[1]);
    return 0;
}

# SHA-1

This program uses several C++ libraries digestion functions to calculate hashes using SHA-1 algorithm and evaluate their performance level.
Libraries tested:
1. CryptoPP
2. Intel IPP
3. Standard Windows API (CNG)

## Dependencies

To compile this program on your own PC you'll need:

### CryptoPP library:
```
https://www.cryptopp.com/#download
```
or
```
https://github.com/weidai11/cryptopp
```
### Windows SDK:
```
https://developer.microsoft.com/ru-ru/windows/downloads/windows-sdk/
```
### Intel IPP:
Base toolkit:
```
https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html
```
Standalone version:
```
https://www.intel.com/content/www/us/en/developer/tools/oneapi/ipp-download.html
```

## Usage

To launch this programm head to ...\SHA1\x64\Debug or add it to your system variables (PATH). You will also need RAMMap to flush memory inbetween functions.
```
./SHA1.exe <\path\to\file>
```
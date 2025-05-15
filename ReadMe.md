# SHA-1

Данная программа использует несколько дополнительных библиотек для расчета хэш-суммы по алгоритму SHA-1.
Использованные библиотеки:
1. CryptoPP
2. Intel IPP
3. Standard Windows API (CNG)

## CryptoPP

Crypto++ (также известная как CryptoPP, libcrypto++ и libcryptopp) — это бесплатная библиотека C++ с открытым исходным кодом криптографических алгоритмов и схем, написанная китайским компьютерным инженером Вэй Даем. 
Будучи выпущенной в 1995, библиотека полностью поддерживает 32-разрядные и 64-разрядные архитектуры для многих главных операционных систем и платформ, таких как Android (с использованием STLport), Apple (Mac OS X и iOS), Linux, MinGW, Solaris, Windows и др. 
Проект также поддерживает компиляцию с использованием библиотек различных сред выполнения C++03, C++11 и C++17; и множество других компиляторов и IDE,

Документация: [link](https://cryptopp.com/wiki/Hash_Functions)

### Линковка

Для линковки библиотеки в MS Visual Studio сперва необходимо скачать ее:

[link](https://www.cryptopp.com/#download)
или
[link](https://github.com/weidai11/cryptopp)

После установки библиотеку необходимо статически собрать. В Visual Studio нужно открыть файл проекта библиотеки `cryptest.sln`,
затем перейти в его свойства конфигурации и выбрать в окне *Конфигурация* **Debug** или **Release**, затем в разделе **C/C++ -> Создание кода -> Библиотека времени выполнения** выбрать `/MT` и собрать проект.
В результате получим файл `cryptlib.lib`.

Далее в настройках нашего проекта: 
- В разделе **C/C++ -> Общие -> Дополнительные каталоги включаемых файлов** добавить путь к собранной библиотеке
- В разделе **Компоновщик -> Ввод -> Дополнительные зависимости** вписать `cryptlib.lib`
- В разделе **Компоновщик -> Общие -> Дополнительные каталоги библиотек** добавить путь `.../cryptopp/x64/Output/Debug` (или `.../cryptopp/x64/Output/Release`, смотря как библиотека была собрана)

### Описание работы библиотеки

Реализация функции `CryptoPP_hash` с передачей имени файла в качества аргумента
```
void CryptoPP_hash(const char* filename)
```
Открытие файла в бинарном формате
```
ifstream hash_file(filename, ios::binary);
```
Считывание размера файла
```
hash_file.seekg(0, ios::end);
size_t file_size = hash_file.tellg();
hash_file.seekg(0, ios::beg);
```
Создание блоков (чанков), по которым будет происходит считывание файла
```
size_t chunk = static_cast<size_t>(1) << 25; // 00000010 00000000 00000000 00000000 = 32 Мб
size_t number_of_chunks = file_size / chunk; // Количество блоков для обхода в цикле
size_t leftover = file_size % chunk; // Остаток от деления на блоки
vector<char> buffer(chunk, 0); // Инициализация вектора для считывания файла
```
Определение размера хэш-суммы, а также определение способа расчета этой суммы
```
uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
CryptoPP::SHA1 hash;
```
Старт замера времени с использованием библиотек `chrono` и `iomanip`
```
auto start = high_resolution_clock::now();
```
Цикл для последовательного считывания и расчета хэш-суммы каждого из блоков
```
 for (int i = 0; i < number_of_chunks; i++)
 {
     hash_file.read(buffer.data(), chunk);
     hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), chunk);
 }
```
Расчет хэш-суммы для остатка
```
 hash_file.read(buffer.data(), leftover);
 hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), leftover);
```
Сборка всех частей хэш-суммы в одну цельную последовательность
```
 hash.Final(digest);
```
Остановка счета времени
```
auto stop = high_resolution_clock::now();
```
Вывод хэш-суммы с установкой длины каждого фрагмента - 2 (для корректного отображения последовательности ввиду устройства алгоритма) и заполнением пуустых пространств нулями
```
for (auto i : digest)
{
    cout << hex << setw(2) << setfill('0') << static_cast<int>(i);
}
cout << endl;
```
Расчет прошедшего времени:
```
auto duration = duration_cast<microseconds>(stop - start);
```
Подсчет скорости (деление на 1е6 нужно для перевода единиц из байтов в микросекунду в мегабайты в секунду):
```
double speed = (file_size / 1e6) / (duration.count() / 1e6);
```
Закрытие файла:
```
hash_file.close();
```
Очистка буфера
```
buffer.clear();
buffer.shrink_to_fit();
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
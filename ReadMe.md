# SHA-1

Данная программа использует несколько дополнительных библиотек для расчета хэш-суммы по алгоритму SHA-1.
Использованные библиотеки:
1. CryptoPP
2. Intel IPP
3. Standard Windows API (CNG)

## Функционал

Открытие файла в бинарном формате:
```
ifstream hash_file(filename, ios::binary);
```
Считывание размера файла:
```
hash_file.seekg(0, ios::end);
size_t file_size = hash_file.tellg();
hash_file.seekg(0, ios::beg);
```
Создание блоков (чанков), по которым будет происходит считывание файла:
```
size_t chunk = static_cast<size_t>(1) << 25; // 00000010 00000000 00000000 00000000 = 32 Мб
size_t number_of_chunks = file_size / chunk; // Количество блоков для обхода в цикле
size_t leftover = file_size % chunk; // Остаток от деления на блоки
```
Создание вектора, в который будет идти запись:
```
vector<vector<char>> buffer;
vector<char> block(chunk, 0);
```	
Цикл для последовательного считывания файла по блокам:
```
 for (int i = 0; i < number_of_chunks; i++)
{
    hash_file.read(block.data(), chunk);
    buffer.push_back(block);
}
 hash_file.read(block.data(), leftover);
 buffer.push_back(block);
```
Вызов функции `CryptoPP_hash` с делением на потоки:
```
future<void> crypto_async = std::async(launch::async, &CryptoPP_hash, argv[1], &buffer, &file_size);
cout << "Calculating hash..." << endl;
crypto_async.get();
```
Вызов функции `IntelPP_hash` с делением на потоки:
```
future<void> intel_future = std::async(launch::async, &IntelPP_hash, argv[1], &buffer, &file_size);
cout << "Calculating hash..." << endl;
intel_future.get();
```
Вызов функции `WinAPI_hash` с делением на потоки:
```
future<void> cng_future = std::async(launch::async, &WinAPI_hash, argv[1], &buffer, &file_size);
cout << "Calculating hash..." << endl;
cng_future.get();
```
Закрытие файла:
```
hash_file.close();
```
Очистка буфера:
```
buffer.clear();
buffer.shrink_to_fit();
```

**Важное замечание:** Несмотря на то, что функции вызываются в многопоточном режиме, сам алгоритм SHA-1 не поддерживает деление на потоки: расчет хэша для следующего блока данных невозможен, пока не завершится расчет для предыдущего.

## CryptoPP_hash

**Crypto++** (также известная как CryptoPP, libcrypto++ и libcryptopp) — это бесплатная библиотека C++ с открытым исходным кодом криптографических алгоритмов и схем, написанная китайским компьютерным инженером Вэй Даем. 
Будучи выпущенной в 1995, библиотека полностью поддерживает 32-разрядные и 64-разрядные архитектуры для многих главных операционных систем и платформ, таких как Android (с использованием STLport), Apple (Mac OS X и iOS), Linux, MinGW, Solaris, Windows и др. 
Проект также поддерживает компиляцию с использованием библиотек различных сред выполнения C++03, C++11 и C++17; и множество других компиляторов и IDE,

Документация: [https://cryptopp.com/wiki/Hash_Functions](https://cryptopp.com/wiki/Hash_Functions)

### Линковка

Для линковки библиотеки в MS Visual Studio сперва необходимо скачать ее:

[https://www.cryptopp.com/#download](https://www.cryptopp.com/#download)

или

[https://github.com/weidai11/cryptopp](https://github.com/weidai11/cryptopp)

После установки библиотеку необходимо статически собрать. В Visual Studio нужно открыть файл проекта библиотеки `cryptest.sln`,
затем перейти в его свойства конфигурации и выбрать в окне **Конфигурация -> Release**, затем в разделе **C/C++ -> Создание кода -> Библиотека времени выполнения** выбрать `/MT` и собрать проект.
В результате получим файл `cryptlib.lib`.

Далее в настройках нашего проекта: 
- В разделе **C/C++ -> Общие -> Дополнительные каталоги включаемых файлов** добавьте путь к собранной библиотеке
- В разделе **Компоновщик -> Ввод -> Дополнительные зависимости** добавьте `cryptlib.lib`
- В разделе **Компоновщик -> Общие -> Дополнительные каталоги библиотек** добавьте путь `.../cryptopp/x64/Output/Release`

### Описание работы библиотеки

Реализация функции `CryptoPP_hash` с передачей имени файла, вектора данных и размера файла в качества аргументов:
```
void CryptoPP_hash(const char* filename, const vector<vector<char>> *buffer, const size_t *file_size)
```

Определение размера хэш-суммы, а также определение способа расчета этой суммы:
```
uint8_t digest[CryptoPP::SHA1::DIGESTSIZE];
CryptoPP::SHA1 hash;
```
Старт замера времени с использованием библиотек `chrono` и `iomanip`:
```
auto start = high_resolution_clock::now();
```
Цикл для обхода вектора и расчета хэш-суммы каждого из блоков:
```
for (const auto &block : *buffer)
{
	hash.Update(reinterpret_cast<const CryptoPP::byte*>(block.data()), block.size());
}
```
Сборка всех частей хэш-суммы в одну цельную последовательность:
```
hash.Final(digest);
```
Остановка счета времени:
```
auto stop = high_resolution_clock::now();
```
Вывод хэш-суммы с установкой длины каждого фрагмента - 2 (для корректного отображения последовательности ввиду устройства алгоритма) и заполнением пустых пространств нулями:
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

---

## IntelPP_hash

**Intel Integrated Performance Primitives (IPP)** — это высокооптимизированная библиотека для обработки сигналов, изображений, видео, криптографии и других задач на базе SIMD-инструкций от Intel. Библиотека предоставляет готовые функции для быстрого выполнения операций, включая хэш-функции, такие как SHA1, SHA256 и др.
Библиотека предназначена для платформ на базе x86/x64, поддерживает компиляторы Visual Studio, GCC, Clang, а также работает на Windows, Linux и macOS.

Документация: [https://www.intel.com/content/www/us/en/developer/tools/oneapi/ipp-documentation.html](https://www.intel.com/content/www/us/en/developer/tools/oneapi/ipp-documentation.html)

### Линковка

Скачайте и установите Intel oneAPI Base Toolkit:

	[https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html)

В Visual Studio:
- В разделе **C/C++ -> Общие -> Дополнительные каталоги включаемых файлов** добавьте путь к заголовочным файлам IPP:  

     `C:\Program Files (x86)\Intel\oneAPI\ipp\2022.1\include`
	 
	 `C:\Program Files (x86)\Intel\oneAPI\ippcp\2025.1\include`
	 
- В разделе **Компоновщик -> Ввод -> Дополнительные зависимости** укажите путь к `.lib` файлам:  

     `C:\Program Files (x86)\Intel\oneAPI\ipp\2022.1\lib`
	 
	 `C:\Program Files (x86)\Intel\oneAPI\ippcp\2025.1\lib`
	 
- В разделе **Компоновщик -> Общие -> Дополнительные каталоги библиотек** добавьте:
     ```
	ippcoremt.lib
	ippsmt.lib
	ippcpmt.lib
     ```

### Описание работы библиотеки

Реализация функции `IntelPP_hash` с передачей имени файла, вектора данных и размера файла в качества аргументов:
```
void IntelPP_hash(const char* filename, const vector<vector<char>>* buffer, const size_t* file_size)
```

Указание метода расчета хэш-суммы через указатель на структуру, содержащую различные методы:
```
const IppsHashMethod* pMethod = ippsHashMethod_SHA1();
```
Вызов функции из библиотеки для определения размера выделяемой памяти для структуры состояния хэша:
```
 int buffer_size = 0;
 ippsHashGetSize_rmf(&buffer_size);
```
Выделения памяти для данной структуры состояния:
```
 IppsHashState_rmf* hash = (IppsHashState_rmf*)ippsMalloc_8u(buffer_size); 
```
Инициализация хэша для связи метода со структурой состояния
```
 ippsHashInit_rmf(hash, pMethod); 
```
Длина хэш-суммы:
```
uint8_t digest[20];
```
Старт замера времени с использованием библиотек `chrono` и `iomanip`:
```
auto start = high_resolution_clock::now();
```
Цикл для последовательного считывания вектора и расчета хэш-суммы каждого из блоков:
```
for (const auto& block : *buffer)
{
	ippsHashUpdate_rmf(reinterpret_cast<const Ipp8u*>(block.data()), block.size(), hash);
}
```
Сборка всех частей хэш-суммы в одну цельную последовательность:
```
ippsHashFinal_rmf(digest, hash);
```
Остановка счета времени:
```
auto stop = high_resolution_clock::now();
```
Очистка состояния хэша:
```
ippsFree(hash);
```
Вывод хэш-суммы с установкой длины каждого фрагмента - 2 (для корректного отображения последовательности ввиду устройства алгоритма) и заполнением пустых пространств нулями:
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

---

## WinAPI_hash

**Cryptography API: Next Generation (CNG)** — это современный криптографический интерфейс, встроенный в Windows, предоставляющий расширенный доступ к безопасным алгоритмам шифрования, хеширования и управления ключами. 
Он пришёл на смену устаревшему CryptoAPI и является основой безопасности в системах Windows начиная с Windows Vista / Windows Server 2008.

Документация: [https://learn.microsoft.com/ru-ru/windows/win32/seccng/cng-cryptographic-primitive-functions](https://learn.microsoft.com/ru-ru/windows/win32/seccng/cng-cryptographic-primitive-functions)

### Линковка

Скачайте `Windows SDK`: [https://developer.microsoft.com/ru-ru/windows/downloads/windows-sdk/](https://developer.microsoft.com/ru-ru/windows/downloads/windows-sdk/)

Поскольку API встроен в Windows процесс линковки библиотеки сильно проще предыдущих:
- Добавьте в проект следующие библиотеки:
	```
	#include <windows.h>
	#include <bcrypt.lib>
	```
- Добавьте линковку:
	```
	#pragma comment(lib, "bcrypt.lib")
	```
	
### Описание работы библиотеки

Определение длины хэш-суммы алгоритма SHA-1:
```
#define SHA1LEN 20
```
Реализация функции `IntelPP_hash` с передачей имени файла, вектора данных и размера файла в качества аргументов:
```
void WinAPI_hash(const char* filename, const vector<vector<char>>* buffer, const size_t* file_size)
```
Инициализация пустого дескриптора для идентификации алгоритма:
```
BCRYPT_ALG_HANDLE hAlg = nullptr;
```
Дескриптор хэш-объекта:
```
BCRYPT_HASH_HANDLE hHash = nullptr;
```
Массив, в который будет записан хэш:
```
BYTE digest[SHA1LEN];
```
Статус возвращаемых функций (нужен исключительно для определения этапа, на котором произошла ошибка):
```
NTSTATUS status;
```
Открытие алгоритма:
```
status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA1_ALGORITHM, NULL, 0);
if (status != 0)
{
	cout << "Failed to open algorithm provider" << endl;
	return;
}
```
Создание хэша:
```
status = BCryptCreateHash(hAlg, &hHash, NULL, NULL, 0, 0, 0);
if (status != 0)
{
	cout << "Failed to create hash" << endl;
	return;
}
```
Начало замера времени:
```
auto start = high_resolution_clock::now();
```
Цикл для последовательного считывания и расчета хэш-суммы каждого из блоков:
```
 for (auto& block : *buffer)
 {
     status = BCryptHashData(hHash, const_cast<PUCHAR>(reinterpret_cast<const BYTE*>(block.data())), static_cast<ULONG>(block.size()), 0);
     if (status != 0)
     {
         cout << "Failed to digest chunk" << endl;
         return;
     }
 }
```
Сборка всех частей хэш-суммы в одну цельную последовательность:
```
status = BCryptFinishHash(hHash, digest, SHA1LEN, 0);
if (status != 0)
{
	cout << "Failed to finish hash" << endl;
	return;
}
```
Остановка счета времени:
```
auto stop = high_resolution_clock::now();
```
Очистка ресурсов:
```
BCryptDestroyHash(hHash);
BCryptCloseAlgorithmProvider(hAlg, 0);
```
Вывод хэш-суммы с установкой длины каждого фрагмента - 2 (для корректного отображения последовательности ввиду устройства алгоритма) 
и заполнением пустых пространств нулями:
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

---
## Запуск программы

Для запуска программы откройте командную строку и перейдите в каталог с исполняемым файлом: `...\SHA1\x64\Release\`.
Вам также потребуется использовать утилиту **RAMMap** для очистки памяти между вызовами функций (`Empty -> Empty StandBy List`)
```
SHA1.exe <\path\to\file>
```

## Сравнение результатов

После запуска програмы для подсчета хэш-суммы файлас расширением `.rar` и размером 13.4 ГБ были получены следующие результаты:

**CryptoPP**
```
Hash sum of a file: 4f7d253ca17eb74d1d78cad80bfe615a6f893031
Digesting time: 7.18752 seconds
File size: 13723 MB
Calculation speed: 2002.14 MB/s
```
**Intel IPP**
```
Hash sum of a file: 4f7d253ca17eb74d1d78cad80bfe615a6f893031
Digesting time: 13.243 seconds
File size: 13723 MB
Calculation speed: 1086.64 MB/s
```
**Windows standard API**
```
Hash sum of a file: 4f7d253ca17eb74d1d78cad80bfe615a6f893031
Digesting time: 18.8014 seconds
File size: 13723 MB
Calculation speed: 765.39 MB/s
```

Из полученных результатов можно сделать вывод о том, что CryptoPP библиотека наиболее быстро способно рассчитать хэш-сумму по алгоритму SHA-1. Библиотека CNG хоть и расчитывает сумму медленнее остальных, однако наиболее просто подключается к проекту.
Однако стоит отметить, стандартный встроенный API поставляется в комплекте с Windows Developer Kit (SDK) и имеет объем ~10ГБ, что может не подойти для систем с ограниченным дисковым пространством.
# SHA-1

Данная программа использует несколько дополнительных библиотек для расчета хэш-суммы по алгоритму SHA-1.
Использованные библиотеки:
1. CryptoPP
2. Intel IPP
3. Standard Windows API (CNG)

## CryptoPP

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

Реализация функции `CryptoPP_hash` с передачей имени файла в качества аргумента:
```
void CryptoPP_hash(const char* filename)
```
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
vector<char> buffer(chunk, 0); // Инициализация вектора для считывания файла
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
Цикл для последовательного считывания и расчета хэш-суммы каждого из блоков:
```
 for (int i = 0; i < number_of_chunks; i++)
 {
     hash_file.read(buffer.data(), chunk);
     hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), chunk);
 }
```
Расчет хэш-суммы для остатка:
```
 hash_file.read(buffer.data(), leftover);
 hash.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), leftover);
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
Закрытие файла:
```
hash_file.close();
```
Очистка буфера:
```
buffer.clear();
buffer.shrink_to_fit();
```

---

## Intel IPP

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

Реализация функции `IntelPP_hash` с передачей имени файла в качестве аргумента:
```
void IntelPP_hash(const char* filename)
```
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
vector<char> buffer(chunk, 0); // Инициализация вектора для считывания файла
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
Цикл для последовательного считывания и расчета хэш-суммы каждого из блоков:
```
 for (int i = 0; i < number_of_chunks; i++)
 {
    hash_file.read(buffer.data(), chunk);
    ippsHashUpdate_rmf(reinterpret_cast<const Ipp8u*>(buffer.data()), chunk, hash);
 }
```
Расчет хэш-суммы для остатка:
```
 hash_file.read(buffer.data(), leftover);
 ippsHashUpdate_rmf(reinterpret_cast<const Ipp8u*>(buffer.data()), leftover, hash);
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
Закрытие файла:
```
hash_file.close();
```
Очистка буфера:
```
buffer.clear();
buffer.shrink_to_fit();
```

---

## Windows SDK

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
Реализация функции `IntelPP_hash` с передачей имени файла в качестве аргумента:
```
void WinAPI_hash(const char* filename)
```
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
vector<char> buffer(chunk, 0); // Инициализация вектора для считывания файла
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
for (int i = 0; i < number_of_chunks; i++)
{
	hash_file.read(buffer.data(), chunk);
	status = BCryptHashData(hHash, reinterpret_cast<PUCHAR>(buffer.data()), static_cast<ULONG>(chunk), 0);
	if (status != 0)
	{
		cout << "Failed to digest chunk #" << i << endl;
		return;
	}
}
```
Расчет хэш-суммы для остатка:
```
 hash_file.read(buffer.data(), leftover);
 status = BCryptHashData(hHash, reinterpret_cast<PUCHAR>(buffer.data()), static_cast<ULONG>(leftover), 0);
 if (status != 0)
 {
     cout << "Failed to digest last chunk" << endl;
     return;
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
Закрытие файла:
```
hash_file.close();
```
Очистка буфера:
```
buffer.clear();
buffer.shrink_to_fit();
```

---
## Запуск программы

Для запуска программы откройте командную строку и перейдите в каталог с исполняемым файлом: `...\SHA1\x64\Release\`.
Вам также потребуется использовать утилиту **RAMMap** для очистки памяти между вызовами функций (`Empty -> Empty StandBy List`)
```
SHA1.exe <\path\to\file>
```

## Сравнение результатов

### Debug версия

После запуска програмы для подсчета хэш-суммы файлас расширением `.rar` и размером 13.4 ГБ были получены следующие результаты:

**CryptoPP**
```
Hash sum of a file: 3646a1248834948dd8f8f842d694ee9fba0416d3
Digesting time: 88.4723 seconds
File size: 13723 MB
Calculation speed: 162.654 MB/s
```
**Intel IPP**
```
Hash sum of a file: 3646a1248834948dd8f8f842d694ee9fba0416d3
Digesting time: 88.3924 seconds
File size: 13723 MB
Calculation speed: 162.802 MB/s
```
**Windows standard API**
```
Hash sum of a file: 3646a1248834948dd8f8f842d694ee9fba0416d3
Digesting time: 88.4025 seconds
File size: 13723 MB
Calculation speed: 162.783 MB/s
```

### Release версия

**CryptoPP**
```
Hash sum of a file: 3646a1248834948dd8f8f842d694ee9fba0416d3
Digesting time: 88.4665 seconds
File size: 13723 MB
Calculation speed: 162.665 MB/s
```
**Intel IPP**
```
Hash sum of a file: 3646a1248834948dd8f8f842d694ee9fba0416d3
Digesting time: 88.4809 seconds
File size: 13723 MB
Calculation speed: 162.639 MB/s
```
**Windows standard API**
```
Hash sum of a file: 3646a1248834948dd8f8f842d694ee9fba0416d3
Digesting time: 88.4505 seconds
File size: 13723 MB
Calculation speed: 162.695 MB/s
```

Из полученных результатов можно сделать вывод о равнозначности библиотек в вопросе подсчета хэш-сумм в виду незначительной разницы во времени подсчета.
Однако стоит отметить, что стандартный встроенный API является приоритетным выбором для расчета хэш-суммы, поскольку его не нужно линковать к проекту.
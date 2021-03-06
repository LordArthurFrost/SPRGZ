﻿#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <strsafe.h>
#include <utility>   

#define KB 1024
#define MB KB*1024
#define GB MB*1024
#define RESULT pair <DWORD, DOUBLE>

using namespace std;

const DWORD BUFFER_SIZES[] = { 1*KB, 4*KB, 8*KB, 1*MB, 2*MB, 4*MB, 8*MB, 16*MB }; //16 по варианту
const DWORD FILE_SIZE = 1*GB; //1гб

void writeTest(DWORD);
RESULT writeToFile(HANDLE, DWORD);
void readTest();

int __cdecl _tmain(int argc, TCHAR* argv[])
{
    DWORD TESTS[] = { FILE_FLAG_WRITE_THROUGH, FILE_FLAG_NO_BUFFERING, FILE_FLAG_RANDOM_ACCESS, FILE_FLAG_SEQUENTIAL_SCAN };

    for(int i=0;i< sizeof(TESTS) / sizeof(DWORD);i++)
      writeTest(TESTS[i]);

    //readTest();
    system("Pause");
}

void writeTest(DWORD arg) {
    BOOL bErrorFlag = FALSE;

    //создаем файл "test.bin", после закрытия хендла файл будет удален
    HANDLE writeFile = CreateFile(_T("test.bin"),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        arg | FILE_FLAG_DELETE_ON_CLOSE, //FILE_FLAG_DELETE_ON_CLOSE
        NULL);

    //если файл не создался
    if (writeFile == INVALID_HANDLE_VALUE) {
        _tprintf(TEXT("Terminal failure: Unable to create file for write with error code %d.\n"), GetLastError());
        return;
    }

    DWORD arr_size = sizeof(BUFFER_SIZES) / sizeof(DWORD);
    RESULT*test_times = new RESULT[arr_size];

    _tprintf(TEXT("Testing...\n"));
    for (DWORD i = 0; i < arr_size; i++)
    {
        test_times[i] = writeToFile(writeFile, BUFFER_SIZES[i]);
       
        //отслеживаем ошибки
        if (test_times[i].first == NULL)
        {
            _tprintf(TEXT("Terminal failure: Unable to write to file with error code %d.\n"), GetLastError());
        }
        else
        {
            //выводим информацию про результаты тестирования
            double totalmb = ((test_times[i].first / (double)1024)) / (double)1024;
            _tprintf(TEXT("Wrote %lf mb successfully.\n"), totalmb);
            _tprintf(TEXT("Elapsed time = %lf seconds\n"), test_times[i].second);
            _tprintf(TEXT("Your write speed is %lf mb/sec\n\n"), (double)totalmb / test_times[i].second);

        }
        Sleep(1000);
    }
    CloseHandle(writeFile);
}

RESULT writeToFile(HANDLE writeFile, DWORD buffer_size) {
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;
    BOOL bErrorFlag;

    //тестовый массив данных
    char* DataBuffer = new char[buffer_size];
    for (int i = 0; i < buffer_size; i++)
        DataBuffer[i] = 't';

    DOUBLE totalTime = 0;
    DWORD iterations = FILE_SIZE / buffer_size;
    DWORD dwBytesToWrite = buffer_size * iterations;
    DWORD dwBytesWritten = 0, sumWritten = 0;

    //начинаем отсчет времени
    QueryPerformanceFrequency(&Frequency);

    //записываем в файл count раз массива данных
    for (int i = 0; i < iterations; ++i)
    {
        QueryPerformanceCounter(&StartingTime);

        bErrorFlag = WriteFile(
            writeFile,
            DataBuffer,
            buffer_size,
            &dwBytesWritten,
            NULL);

        QueryPerformanceCounter(&EndingTime);

        if (bErrorFlag == FALSE) return make_pair(NULL, NULL);

        //подсчитываем время
        ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
        ElapsedMicroseconds.QuadPart *= 1000000;
        ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

        sumWritten += dwBytesWritten;
        totalTime += (ElapsedMicroseconds.QuadPart / (double)1000000);
    }

    if (sumWritten != dwBytesToWrite)
    {
        _tprintf(TEXT("Error: dwBytesWritten != dwBytesToWrite\n"));
        return make_pair(NULL, NULL);
    }
    return make_pair(sumWritten, totalTime);
}

void readTest() {
  //  writeTest(NULL);

    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;

    //массив данных
    char* DataBuffer = new char[1024];

    DWORD dwBytesRead = 0;
    DWORD dwBytesTotalRead = 0;
    BOOL bErrorFlag = FALSE;

    //открываем файл "test.bin"
    HANDLE readFile = CreateFile(_T("test.bin"),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING | FILE_ATTRIBUTE_NORMAL,
        NULL);

    //если файл не открылся
    if (readFile == INVALID_HANDLE_VALUE) {
        _tprintf(TEXT("Terminal failure: Unable to read from file with error code %d.\n"), GetLastError());
        return;
    }

    //начинаем отсчет времени
    _tprintf(TEXT("Testing...\n"));

    DWORD sumRead = 0;
    double totalTime = 0;
    QueryPerformanceFrequency(&Frequency);

    //записываем в файл count раз массива данныхъ
    for (int i = 0; i < 5; ++i)
    {
        QueryPerformanceCounter(&StartingTime);

        bErrorFlag = ReadFile(
            readFile,
            DataBuffer,
            (1024),
            &dwBytesRead,
            NULL);

        QueryPerformanceCounter(&EndingTime);

        //подсчитываем время
        ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
        ElapsedMicroseconds.QuadPart *= 1000000;
        ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

        sumRead += dwBytesRead;
        totalTime += (ElapsedMicroseconds.QuadPart / (double)1000000);

        Sleep(1000);
    }

    //отслеживаем ошибки
    if (FALSE == bErrorFlag)
    {
        _tprintf(TEXT("Terminal failure: Unable to read from file with error code %d.\n"), GetLastError());
    }
    else
    {
        //выводим информацию про результаты тестирования
        double totalmb = (((sumRead) / (double)1024)) / (double)1024;

        _tprintf(TEXT("Read %lf mb successfully.\n"), totalmb);
        _tprintf(TEXT("Elapsed time = %lf seconds\n"), totalTime);
        _tprintf(TEXT("Your read speed is %lf mb/sec\n\n"), (double)totalmb / totalTime);
    }

    CloseHandle(readFile);
}


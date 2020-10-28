﻿#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <strsafe.h>

#define BUFFER_SIZE 1024*100000 //размер массива данных 250кб
#define COUNT 5 //кол-во записей в файл тестового массива (итоговый файл ~1г)

HANDLE writeFile, readFile;

void writeTest(DWORD);
void readTest();

int __cdecl _tmain(int argc, TCHAR* argv[])
{
   //  writeTest(FILE_FLAG_DELETE_ON_CLOSE);
    readTest();
}

void writeTest(DWORD atr) {
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;

    //тестовый массив данных
    char* DataBuffer = new char[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; i++)
        DataBuffer[i] = 't';


    DWORD dwBytesToWrite = BUFFER_SIZE * COUNT;
    DWORD dwBytesWritten = 0, sumWritten = 0;
    BOOL bErrorFlag = FALSE;
    double totalTime = 0;


    //создаем файл "test.bin", после закрытия хендла файл будет удален
    HANDLE writeFile = CreateFile(_T("test.bin"),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_FLAG_NO_BUFFERING | atr, //FILE_FLAG_DELETE_ON_CLOSE
        NULL);

    //если файл не создался
    if (writeFile == INVALID_HANDLE_VALUE) {
        _tprintf(TEXT("Terminal failure: Unable to create file for write with error code %d.\n"), GetLastError());
        return;
    }

    //начинаем отсчет времени
    _tprintf(TEXT("Testing...\n"));

    QueryPerformanceFrequency(&Frequency);

    //записываем в файл count раз массива данныхъ
    for (int i = 0; i < COUNT; ++i)
    {
        QueryPerformanceCounter(&StartingTime);

        bErrorFlag = WriteFile(
            writeFile,
            DataBuffer,
            BUFFER_SIZE,
            &dwBytesWritten,
            NULL);

        QueryPerformanceCounter(&EndingTime);

        //подсчитываем время
        ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
        ElapsedMicroseconds.QuadPart *= 1000000;
        ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

        sumWritten += dwBytesWritten;
        totalTime += (ElapsedMicroseconds.QuadPart / (double)1000000);

        Sleep(1000);
    }

    //отслеживаем ошибки
    if (FALSE == bErrorFlag)
    {
        _tprintf(TEXT("Terminal failure: Unable to write to file with error code %d.\n"), GetLastError());
    }
    else
    {
        if (sumWritten != dwBytesToWrite)
        {
            // This is an error because a synchronous write that results in
            // success (WriteFile returns TRUE) should write all data as
            // requested. This would not necessarily be the case for
            // asynchronous writes.
            _tprintf(TEXT("Error: dwBytesWritten != dwBytesToWrite\n"));
        }
        else
        {
            //выводим информацию про результаты тестирования
            double totalmb = ((sumWritten / (double)1024)) / (double)1024;
            _tprintf(TEXT("Wrote %lf mb successfully.\n"), totalmb);
            _tprintf(TEXT("Elapsed time = %lf seconds\n"), totalTime);
            _tprintf(TEXT("Your write speed is %lf mb/sec\n\n"), (double)totalmb / totalTime);
        }
    }

    CloseHandle(writeFile);
}

void readTest() {
    writeTest(NULL);

    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;

    //массив данных
    char* DataBuffer = new char[BUFFER_SIZE];

    DWORD dwBytesRead = 0;
    DWORD dwBytesTotalRead = 0;
    BOOL bErrorFlag = FALSE;

    //открываем файл "test.bin"
    HANDLE readFile = CreateFile(_T("test.bin"),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING | FILE_ATTRIBUTE_NORMAL,
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
    for (int i = 0; i < COUNT; ++i)
    {
        QueryPerformanceCounter(&StartingTime);

        bErrorFlag = ReadFile(
            readFile,
            DataBuffer,
            (BUFFER_SIZE),
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


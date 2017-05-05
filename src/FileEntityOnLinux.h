/*
 *
 * FileEntityOnLinux.h
 *
 * Copyright 2016 Yuichi Yoshii
 *     吉井雄一 @ 吉井産業  you.65535.kir@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef SRC_FILEENTITYONLINUX_H_
#define SRC_FILEENTITYONLINUX_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class FileEntityOnLinux {

private:

    string directory;

    string fileName;

    string fullPath;

    ifstream ifile;

    wifstream wifile;

    ofstream ofile;

    wofstream wofile;

    int fileSize;

    unique_ptr<char> fileContents;

    vector<string> fileLines;

    vector<wstring> wfileLines;

    bool readAsBinary;

    bool readAsMB;

    bool readPrepared;

    bool writeAsBinary;

    bool writeAsMB;

    bool writePrepared;

    bool readSuccess;

    bool writeSuccess;

    bool deleteSuccess;

    bool copySuccess;

    bool disposed;

    int CountFileSize();

    void FetchOnce();

public:

    void SetDirectory(string arg);

    string GetDirectory();

    void SetFileName(string arg);

    string GetFileName();

    void SetFullPath(string arg);

    string GetFullPath();

    void ReadPrepare(bool asBinary);

    void ReadPrepareW();

    bool IsReadPrepared();

    int GetFileSize();

    void ReadFile();

    void ReadFileW();

    bool IsReadSuccess();

    unique_ptr<char> GetFileContents();

    unique_ptr<char> GetFileContents(int arg1start, int arg2length);

    vector<string> GetFileLines();

    vector<wstring> GetFileLinesW();

    void SetFileContents(char * arg, int size);

    void SetFileLines(vector<string> arg);

    void SetFileLinesW(vector<wstring> arg);

    void AddFileLines(string arg);

    void AddFileLinesW(wstring arg);

    void WritePrepare(bool asBinary);

    void WritePrepareW();

    void OverWritePrepare(bool asBinary);

    void OverWritePrepareW();

    bool IsWritePrepared();

    void WriteFile();

    void WriteFileW(string localeword);

    bool IsWriteSuccess();

    bool FindFile();

    void DeleteExistingFile();

    bool IsDeleteSuccess();

    void CopyFile(string fileName);

    bool IsCopySuccess();

    FileEntityOnLinux();

    ~FileEntityOnLinux();

};

#endif /* SRC_FILEENTITYONLINUX_H_ */

/*
 *
 * FileEntityOnLinux.cpp
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

#include "FileEntityOnLinux.h"

int FileEntityOnLinux::CountFileSize() {
    ifile.seekg(0, istream::end);
    streamoff endPos = ifile.tellg();
    ifile.seekg(0, istream::beg);
    streamoff startPos = ifile.tellg();
    return (int) (endPos - startPos);
}

void FileEntityOnLinux::FetchOnce() {
    if (readAsBinary) {
        unique_ptr<char> readBuffer(new char[fileSize]);
        ifile.read(readBuffer.get(), fileSize);
        fileContents = move(readBuffer);
    }
    else {
        if (readAsMB) {
            string readingLine;
            while (getline(ifile, readingLine)) {
                fileLines.push_back(readingLine);
            }
        }
        else {
            wstring readingLine;
            while (getline(wifile, readingLine)) {
                wfileLines.push_back(readingLine);
            }
        }
    }
}

void FileEntityOnLinux::SetDirectory(string arg) {
    string newFullPath = arg + '/' + fileName;
    fullPath = newFullPath;
    directory.assign(arg);
}

string FileEntityOnLinux::GetDirectory() {
    return directory;
}

void FileEntityOnLinux::SetFileName(string arg) {
    string newFullPath = directory + '/' + arg;
    fullPath = newFullPath;
    fileName.assign(arg);
}

string FileEntityOnLinux::GetFileName() {
    return fileName;
}

void FileEntityOnLinux::SetFullPath(string arg) {
    string newDirectory = arg.substr(0, arg.find_last_of('/'));
    directory = newDirectory;
    string newFileName = arg.substr(arg.find_last_of('/') + 1);
    fileName = newFileName;
    fullPath.assign(arg);
}

string FileEntityOnLinux::GetFullPath() {
    if (fullPath.length() != 0) {
        return fullPath;
    }
    else {
        return directory + '/' + fileName;
    }
}

void FileEntityOnLinux::ReadPrepare(bool asBinary) {
    if (writePrepared) {
        readPrepared = false;
        return;
    }
    string readFullPath;
    if (fullPath.length() != 0) {
        readFullPath.assign(fullPath);
    }
    else {
        readFullPath.assign(directory);
        readFullPath.append("\\");
        readFullPath.append(fileName);
    }
    if (asBinary) {
        ifile = ifstream(readFullPath, ios::in | ios::binary);
        readAsBinary = true;
        readAsMB = false;
        writeAsBinary = true;
        writeAsMB = false;
    }
    else {
        ifile = ifstream(readFullPath, ios::in);
        readAsBinary = false;
        readAsMB = true;
        writeAsBinary = false;
        writeAsMB = true;
    }
    if (ifile) {
        fileSize = CountFileSize();
        readPrepared = true;
    }
    else {
        readPrepared = false;
    }
}

void FileEntityOnLinux::ReadPrepareW() {
    if (writePrepared) {
        readPrepared = false;
        return;
    }
    string readFullPath;
    if (fullPath.length() != 0) {
        readFullPath.assign(fullPath);
    }
    else {
        readFullPath.assign(directory);
        readFullPath.append("\\");
        readFullPath.append(fileName);
    }
    wifile = wifstream(readFullPath, ios::in);
    readAsBinary = false;
    readAsMB = false;
    writeAsBinary = false;
    writeAsMB = false;
    if (ifile) {
        readPrepared = true;
    }
    else {
        readPrepared = false;
    }
}

bool FileEntityOnLinux::IsReadPrepared() {
    return readPrepared;
}

int FileEntityOnLinux::GetFileSize() {
    return fileSize;
}

void FileEntityOnLinux::ReadFile() {
    readSuccess = false;

    FetchOnce();

    ifile.close();

    readSuccess = true;
}

void FileEntityOnLinux::ReadFileW() {
    readSuccess = false;

    FetchOnce();

    wifile.close();

    readSuccess = true;
}

bool FileEntityOnLinux::IsReadSuccess() {
    return readSuccess;
}

unique_ptr<char> FileEntityOnLinux::GetFileContents() {
    unique_ptr<char> ret(new char[fileSize]);
    for (int i = 0; i < fileSize; i++) {
        ret.get()[i] = fileContents.get()[i];
    }
    return ret;
}

unique_ptr<char> FileEntityOnLinux::GetFileContents(int arg1start,
        int arg2length) {
    unique_ptr<char> ret(new char[arg2length]);
    for (int i = 0; i < arg2length; i++) {
        ret.get()[i] = fileContents.get()[arg1start + i];
    }
    return ret;
}

vector<string> FileEntityOnLinux::GetFileLines() {
    return fileLines;
}

vector<wstring> FileEntityOnLinux::GetFileLinesW() {
    return wfileLines;
}

void FileEntityOnLinux::SetFileContents(char * arg, int size) {
    fileContents.reset(arg);
    fileSize = size;
}

void FileEntityOnLinux::SetFileLines(vector<string> arg) {
    fileLines = arg;
}

void FileEntityOnLinux::SetFileLinesW(vector<wstring> arg) {
    wfileLines = arg;
}

void FileEntityOnLinux::AddFileLines(string arg) {
    fileLines.push_back(arg);
}

void FileEntityOnLinux::AddFileLinesW(wstring arg) {
    wfileLines.push_back(arg);
}

void FileEntityOnLinux::WritePrepare(bool asBinary) {
    if (readPrepared) {
        writePrepared = false;
        return;
    }
    string writeFullPath;
    if (fullPath.length() != 0) {
        writeFullPath.assign(fullPath);
    }
    else {
        writeFullPath.assign(directory);
        writeFullPath.append("\\");
        writeFullPath.append(fileName);
    }
    if (asBinary) {
        ofile = ofstream(writeFullPath, ios::out | ios::binary);
        readAsBinary = true;
        readAsMB = false;
        writeAsBinary = true;
        writeAsMB = false;
    }
    else {
        ofile = ofstream(writeFullPath, ios::out);
        readAsBinary = false;
        readAsMB = true;
        writeAsBinary = false;
        writeAsMB = true;
    }
    if (ofile) {
        writePrepared = true;
    }
    else {
        writePrepared = false;
    }
}

void FileEntityOnLinux::WritePrepareW() {
    if (readPrepared) {
        writePrepared = false;
        return;
    }
    string writeFullPath;
    if (fullPath.length() != 0) {
        writeFullPath.assign(fullPath);
    }
    else {
        writeFullPath.assign(directory);
        writeFullPath.append("\\");
        writeFullPath.append(fileName);
    }
    wofile = wofstream(writeFullPath, ios::out);
    readAsBinary = false;
    readAsMB = false;
    writeAsBinary = false;
    writeAsMB = false;
    if (ofile) {
        writePrepared = true;
    }
    else {
        writePrepared = false;
    }
}

void FileEntityOnLinux::OverWritePrepare(bool asBinary) {
    if (readPrepared) {
        writePrepared = false;
        return;
    }
    string writeFullPath;
    if (fullPath.length() != 0) {
        writeFullPath.assign(fullPath);
    }
    else {
        writeFullPath.assign(directory);
        writeFullPath.append("\\");
        writeFullPath.append(fileName);
    }
    if (asBinary) {
        ofile = ofstream(writeFullPath, ios::out | ios::binary | ios::app);
        readAsBinary = true;
        readAsMB = false;
        writeAsBinary = true;
        writeAsMB = false;
    }
    else {
        ofile = ofstream(writeFullPath, ios::out | ios::app);
        readAsBinary = false;
        readAsMB = true;
        writeAsBinary = false;
        writeAsMB = true;
    }
    if (ofile) {
        writePrepared = true;
    }
    else {
        writePrepared = false;
    }
}

void FileEntityOnLinux::OverWritePrepareW() {
    if (readPrepared) {
        writePrepared = false;
        return;
    }
    string writeFullPath;
    if (fullPath.length() != 0) {
        writeFullPath.assign(fullPath);
    }
    else {
        writeFullPath.assign(directory);
        writeFullPath.append("\\");
        writeFullPath.append(fileName);
    }
    wofile = wofstream(writeFullPath, ios::out | ios::app);
    readAsBinary = false;
    readAsMB = false;
    writeAsBinary = false;
    writeAsMB = false;
    if (ofile) {
        writePrepared = true;
    }
    else {
        writePrepared = false;
    }
}

bool FileEntityOnLinux::IsWritePrepared() {
    return writePrepared;
}

void FileEntityOnLinux::WriteFile() {
    writeSuccess = false;

    if (writeAsBinary) {
        ofile.write(fileContents.get(), fileSize);
    }
    else {
        int count = (int) fileLines.size();
        for (int i = 0; i < count; i++) {
            ofile.write(fileLines.at(i).c_str(), fileLines.at(i).size());
            ofile.write("\n", 1);
        }
    }
    ofile.close();

    writeSuccess = true;
}

void FileEntityOnLinux::WriteFileW(string localeword) {
    writeSuccess = false;

    if (localeword.compare("none") == 0) {
        ios_base::sync_with_stdio(false);
        locale newlocale("");
        wofile.imbue(newlocale);
    }
    else if (localeword.size() != 0) {
        ios_base::sync_with_stdio(false);
        locale newlocale(localeword);
        wofile.imbue(newlocale);
    }
    int count = (int) fileLines.size();
    for (int i = 0; i < count; i++) {
        wofile.write(wfileLines.at(i).c_str(), wfileLines.at(i).size());
        wofile.write(L"\n", 1);
    }
    wofile.close();

    writeSuccess = true;
}

bool FileEntityOnLinux::IsWriteSuccess() {
    return writeSuccess;
}

bool FileEntityOnLinux::FindFile() {
    string path;
    if (fullPath.length() != 0) {
        path = fullPath;
    }
    else {
        path = directory + "\\" + fileName;
    }
    ifstream f(path);
    return f.good();
}

void FileEntityOnLinux::DeleteExistingFile() {
    deleteSuccess = false;

    string path;
    if (fullPath.length() != 0) {
        path = fullPath;
    }
    else {
        path = directory + "\\" + fileName;
    }
    if (FindFile()) {
        if (remove(path.c_str())) {
            deleteSuccess = true;
        }
    }
}

bool FileEntityOnLinux::IsDeleteSuccess() {
    return deleteSuccess;
}

void FileEntityOnLinux::CopyFile(string fileName) {
    ifstream from(GetFullPath());
    if (!from) {
        return;
    }
    ofstream to(fileName.c_str());
    if (!to) {
        return;
    }
    to << from.rdbuf() << flush;
    if (!from) {
        return;
    }
    if (!to) {
        return;
    }
    copySuccess = true;
}

bool FileEntityOnLinux::IsCopySuccess() {
    return copySuccess;
}

FileEntityOnLinux::FileEntityOnLinux() {
    fileSize = 0;
    readAsBinary = false;
    readAsMB = false;
    readPrepared = false;
    writeAsBinary = false;
    writeAsMB = false;
    writePrepared = false;
    readSuccess = false;
    writeSuccess = false;
    deleteSuccess = false;
    copySuccess = false;
    disposed = false;
}

FileEntityOnLinux::~FileEntityOnLinux() {
}

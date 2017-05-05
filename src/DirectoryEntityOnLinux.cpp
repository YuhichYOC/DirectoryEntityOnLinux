/*
 *
 * DirectoryEntityOnLinux.cpp
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

#include "DirectoryEntityOnLinux.h"

// ディレクトリ構成をメモリ上に展開
unique_ptr<DirectoryEntityOnLinux> DirectoryEntityOnLinux::Describe(
        string arg) {
    unique_ptr<DirectoryEntityOnLinux> d(new DirectoryEntityOnLinux());
    d->SetDirectory(arg);

    struct dirent ** e;
    int ret = scandir(arg.c_str(), &e, nullptr, nullptr);
    if (ret != -1) {
        for (int i = 0; i < ret; i++) {
            string childName(e[i]->d_name);
            if (!(childName.compare(".\0") == 0
                    || childName.compare("..\0") == 0)) {
                string childPath = arg + childName;
                struct stat s;
                if (stat(childPath.c_str(), &s) == 0) {
                    if ((s.st_mode & S_IFMT) == S_IFDIR) {
                        d->AddDirectory(Describe(childPath));
                    }
                    else {
                        unique_ptr<FileEntityOnLinux> f(
                                new FileEntityOnLinux());
                        f->SetFileName(childPath);
                        d->AddFile(move(f));
                    }
                }
            }
        }
    }

    return d;
}

// すでに存在するディレクトリを削除 ( DeleteExistingDir, recursive = true ) が呼び出す
void DirectoryEntityOnLinux::DeleteExistingDir(
        unique_ptr<DirectoryEntityOnLinux> arg) {
    for (size_t i = 0; i < arg->GetDirectories().size(); i++) {
        DeleteExistingDir(move(arg->GetDirectories().at(i)));
        int ret = remove(arg->GetFullPath().c_str());
        if (ret != 0) {
            return;
        }
    }
    for (size_t j = 0; j < arg->GetFiles().size(); j++) {
        arg->GetFiles().at(j)->DeleteExistingFile();
        if (!arg->GetFiles().at(j)->IsDeleteSuccess()) {
            return;
        }
    }
    deleteSuccess = true;
}

// すでに存在するディレクトリを削除 ( DeleteExistingDir, recursive = false ) が呼び出す
void DirectoryEntityOnLinux::DeleteExistingDirNoRecursive(
        unique_ptr<DirectoryEntityOnLinux> arg) {
    for (size_t i = 0; i < arg->GetFiles().size(); i++) {
        arg->GetFiles().at(i)->DeleteExistingFile();
        if (!arg->GetFiles().at(i)->IsDeleteSuccess()) {
            return;
        }
    }
    deleteSuccess = true;
}

// ディレクトリコピー ( DirCopy, recursive = true ) が呼び出す
void DirectoryEntityOnLinux::DirCopy(unique_ptr<DirectoryEntityOnLinux> arg1sub,
        string arg2path) {
    for (size_t i = 0; i < arg1sub->GetDirectories().size(); i++) {
        int ret = mkdir(arg1sub->GetFullPath().c_str(), S_IRWXU);
        if (ret != 0) {
            copySuccess = false;
        }
    }
    for (size_t j = 0; j < arg1sub->GetFiles().size(); j++) {
        arg1sub->GetFiles().at(j)->CopyFile(
                arg2path + '/' + arg1sub->GetFiles().at(j)->GetFileName());
        if (!arg1sub->GetFiles().at(j)->IsCopySuccess()) {
            copySuccess = false;
        }
    }
    copySuccess = true;
}

// ディレクトリコピー ( DirCopy, recursive = false ) が呼び出す
void DirectoryEntityOnLinux::DirCopyNoRecursive(
        unique_ptr<DirectoryEntityOnLinux> arg1sub, string arg2path) {
    for (size_t i = 0; i < arg1sub->GetFiles().size(); i++) {
        arg1sub->GetFiles().at(i)->CopyFile(
                arg2path + '/' + arg1sub->GetFiles().at(i)->GetFileName());
        if (!arg1sub->GetFiles().at(i)->IsCopySuccess()) {
            copySuccess = false;
        }
    }
    copySuccess = true;
}

// このインスタンスが表すディレクトリのフルパスをセットする
void DirectoryEntityOnLinux::SetDirectory(string arg) {
    rootDirectoryFound = false;
    if (FindDir(arg)) {
        fullPath = arg;
        directoryName = arg.substr(arg.find_last_of('/') + 1);
        rootDirectoryFound = true;
    }
}

// このインスタンスが表すディレクトリの名前を返す
string DirectoryEntityOnLinux::GetDirectoryName() {
    return directoryName;
}

// このインスタンスが表すディレクトリのフルパスを返す
string DirectoryEntityOnLinux::GetFullPath() {
    return fullPath;
}

// １階層目のディレクトリ名がセットされている場合 true を返す
bool DirectoryEntityOnLinux::RootDirectoryFound() {
    return rootDirectoryFound;
}

// ディレクトリ構成をメモリ上に展開
void DirectoryEntityOnLinux::Describe() {
    if (!rootDirectoryFound) {
        return;
    }

    struct dirent ** e;
    int ret = scandir(fullPath.c_str(), &e, nullptr, nullptr);
    if (ret != -1) {
        for (int i = 0; i < ret; i++) {
            string childName(e[i]->d_name);
            if (!(childName.compare(".\0") == 0
                    || childName.compare("..\0") == 0)) {
                string childPath = fullPath + '/' + childName;
                struct stat s;
                if (stat(childPath.c_str(), &s) == 0) {
                    if ((s.st_mode & S_IFMT) == S_IFDIR) {
                        this->AddDirectory(Describe(childPath));
                    }
                    else {
                        unique_ptr<FileEntityOnLinux> f(
                                new FileEntityOnLinux());
                        f->SetFileName(childPath);
                        this->AddFile(move(f));
                    }
                }
            }
        }
    }

}

// このインスタンスが表すディレクトリそのものをファイルシステム上に作成する
void DirectoryEntityOnLinux::CreateRootDirectory() {
    createSuccess = false;

    if (!FindDir(fullPath)) {
        int ret = mkdir(fullPath.c_str(), S_IRWXU);
        if (ret == 0) {
            createSuccess = true;
        }
    }
}

// サブディレクトリの構成を表すメモリ情報を返す
vector<unique_ptr<DirectoryEntityOnLinux>> DirectoryEntityOnLinux::GetDirectories() {
    return move(subDirectories);
}

// サブディレクトリの構成に引数のメモリ情報を追加する
void DirectoryEntityOnLinux::AddDirectory(
        unique_ptr<DirectoryEntityOnLinux> arg) {
    subDirectories.push_back(move(arg));
}

// このインスタンスが表すディレクトリ直下のファイルを表すメモリ情報を返す
vector<unique_ptr<FileEntityOnLinux>> DirectoryEntityOnLinux::GetFiles() {
    return move(files);
}

// このインスタンスが表すディレクトリにファイルを表すメモリ情報を追加する
void DirectoryEntityOnLinux::AddFile(unique_ptr<FileEntityOnLinux> arg) {
    files.push_back(move(arg));
}

// ディレクトリ作成 ( CreateRootDirectory, CreateDir ) が成功している場合 true を返す
bool DirectoryEntityOnLinux::IsCreateSuccess() {
    return createSuccess;
}

// ディレクトリ削除 ( DeleteExistingDir, DeleteExistingDirNoRecursive ) が成功している場合 true を返す
bool DirectoryEntityOnLinux::IsDeleteSuccess() {
    return deleteSuccess;
}

// ディレクトリ作成
void DirectoryEntityOnLinux::CreateDir() {
    createSuccess = false;

    if (!FindDir()) {
        int ret = mkdir(fullPath.c_str(), S_IRWXU);
        if (ret == 0) {
            createSuccess = true;
        }
    }
}

// ディレクトリが実在する場合 true を返す
bool DirectoryEntityOnLinux::FindDir() {
    struct stat s;
    int ret = stat(fullPath.c_str(), &s);
    if (ret == 0) {
        return true;
    }
    return false;
}

// 引数のディレクトリが実在する場合 true を返す
bool DirectoryEntityOnLinux::FindDir(string arg) {
    struct stat s;
    int ret = stat(arg.c_str(), &s);
    if (ret == 0) {
        return true;
    }
    return false;
}

// ディレクトリの内容を削除
// recursive = true の場合サブディレクトリの内容物も全て削除
void DirectoryEntityOnLinux::DeleteExistingDir(bool recursive) {
    deleteSuccess = false;

    if (FindDir()) {
        unique_ptr<DirectoryEntityOnLinux> d(new DirectoryEntityOnLinux());
        d.reset(this);
        if (recursive) {
            DeleteExistingDir(move(d));
        }
        else {
            DeleteExistingDirNoRecursive(move(d));
        }
    }
}

// 引数のディレクトリの内容を削除
// recursive = true の場合サブディレクトリの内容物も全て削除
void DirectoryEntityOnLinux::DeleteExistingDir(string arg, bool recursive) {
    deleteSuccess = false;

    if (FindDir(arg)) {
        unique_ptr<DirectoryEntityOnLinux> d(new DirectoryEntityOnLinux());
        d->SetDirectory(arg);
        d->Describe();
        if (recursive) {
            DeleteExistingDir(move(d));
        }
        else {
            DeleteExistingDirNoRecursive(move(d));
        }
    }
}

// このインスタンスで表されるディレクトリを引数のディレクトリコピー
// 引数のディレクトリが実在しない場合はコピー前に作成する
// recursive = true の場合サブディレクトリの内容物も再帰的にコピー
void DirectoryEntityOnLinux::DirCopy(string arg, bool recursive,
        bool rollback) {
    copySuccess = true;

    if (!FindDir(arg)) {
        int ret = mkdir(arg.c_str(), S_IRWXU);
        if (ret != 0) {
            return;
        }
    }
    unique_ptr<DirectoryEntityOnLinux> d(new DirectoryEntityOnLinux());
    d.reset(this);
    if (recursive) {
        DirCopy(move(d), arg);
    }
    else {
        DirCopyNoRecursive(move(d), arg);
    }
}

// コピー ( DirCopy ) のロールバック
// コピー先ディレクトリを全て削除する
void DirectoryEntityOnLinux::CopyRollback() {
    DeleteExistingDir(dirCopyTo, true);
}

// コンストラクタ
DirectoryEntityOnLinux::DirectoryEntityOnLinux() {
    directoryName = string();
    fullPath = string();
    rootDirectoryFound = false;
    createSuccess = false;
    deleteSuccess = false;
    dirCopyTo = string();
    useCopyRollback = false;
    copySuccess = false;
}

// デストラクタ
DirectoryEntityOnLinux::~DirectoryEntityOnLinux() {
}

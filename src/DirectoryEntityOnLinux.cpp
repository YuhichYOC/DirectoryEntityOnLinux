/*
 * DirectoryEntityOnLinux.cpp
 *
 *  Created on: 2017/02/24
 *      Author: mssqlserver
 */

#include "DirectoryEntityOnLinux.h"

// ディレクトリ構成をメモリ上に展開
DirectoryEntityOnLinux DirectoryEntityOnLinux::Describe(string arg) {
    DirectoryEntityOnLinux d;
    d.SetDirectory(arg);

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
                        d.AddDirectory(Describe(childPath));
                    }
                    else {
                        FileEntityOnLinux f;
                        f.SetFileName(childPath);
                        d.AddFile(f);
                    }
                }
            }
        }
    }

    return d;
}

// すでに存在するディレクトリを削除 ( DeleteExistingDir, recursive = true ) が呼び出す
void DirectoryEntityOnLinux::DeleteExistingDir(DirectoryEntityOnLinux arg) {
    for (int i = 0; i < arg.GetDirectories().size(); i++) {
        DeleteExistingDir(arg.GetDirectories().at(i).get());
        int ret = remove(arg.GetFullPath().c_str());
        if (ret != 0) {
            return;
        }
    }
    for (int j = 0; j < arg.GetFiles().size(); j++) {
        arg.GetFiles().at(j)->DeleteExistingFile();
        if (!arg.GetFiles().at(j)->IsDeleteSuccess()) {
            return;
        }
    }
    deleteSuccess = true;
}

// すでに存在するディレクトリを削除 ( DeleteExistingDir, recursive = false ) が呼び出す
void DirectoryEntityOnLinux::DeleteExistingDirNoRecursive(
        DirectoryEntityOnLinux arg) {
    for (int i = 0; i < arg.GetFiles().size(); i++) {
        arg.GetFiles().at(i)->DeleteExistingFile();
        if (!arg.GetFiles().at(i)->IsDeleteSuccess()) {
            return;
        }
    }
    deleteSuccess = true;
}

// ディレクトリコピー ( DirCopy, recursive = true ) が呼び出す
void DirectoryEntityOnLinux::DirCopy(DirectoryEntityOnLinux arg1subDir,
        string arg2path) {
    for (int i = 0; i < arg1subDir.GetDirectories().size(); i++) {
        int ret = mkdir(arg1subDir.GetFullPath().c_str(), S_IRWXU);
        if (ret != 0) {
            copySuccess = false;
        }
    }
    for (int j = 0; j < arg1subDir.GetFiles().size(); j++) {
        arg1subDir.GetFiles().at(j)->CopyFile(
                arg2path + '/' + arg1subDir.GetFiles().at(j)->GetFileName());
        if (!arg1subDir.GetFiles().at(j)->IsCopySuccess()) {
            copySuccess = false;
        }
    }
    copySuccess = true;
}

// ディレクトリコピー ( DirCopy, recursive = false ) が呼び出す
void DirectoryEntityOnLinux::DirCopyNoRecursive(
        DirectoryEntityOnLinux arg1subDir, string arg2path) {
    for (int i = 0; i < arg1subDir.GetFiles().size(); i++) {
        arg1subDir.GetFiles().at(i)->CopyFile(
                arg2path + '/' + arg1subDir.GetFiles().at(i)->GetFileName());
        if (!arg1subDir.GetFiles().at(i)->IsCopySuccess()) {
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
                        subDirectories.push_back(
                                unique_ptr<DirectoryEntityOnLinux>(
                                        Describe(childPath)));
                    }
                    else {
                        FileEntityOnLinux f;
                        f.SetFileName(childPath);
                        files.push_back(unique_ptr<FileEntityOnLinux>(f));
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

// サブディレクトリの構成を表すメモリ情報をセットする
void DirectoryEntityOnLinux::SetDirectories(
        vector<unique_ptr<DirectoryEntityOnLinux>> arg) {
    subDirectories = arg;
}

// サブディレクトリの構成を表すメモリ情報を返す
vector<unique_ptr<DirectoryEntityOnLinux>> DirectoryEntityOnLinux::GetDirectories() {
    return subDirectories;
}

// サブディレクトリの構成に引数のメモリ情報を追加する
void DirectoryEntityOnLinux::AddDirectory(DirectoryEntityOnLinux arg) {
    subDirectories.push_back(unique_ptr<DirectoryEntityOnLinux>(arg));
}

// このインスタンスが表すディレクトリにファイルを表すメモリ情報をセットする
void DirectoryEntityOnLinux::SetFiles(
        vector<unique_ptr<FileEntityOnLinux>> arg) {
    files = arg;
}

// このインスタンスが表すディレクトリ直下のファイルを表すメモリ情報を返す
vector<unique_ptr<FileEntityOnLinux>> DirectoryEntityOnLinux::GetFiles() {
    return files;
}

// このインスタンスが表すディレクトリにファイルを表すメモリ情報を追加する
void DirectoryEntityOnLinux::AddFile(FileEntityOnLinux arg) {
    files.push_back(unique_ptr<FileEntityOnLinux>(arg));
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
    WCharString path;
    unique_ptr<wchar_t> dirPath = move(path.Value(fullPath).ToWChar());
    if (PathFileExists((LPCWSTR) dirPath.get())) {
        return true;
    }
    return false;
}

// 引数のディレクトリが実在する場合 true を返す
bool DirectoryEntityOnLinux::FindDir(string arg)
        {
    WCharString path;
    unique_ptr<wchar_t> dirPath = move(path.Value(arg).ToWChar());
    if (PathFileExists((LPCWSTR) dirPath.get())) {
        return true;
    }
    return false;
}

// ディレクトリの内容を削除
// recursive = true の場合サブディレクトリの内容物も全て削除
void DirectoryEntityOnLinux::DeleteExistingDir(bool recursive)
        {
    deleteSuccess = false;

    if (FindDir()) {
        unique_ptr<DirectoryEntityOnLinux> deleteDir(
                new DirectoryEntityOnLinux());
        deleteDir->SetDirectory(fullPath);
        deleteDir->Describe();
        if (recursive) {
            DeleteExistingDir(deleteDir.get());
        }
        else {
            DeleteExistingDirNoRecursive(deleteDir.get());
        }
    }
}

// 引数のディレクトリの内容を削除
// recursive = true の場合サブディレクトリの内容物も全て削除
void DirectoryEntityOnLinux::DeleteExistingDir(string arg, bool recursive)
        {
    deleteSuccess = false;

    if (FindDir(arg)) {
        unique_ptr<DirectoryEntityOnLinux> deleteDir(
                new DirectoryEntityOnLinux());
        deleteDir->SetDirectory(arg);
        deleteDir->Describe();
        if (recursive) {
            DeleteExistingDir(deleteDir.get());
        }
        else {
            DeleteExistingDirNoRecursive(deleteDir.get());
        }
    }
}

// このインスタンスで表されるディレクトリを引数のディレクトリコピー
// 引数のディレクトリが実在しない場合はコピー前に作成する
// recursive = true の場合サブディレクトリの内容物も再帰的にコピー
void DirectoryEntityOnLinux::DirCopy(string arg, bool recursive, bool rollback)
        {
    copySuccess = true;

    useCopyRollback = rollback;
    dirCopyTo = arg;

    WCharString path;
    unique_ptr<wchar_t> createDirPath = move(path.Value(arg).ToWChar());
    int ret = CreateDirectory((LPCWSTR) createDirPath.get(), nullptr);
    if (ret != 0) {
        if (recursive) {
            DirCopy(this, arg);
        }
        else {
            DirCopyNoRecursive(this, arg);
        }
    }
    else {
        copySuccess = false;
    }
}

// コピー ( DirCopy ) のロールバック
// コピー先ディレクトリを全て削除する
void DirectoryEntityOnLinux::CopyRollback()
{
    DeleteExistingDir(dirCopyTo, true);
}

// コンストラクタ
DirectoryEntityOnLinux::DirectoryEntityOnLinux()
{
    subDirectories = new vector<DirectoryEntityOnLinux *>();
    files = new vector<FileEntityOnLinux *>();
    disposed = false;
}

// メモリ開放メソッド
void DirectoryEntityOnLinux::Dispose()
{
    for (size_t i = 0; i < subDirectories->size(); i++) {
        delete subDirectories->at(i);
    }
    delete subDirectories;
    for (size_t j = 0; j < files->size(); j++) {
        delete files->at(j);
    }
    delete files;
    disposed = true;
}

// デストラクタ
DirectoryEntityOnLinux::~DirectoryEntityOnLinux()
{
    if (!disposed) {
        Dispose();
    }
}

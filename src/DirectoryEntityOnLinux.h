/*
 *
 * DirectoryEntityOnLinux.h
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

#ifndef SRC_DIRECTORYENTITYONLINUX_H_
#define SRC_DIRECTORYENTITYONLINUX_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <dirent.h>
#include <sys/stat.h>

#include "FileEntityOnLinux.h"

using namespace std;

class DirectoryEntityOnLinux {

private:

    // このインスタンスが表すディレクトリの名前
    string directoryName;

    // このインスタンスが表すディレクトリのフルパス
    string fullPath;

    // サブディレクトリを表すコンテナ
    vector<unique_ptr<DirectoryEntityOnLinux>> subDirectories;

    // このインスタンスに格納されているファイルを表すコンテナ
    vector<unique_ptr<FileEntityOnLinux>> files;

    // １階層目のディレクトリがセットされている場合 true
    bool rootDirectoryFound;

    // ディレクトリ作成 ( CreateRootDirectory, CreateDir ) が成功している場合 true
    bool createSuccess;

    // ディレクトリ削除 ( DeleteExistingDir, DeleteExistingDirNoRecursive ) が成功している場合 true
    bool deleteSuccess;

    // このインスタンスで表されるディレクトリのコピー先 ( CopyRollback で使用する )
    string dirCopyTo;

    // ディレクトリコピー ( DirCopy ) のオプション・このインスタンスによるディレクトリコピーの後
    // 何らかの異常系処理でコピー結果を削除するときに true をセットする
    bool useCopyRollback;

    // ディレクトリコピー ( DirCopy ) が成功している場合 true
    bool copySuccess;

    // ディレクトリ構成をメモリ上に展開
    unique_ptr<DirectoryEntityOnLinux> Describe(string arg);

    // すでに存在するディレクトリを削除 ( DeleteExistingDir, recursive = true ) が呼び出す
    void DeleteExistingDir(unique_ptr<DirectoryEntityOnLinux> arg);

    // すでに存在するディレクトリを削除 ( DeleteExistingDir, recursive = false ) が呼び出す
    void DeleteExistingDirNoRecursive(unique_ptr<DirectoryEntityOnLinux> arg);

    // ディレクトリコピー ( DirCopy, recursive = true ) が呼び出す
    void DirCopy(unique_ptr<DirectoryEntityOnLinux> arg1sub, string arg2path);

    // ディレクトリコピー ( DirCopy, recursive = false ) が呼び出す
    void DirCopyNoRecursive(unique_ptr<DirectoryEntityOnLinux> arg1sub,
            string arg2path);

public:

    // このインスタンスが表すディレクトリのフルパスをセットする
    void SetDirectory(string arg);

    // このインスタンスが表すディレクトリの名前を返す
    string GetDirectoryName();

    // このインスタンスが表すディレクトリのフルパスを返す
    string GetFullPath();

    // １階層目のディレクトリ名がセットされている場合 true を返す
    bool RootDirectoryFound();

    // ディレクトリ構成をメモリ上に展開
    void Describe();

    // このインスタンスが表すディレクトリそのものをファイルシステム上に作成する
    void CreateRootDirectory();

    // サブディレクトリの構成を表すメモリ情報を返す
    vector<unique_ptr<DirectoryEntityOnLinux>> GetDirectories();

    // サブディレクトリの構成に引数のメモリ情報を追加する
    void AddDirectory(unique_ptr<DirectoryEntityOnLinux> arg);

    // このインスタンスが表すディレクトリ直下のファイルを表すメモリ情報を返す
    vector<unique_ptr<FileEntityOnLinux>> GetFiles();

    // このインスタンスが表すディレクトリにファイルを表すメモリ情報を追加する
    void AddFile(unique_ptr<FileEntityOnLinux> arg);

    // ディレクトリ作成 ( CreateRootDirectory, CreateDir ) が成功している場合 true を返す
    bool IsCreateSuccess();

    // ディレクトリ削除 ( DeleteExistingDir, DeleteExistingDirNoRecursive ) が成功している場合 true を返す
    bool IsDeleteSuccess();

    // ディレクトリ作成
    void CreateDir();

    // ディレクトリが実在する場合 true を返す
    bool FindDir();

    // 引数のディレクトリが実在する場合 true を返す
    bool FindDir(string arg);

    // ディレクトリの内容を削除
    // recursive = true の場合サブディレクトリの内容物も全て削除
    void DeleteExistingDir(bool recursive);

    // 引数のディレクトリの内容を削除
    // recursive = true の場合サブディレクトリの内容物も全て削除
    void DeleteExistingDir(string arg, bool recursive);

    // このインスタンスで表されるディレクトリを引数のディレクトリコピー
    // 引数のディレクトリが実在しない場合はコピー前に作成する
    // recursive = true の場合サブディレクトリの内容物も再帰的にコピー
    void DirCopy(string arg, bool recursive, bool rollback);

    // コピー ( DirCopy ) のロールバック
    // コピー先ディレクトリを全て削除する
    void CopyRollback();

    // コンストラクタ
    DirectoryEntityOnLinux();

    // デストラクタ
    ~DirectoryEntityOnLinux();

};

#endif /* SRC_DIRECTORYENTITYONLINUX_H_ */

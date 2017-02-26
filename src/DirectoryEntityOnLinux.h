/*
 * DirectoryEntityOnLinux.h
 *
 *  Created on: 2017/02/24
 *      Author: mssqlserver
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

#include <FileEntityOnLinux.h>

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

    // インスタンス破棄が完了している場合 true
    bool disposed;

    // ディレクトリ構成をメモリ上に展開
    DirectoryEntityOnLinux Describe(string arg);

    // すでに存在するディレクトリを削除 ( DeleteExistingDir, recursive = true ) が呼び出す
    void DeleteExistingDir(DirectoryEntityOnLinux arg);

    // すでに存在するディレクトリを削除 ( DeleteExistingDir, recursive = false ) が呼び出す
    void DeleteExistingDirNoRecursive(DirectoryEntityOnLinux arg);

    // ディレクトリコピー ( DirCopy, recursive = true ) が呼び出す
    void DirCopy(DirectoryEntityOnLinux arg1subDir, string arg2path);

    // ディレクトリコピー ( DirCopy, recursive = false ) が呼び出す
    void DirCopyNoRecursive(DirectoryEntityOnLinux arg1subDir, string arg2path);

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

    // サブディレクトリの構成を表すメモリ情報をセットする
    void SetDirectories(vector<unique_ptr<DirectoryEntityOnLinux>> arg);

    // サブディレクトリの構成を表すメモリ情報を返す
    vector<unique_ptr<DirectoryEntityOnLinux>> GetDirectories();

    // サブディレクトリの構成に引数のメモリ情報を追加する
    void AddDirectory(DirectoryEntityOnLinux arg);

    // このインスタンスが表すディレクトリにファイルを表すメモリ情報をセットする
    void SetFiles(vector<unique_ptr<FileEntityOnLinux>> arg);

    // このインスタンスが表すディレクトリ直下のファイルを表すメモリ情報を返す
    vector<unique_ptr<FileEntityOnLinux>> GetFiles();

    // このインスタンスが表すディレクトリにファイルを表すメモリ情報を追加する
    void AddFile(FileEntityOnLinux arg);

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

    // メモリ開放メソッド
    void Dispose();

    // デストラクタ
    ~DirectoryEntityOnLinux();
};

#endif /* SRC_DIRECTORYENTITYONLINUX_H_ */

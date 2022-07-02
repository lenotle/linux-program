#pragma once
#include <iostream>
#include <string>
using namespace std;

// 共享内存基类
// 创建、关联、断开连接、删除共享内存
class BaseShm
{
public:
    // 打开已经存在的共享内存
    BaseShm(int key);
    BaseShm(string path);
    // 创建共享内存
    BaseShm(key_t key, int size);
    BaseShm(string path, int size);
    ~BaseShm();

    // 共享内存和进程建立关联
    void *mapShm();
    // 共享内存和进程断开关联
    int unmapShm();
    // 标记删除共享内存
    int delShm();

private:
    int getShmKeyId(key_t key, int size, int flag);

protected:
    void *ptr; // 共享内存区域
private:
    int shmId;
};
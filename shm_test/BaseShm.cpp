#include "BaseShm.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

// ftok 最后一个参数
const char RandX = 'x';

BaseShm::BaseShm(int key)
{
   this->getShmKeyId(key, 0, 0);
}
BaseShm::BaseShm(string path)
{
    this->getShmKeyId(ftok(path, RandX), 0, 0);
}

BaseShm::BaseShm(key_t key, int size)
{
    this->getShmKeyId(key, size, IPC_CREAT | 0644);
}
BaseShm::BaseShm(string path, int size)
{
    this->getShmKeyId(ftok(path, RandX), size, IPC_CREAT | 0644);
}
BaseShm::~BaseShm()
{

}


void *BaseShm::mapShm()
{
    this->ptr = shmat(this->shmId, NULL, 0);
    if (this->ptr == (void *)-1)
    {
        return nullptr;
    }
    return this->ptr;
}

int BaseShm::unmapShm()
{
    return shmdt(this->ptr);
}

int BaseShm::delShm()
{
    return shmctl(this->shmId, IPC_RMID, NULL);
}

int BaseShm::getShmKeyId(key_t key, int size, int flag)
{
    cout << "Shared memory size: " << size << endl;
    this->shmId = shmget(key, size, flag);
    if (-1 == this->shmId)
    {
        cerr << "open or create shared memory failed..." << endl;
    }
    return this->shmId;
}
#include "SecKeyShm.h"

SecKeyShm::SecKeyShm(key_t key, int maxNode) 
    : BaseShm(key, maxNode * sizeof(NodeSHMInfo)),
    maxNode(maxNode)
{
}
SecKeyShm::SecKeyShm(string path, int maxNode)
    : BaseShm(path, maxNode * sizeof(NodeSHMInfo))
{
}

SecKeyShm::~SecKeyShm()
{

}

void SecKeyShm::InitShm()
{
    if (this->ptr != NULL)
    {
        memset(this->ptr, 0, this->maxNode * sizeof(NodeSHMInfo));
    }
}

int SecKeyShm::ShmWrite(NodeSHMInfo *info)
{
    int ret = -1;
    // 1. 与共享内存建立连接
    NodeSHMInfo *pNode = NULL;
    pNode = static_cast<NodeSHMInfo *>(this->mapShm());
    if (NULL == pNode)
    {
        cerr << "ShmWrite::mapShm() failed" << endl;
        return NULL;
    }

    // 2. 如果在共享内存中存在数据，则覆盖
    NodeSHMInfo *temp;
    int i;
    for (i = 0; i < this->maxNode; i++)
    {
        temp = pNode + i;
        cout << "compare " << i << "segment memory" << endl;
        cout << "cliendId compare: " << temp->clientID << ", " << info->clientID << endl;
        cout << "serverId compare: " << temp->serverId << ", " << info->serverId << endl;
        if (strcmp(temp->clientID, info->clientID) == 0 &&
                strcmp(temp->serverID, info->serverID) == 0)
        {
            // 找到写入位置
            memcpy(temp, info, sizeof(NodeSHMInfo));
            unmapShm();
            cout << "new data cover old data" << endl;
            return 0;
        }
    }
    
    // 3. 如果在共享内存中不存在数据，则寻找一块空闲内存写入
    NodeSHMInfo tmpNodeInfo;
    for (i = 0; i < this->maxNode; i++)
    {
        temp = pNode + i;
        if (memcmp(temp, &tmpNodeInfo, sizeof(NodeSHMInfo)) == 0)
        {
            ret = 0;
            memcpy(temp, info, sizeof(NodeSHMInfo));
            unmapShm();
            cout << "write data from a new pos" << endl;
            break;
        }
    }

    if (i == maxNode)
    {
        ret = -1;
    }
    unmapShm();
    return ret;

}

NodeSHMInfo SecKeyShm::ShmRead(string clientId, string ServerId)
{
    int ret = 0;
	// 关联共享内存
	NodeSHMInfo *pAddr = NULL;
	pAddr = static_cast<NodeSHMInfo*>(mapShm());
	if (pAddr == NULL)
	{
		cout << "共享内存关联失败..." << endl;
		return NodeSHMInfo();
	}
	cout << "共享内存关联成功..." << endl;

	//遍历网点信息
	int i = 0;
	NodeSHMInfo info;
	NodeSHMInfo	*pNode = NULL;
	// 通过clientID和serverID查找节点
	cout << "maxNode: " << m_maxNode << endl;
	for (i = 0; i < m_maxNode; i++)
	{
		pNode = pAddr + i;
		cout << i << endl;
		cout << "clientID 比较: " << pNode->clientID << ", " << clientID.data() << endl;
		cout << "serverID 比较: " << pNode->serverID << ", " << serverID.data() << endl;
		if (strcmp(pNode->clientID, clientID.data()) == 0 &&
			strcmp(pNode->serverID, serverID.data()) == 0)
		{
			// 找到的节点信息, 拷贝到传出参数
			info = *pNode;
			cout << "++++++++++++++" << endl;
			cout << info.clientID << " , " << info.serverID << ", "
				<< info.seckeyID << ", " << info.status << ", "
				<< info.seckey << endl;
			cout << "===============" << endl;
			break;
		}
	}

	unmapShm();
	return info;
}
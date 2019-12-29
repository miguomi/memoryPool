#ifndef _MEMORY_MGR_hpp_
#define _MEMORY_MGR_hpp_

#define MAX_MEMORY_SIZE 2048
#define MAX_MEMORY_SIZE_64 64
#define MAX_MEMORY_SIZE_128 128
#define MAX_MEMORY_SIZE_256 256
#define MAX_MEMORY_SIZE_512 512
#define MAX_MEMORY_SIZE_1024 1024

#ifdef _DEBUG
  #include <stdio.h>
  #define xPrintf(...) printf(__VA_ARGS__);
#else
  #define xPrintf(...)
#endif //DEBUG

#include <iostream>
#include <assert.h>
#include <mutex>
class MemoryAlloc;
//�ڴ�飬��С��Ԫ
class MemoryBlock
{
public:
	MemoryBlock() {

	}
	~MemoryBlock() {
		
	}


	int _nID;//�ڴ����ID
	int _nRef;//���ô���
	MemoryAlloc* _pAlloc; //��������ڴ��
	MemoryBlock* _pNext;//��һ��λ��
	bool _bPool;//�Ƿ����ڴ����
private:
	char _cNull0;//Ԥ��
	char _cNull1;//Ԥ��
	char _cNull2;//Ԥ��


};





//�ڴ��
class MemoryAlloc
{
public:
	MemoryAlloc() {
		_pBuf = nullptr;
		_pHeader = nullptr;
		_nBlockNum = 0;
		_nEveryBlockMemorySize = 0;
	}
	~MemoryAlloc() {
		free(_pBuf);
	}

	//�����ڴ�
	void * allocMemory(size_t nSize) {
		std::lock_guard<std::mutex> lg(_lockMutex);
		if (nullptr == _pBuf) {
			std::cout << "pbuf is null" << std::endl;
			Initmemory();
		}

		MemoryBlock* pReturn = nullptr;
		//�����ڴ���
		if (nullptr == _pHeader) {
			pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
			pReturn->_bPool = false;
			pReturn->_nID = -1;
			pReturn->_nRef = 1;
			pReturn->_pAlloc = nullptr;
			pReturn->_pNext = nullptr;
		}
		else
		{
			pReturn = _pHeader;
			_pHeader = _pHeader->_pNext;
			assert(0 == pReturn->_nRef);
			pReturn->_nRef = 1;
		}

		return (void*)((char*)pReturn+sizeof(MemoryBlock));
	}
	//�ͷ��ڴ�
	void  freeMemory(void * pMem) {
		std::lock_guard<std::mutex> lg(_lockMutex);
		//free(p);
		char* pDate = (char*)pMem;
		
		MemoryBlock* pBlock = (MemoryBlock*)(pDate - sizeof(MemoryBlock));
		assert(1 == pBlock->_nRef);
		if (0 != (--pBlock->_nRef)){
			return;
		}

		if (false == pBlock->_bPool) {
			//free(pMem);
			free(pBlock);
		}
		else
		{
			pBlock->_pNext = _pHeader;
			pBlock->_nRef = 0;
			_pHeader = pBlock; 	
			
			//free(pMem);
		}
		
	}

	//��ʼ���ڴ��
	void Initmemory() {
		//std::lock_guard<std::mutex> lg(_lockMutex);
		assert(nullptr == _pBuf);
		if (nullptr != _pBuf) {
			return;
		}
		//��ϵͳ������ڴ�
		size_t bufSize = _nBlockNum*(_nEveryBlockMemorySize+sizeof(MemoryBlock));
		_pBuf = (char*)malloc(bufSize);
		//��ʼ���ڴ��
		_pHeader = (MemoryBlock*)_pBuf;
		_pHeader->_bPool = true;
		_pHeader->_nID = 0;
		_pHeader->_nRef = 0;
		_pHeader->_pAlloc = this;
		_pHeader->_pNext = nullptr;

		MemoryBlock* pTempBefore = _pHeader;
		for (size_t n = 1; n < _nBlockNum; n++) {
			//MemoryBlock* pTemp = (MemoryBlock*)(_pBuf + n * _nEveryBlockMemorySize);
			MemoryBlock* pTemp = (MemoryBlock*)(_pBuf + n * (_nEveryBlockMemorySize + sizeof(MemoryBlock)));
			pTemp->_bPool = true;
			pTemp->_nID = n;
			pTemp->_nRef = 0;
			pTemp->_pAlloc = this;
			pTemp->_pNext = nullptr;

			pTempBefore->_pNext = pTemp;
			pTempBefore = pTemp;
		}
		
	}

protected:
	//�ڴ�ص�ַ
	char* _pBuf;
	//�ڴ��ͷ��
	MemoryBlock* _pHeader;
	//�ڴ���������С
	size_t _nEveryBlockMemorySize ;
	//�ڴ�������
	size_t _nBlockNum;

	std::mutex _lockMutex;
};

template<size_t nEveryBlockMemorySize,size_t nBlockNum>
class MemoryAlloctor:public MemoryAlloc
{
public:
	MemoryAlloctor() {
		//�ֽڶ���
		const size_t n = sizeof(void*);
		_nEveryBlockMemorySize = (nEveryBlockMemorySize / n)*n+(nEveryBlockMemorySize%n ? n:0);
		
		//_nEveryBlockMemorySize = nEveryBlockMemorySize;
		_nBlockNum = nBlockNum;
	}
	virtual ~MemoryAlloctor() {

	}

private:

};


//�ڴ������
class MemoryMgr
{
private:
	MemoryMgr() {
		init(0, 64, &_mem64);
		init(65, 128, &_mem128);
		init(129, 256, &_mem256);
		init(257, 512,  &_mem512);
		init(513, 1024, &_mem1024);
		
	}
	virtual ~MemoryMgr() {

	}

	//��ʼ���ڴ��ӳ������
	void init(int nBegin,int nEnd, MemoryAlloc* pMemAlloc) {
		for (int n = nBegin; n <= nEnd; n++) {
			_szAlloc[n] = pMemAlloc;
			//szAlloc[n] = pMemAlloc;
		}
	}


public:
	//����ģʽ
	static MemoryMgr& Instance() {
		static MemoryMgr mgr;
		return mgr;
	}


	//�����ڴ�
	void * allocMemory(size_t nSize) {
		//return malloc(size);
		if (nSize <= MAX_MEMORY_SIZE) {
			return _szAlloc[nSize]->allocMemory(nSize);
		}
		else {
			printf("+++allocMemory\n");
			xPrintf("allocMemory\n");
			std::cout << "sizeof(MemoryBlock)[" << sizeof(MemoryBlock) << "]" << std::endl;
			MemoryBlock* pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
			pReturn->_bPool = false;
			pReturn->_nID = -1;
			pReturn->_nRef = 1;
			pReturn->_pAlloc = nullptr;
			pReturn->_pNext = nullptr;
			std::cout << "pReturn[" << pReturn << "]--allocReturn["<< (char*)pReturn + sizeof(MemoryBlock)<<"]" << std::endl;

			//freeMemory((char*)pReturn + sizeof(MemoryBlock));
			return (char*)pReturn + sizeof(MemoryBlock);
		}
	}
	//�ͷ��ڴ�
	void  freeMemory(void * pMem) {
		std::cout << "sizeof(MemoryBlock)[" << sizeof(MemoryBlock) << "]" << std::endl;
		std::cout << "freeMemory pMem[" << pMem << "]"<< std::endl;
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		std::cout << "freeMemory pMem[" << pMem << "]--pBlock[" << pBlock << "]" << std::endl;
		if (pBlock->_bPool) {
			pBlock->_pAlloc->freeMemory(pMem);
		}
		else {
			if (--pBlock->_nRef == 0) {
				free(pBlock);//TODO:Ӧ��ɾ��pBlock;
			}
		}
	}

	void addRef(void* pMem) {
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		pBlock->_nRef++;
	}

private:
	MemoryAlloctor<MAX_MEMORY_SIZE_64, 200> _mem64;
	MemoryAlloctor<MAX_MEMORY_SIZE_128, 200> _mem128;
	MemoryAlloctor<MAX_MEMORY_SIZE_256, 200> _mem256;
	MemoryAlloctor<MAX_MEMORY_SIZE_512, 200> _mem512;
	MemoryAlloctor<MAX_MEMORY_SIZE_1024, 200> _mem1024;

	MemoryAlloc* _szAlloc[MAX_MEMORY_SIZE + 1];
	
	
	
};





#endif
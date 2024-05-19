// z_unblock header
#pragma once
#ifndef _Z_UNBLOCK_H_
#define _Z_UNBLOCK_H_
#ifndef RC_INVOKED
#include "drzix.h"
#include "z_blockobj.h"
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
#pragma push_macro("new")
#undef new

_DRZIX_BEGIN
#define SET_RLEN 6
#define SET_CLEN 6
#define CHK_POS 0xFFFFFFFFF	// 36bit
#define ROW_CHK 0x60	// ħ���ϴ� ����
#define DST_POS 0x40000	// ��ǥ ��ġ
#define TOP_DOWN_ROW 0x40000000
#define BOTTOM_UP_ROW 0x20
#define DST_BLOCK 0
#define THREAD_NUM 100
using drzix::DIRECTION;

class z_unblock
	{
	typedef z_unblock _Myt;
	typedef std::vector<z_blockobj> _Vec;
	typedef std::pair<int, int> _Pair;
	typedef std::list<std::pair<int, int>> _List;
	typedef std::list<std::pair<int, int>>::iterator _Iter;
	typedef long long int _Myl;
	typedef std::unordered_map<std::string, int> _MemoMap;
	typedef std::pair<std::string, int> _MemoPair;

private:
	class z_blockstate
		{	// BFS ��忡 ���� ����ü
		public:
			// ��� ����Ʈ ����
			_Vec _BlockList;
			
			// Ʈ��ŷ
			_List _Process;

			// ����� �ִٸ� 1, ���ٸ� 0
			_Myl _State;
		};

	void check(const _Vec & _BlockList) const
		{	// ��� ����Ʈ�� �˻�
		int BlockSize;
		_Myl _CheckBit;
		if (_BlockList.empty())
			throw(1);
		for (int i = 0; i < (int)_BlockList.size(); ++i)
			{
			if (_BlockList[i]._Position > CHK_POS
				|| _BlockList[i]._Position <= 0)
				throw(1);
			// ��Ʈ������ �������� �����ϴ� 1�κ��� �Ž��� �ö�
			BlockSize = _BlockList[i].size();
			_CheckBit = _BlockList[i]._Position & -_BlockList[i]._Position;
			if (_BlockList[i].dir() == HORZ)
				{ /*HORZ*/
				for (int j = 1; j < BlockSize; ++j)
					_CheckBit |= _CheckBit << 1;
				// ���� ����� �ش� ��ϰ� �´��� �˻�
				if (_CheckBit != _BlockList[i]._Position)
					throw(1);
				for (int j = 0; j < (SET_RLEN - 1); ++j)
					{	// ���� �ʿ��Ͽ� �����ִ� ��Ʈ�� ���� ó��
					if ((_CheckBit & ROW_CHK) == ROW_CHK)
						throw(1);
					}
				}
			else /*VERT*/
				{
				for (int j = 1; j < BlockSize; ++j)
					_CheckBit |= _CheckBit << SET_CLEN;
				// ���� ����� �ش� ��ϰ� �´��� �˻�
				if (_CheckBit != _BlockList[i]._Position)
					throw(1);
				}
			}
		}

	void init_set(const _Vec & _BlockList, z_blockstate & _InitSet)
		{	// ��ϸ���Ʈ�� ���� ��ϵ��� ��ġ�� �ڸ��� ����
		_InitSet._State = 0;
		for (int i = 0; i < (int)_BlockList.size(); ++i)
			{
			// ��ġ�� ����� ������ ��� ��ġ ���� ����
			if (_InitSet._State & _BlockList[i]._Position)
				throw(1);
			_InitSet._State |= _BlockList[i]._Position;
			}
		}
	
	void set2str(std::string & _Str, _Vec & _Bvec)
		{	// �ؽ��� ���� ��Ʈ ���� ��ϵ��� ��ġ ����, ��Ʈ������ �Ľ�
		_Myl _Bpos;
		for (int i = 0; i < (int)_Bvec.size(); ++i)
			{
			_Bpos = _Bvec[i]._Position;
			_Str += 33 + i;	// ����� ����
			while (_Bpos > 0)
				{	// ��Ʈ 5���� �����Ͽ� ������Ŵ
				_Str += (_Bpos & 63) + 51;
				_Bpos >>= 5;
				}
			}
		}

	std::list<z_blockstate> bfs_queue, _TempBfsQueue[THREAD_NUM];
	std::list<std::thread*> _ThreadQueue;
	bool _DoneFlag;
	std::mutex _Mtx;

	void thread_manage()
		{	// ������ ť ���� �Լ�
		int thread_cnt = 0;
		while (true && _DoneFlag)
			{
			if (!_ThreadQueue.empty())
				{
				_Mtx.lock();
				if (_ThreadQueue.front()->joinable())
					_ThreadQueue.front()->join();
				_Mtx.unlock();
				// �����忡 �Ҵ�� �ӽ� ť�� ������ ��¥ ť�� �̽�
				bfs_queue.splice(bfs_queue.end(),_TempBfsQueue[thread_cnt]);
				if (++thread_cnt == THREAD_NUM)
					thread_cnt = 0;
				delete _ThreadQueue.front();
				_Mtx.lock();
				_ThreadQueue.pop_front();
				_Mtx.unlock();
				}
			}
		// ������ ã�� ���� ���� ���� �����带 ��� ����
		while (!_ThreadQueue.empty())
			{
			if (_ThreadQueue.front()->joinable())
				_ThreadQueue.front()->join();
			_ThreadQueue.pop_front();
			}
		}

	void myBFS(z_blockstate& _ground, int _thnum)
		{	// �����忡 �Ҵ��, BFS ���� ��� Ȯ�� �Լ�
			// ��ϵ��� ������ �� �ִ� �������� Nĭ �� ������ ��
		int _MovingCnt;
		_Myl _CheckWall, _TempPos, _TempSet = _ground._State;
		for (int i = 0; i < (int)_ground._BlockList.size(); ++i)
			{
			if (!_ground._Process.empty())
				{	// ��� ���� ������ ����� �ٽ� �����̷� �ϴ� ���
				if (i == _ground._Process.back().first)
					continue;
				}
			_TempPos = _ground._BlockList[i]._Position;
			_MovingCnt = 0;
			_ground._State = _TempSet;
			_ground._State -= _TempPos;
			if (_ground._BlockList[i].dir() == HORZ)
				{	/*HORZ*/
				_CheckWall = TOP_DOWN_ROW;
				while (_CheckWall > _TempPos)
					_CheckWall >>= SET_CLEN;
				// ������ ���� �ε����� Ȯ��
				while (!(_TempPos & _CheckWall))
					{	// ���������� �б� (+)
					++_MovingCnt;
					_TempPos >>= 1;
					// �ٸ� ��Ͽ� �ε����� Ȯ��
					if (!(_TempPos & _ground._State))
						{
						_TempBfsQueue[_thnum].push_back(_ground);
						_TempBfsQueue[_thnum].back()._State |= _TempPos;
						_TempBfsQueue[_thnum].back()._BlockList[i]._Position = _TempPos;
						_TempBfsQueue[_thnum].back()._Process.push_back(_Pair(i, _MovingCnt));
						}
					else
						break;
					}
				_TempPos = _ground._BlockList[i]._Position;
				_MovingCnt = 0;
				_CheckWall = BOTTOM_UP_ROW;
				while ((_CheckWall << 1) <= _TempPos)
					_CheckWall <<= SET_CLEN;
				// ���� ���� �ε����� Ȯ��
				while (!(_TempPos & _CheckWall))
					{	// �������� �б� (-)
					--_MovingCnt;
					_TempPos <<= 1;
					// �ٸ� ��Ͽ� �ε����� Ȯ��
					if (!(_TempPos & _ground._State))
						{
						_TempBfsQueue[_thnum].push_back(_ground);
						_TempBfsQueue[_thnum].back()._State |= _TempPos;
						_TempBfsQueue[_thnum].back()._BlockList[i]._Position = _TempPos;
						_TempBfsQueue[_thnum].back()._Process.push_back(_Pair(i, _MovingCnt));
						}
					else
						break;
					}
				}
			else /*VERT*/
				{
				_CheckWall = _TempPos & -_TempPos;
				while ((_CheckWall >> SET_CLEN) > 0)
					_CheckWall >>= SET_CLEN;
				// �Ʒ��� ���� �ε����� Ȯ��
				while (!(_TempPos & _CheckWall))
					{	// �Ʒ������� �б� (+)
					++_MovingCnt;
					_TempPos >>= SET_CLEN;
					// �ٸ� ��Ͽ� �ε����� Ȯ��
					if (!(_TempPos & _ground._State))
						{
						_TempBfsQueue[_thnum].push_back(_ground);
						_TempBfsQueue[_thnum].back()._State |= _TempPos;
						_TempBfsQueue[_thnum].back()._BlockList[i]._Position = _TempPos;
						_TempBfsQueue[_thnum].back()._Process.push_back(_Pair(i, _MovingCnt));
						}
					else
						break;
					}
				_TempPos = _ground._BlockList[i]._Position;
				_MovingCnt = 0;
				while (true)
					{	// �������� �б� (-)
					--_MovingCnt;
					_TempPos <<= SET_CLEN;
					// ���� ���� �ε����� Ȯ��, �ٸ� ��Ͽ� �ε����� Ȯ��
					if (!(_TempPos & _ground._State)
						&& _TempPos < CHK_POS)
						{
						_TempBfsQueue[_thnum].push_back(_ground);
						_TempBfsQueue[_thnum].back()._State |= _TempPos;
						_TempBfsQueue[_thnum].back()._BlockList[i]._Position = _TempPos;
						_TempBfsQueue[_thnum].back()._Process.push_back(_Pair(i, _MovingCnt));
						}
					else
						break;
					}
				}
			}
		}

	_List find(const _Vec & _BlockList)
		{	// ���� ã�� �Լ�
		z_blockstate ground;
		_MemoMap mem_map;
		std::string set_string;
		_DoneFlag = true;
		std::thread manage(&_Myt::thread_manage, this);
		int bfs_thread_cnt = 0;
		bfs_queue.clear();
		ground._BlockList = _BlockList;
		init_set(ground._BlockList, ground);
		bfs_queue.push_back(ground);
		while (true)
			{
			if (bfs_queue.empty())
				{	// ť�� ����� ���, ���� �ִ� �����带 Ȯ��
				_Mtx.lock();
				if (bfs_queue.empty() && _ThreadQueue.empty())
					{	// BFS�� ��� �����ٸ� ������ ���� ���
					_Mtx.unlock();
					_DoneFlag = false;
					manage.join();
					return (_List(0));
					}
				_Mtx.unlock();
				continue;
				}
			// �̹� ������ ������Ʈ���� Ȯ��
			set2str(set_string, bfs_queue.front()._BlockList);
			if (mem_map.find(set_string) == mem_map.end())
				{
				if (DST_POS & bfs_queue.front()._BlockList[DST_BLOCK]._Position)
					{	// ��ǥ ����� ��ǥ ��ǥ�� ���� �ִ� ��� ���信 ����, ��� ����Ʈ ����
					_DoneFlag = false;
					manage.join();
					return bfs_queue.front()._Process;
					}
				mem_map.insert(_MemoPair(set_string, NULL));
				while (_ThreadQueue.size() > THREAD_NUM);
				_Mtx.lock();
				// ������ �Ҵ��ϰ� BFS ť�� ����
				_ThreadQueue.push_back(
					new std::thread(&_Myt::myBFS, this, bfs_queue.front(), bfs_thread_cnt));
				_Mtx.unlock();
				if (++bfs_thread_cnt == THREAD_NUM)
					bfs_thread_cnt = 0;
				}
			set_string.clear();
			bfs_queue.pop_front();
			}
		}

public:
	void operator()(const _Vec & _BlockList)
		{
		try
			{
			check(_BlockList);
			_List dst = find(_BlockList);
			if (dst != (_List)0)
				{
				int _BestCnt = 0;
				for (_Iter iter = dst.begin(); iter != dst.end(); iter = ++iter)
					{
					if (_BlockList[iter->first].dir() == HORZ)
						{
						if (iter->second > 0)
							std::cout << ++_BestCnt << " : " << iter->first << "�� ����� ���������� " << iter->second << "ĭ �̵�\n";
						else
							std::cout << ++_BestCnt << " : " << iter->first << "�� ����� �������� " << -(iter->second) << "ĭ �̵�\n";
						}
					else
						{
						if (iter->second > 0)
							std::cout << ++_BestCnt << " : " << iter->first << "�� ����� �Ʒ������� " << iter->second << "ĭ �̵�\n";
						else
							std::cout << ++_BestCnt << " : " << iter->first << "�� ����� �������� " << -(iter->second) << "ĭ �̵�\n";
						}
					}
				}
			else
				std::cout << "���\n";
			}
		catch (int e)
			{
			switch (e)
				{
				case 1:				
					std::cout << "��� ��ġ ����\n";
					break;
				default:;
				}
			}
		}
	};
	z_unblock unblocker;
_DRZIX_END
#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _Z_UNBLOCK_H_ */
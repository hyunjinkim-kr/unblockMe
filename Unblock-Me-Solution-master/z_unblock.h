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
#define ROW_CHK 0x60	// 침범하는 가로
#define DST_POS 0x40000	// 목표 위치
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
		{	// BFS 노드에 대한 구조체
		public:
			// 블록 리스트 벡터
			_Vec _BlockList;
			
			// 트래킹
			_List _Process;

			// 블록이 있다면 1, 없다면 0
			_Myl _State;
		};

	void check(const _Vec & _BlockList) const
		{	// 블록 리스트를 검사
		int BlockSize;
		_Myl _CheckBit;
		if (_BlockList.empty())
			throw(1);
		for (int i = 0; i < (int)_BlockList.size(); ++i)
			{
			if (_BlockList[i]._Position > CHK_POS
				|| _BlockList[i]._Position <= 0)
				throw(1);
			// 비트단위로 마지막에 출현하는 1로부터 거슬러 올라감
			BlockSize = _BlockList[i].size();
			_CheckBit = _BlockList[i]._Position & -_BlockList[i]._Position;
			if (_BlockList[i].dir() == HORZ)
				{ /*HORZ*/
				for (int j = 1; j < BlockSize; ++j)
					_CheckBit |= _CheckBit << 1;
				// 명세한 블록이 해당 블록과 맞는지 검사
				if (_CheckBit != _BlockList[i]._Position)
					throw(1);
				for (int j = 0; j < (SET_RLEN - 1); ++j)
					{	// 행을 초월하여 걸쳐있는 비트를 예외 처리
					if ((_CheckBit & ROW_CHK) == ROW_CHK)
						throw(1);
					}
				}
			else /*VERT*/
				{
				for (int j = 1; j < BlockSize; ++j)
					_CheckBit |= _CheckBit << SET_CLEN;
				// 명세한 블록이 해당 블록과 맞는지 검사
				if (_CheckBit != _BlockList[i]._Position)
					throw(1);
				}
			}
		}

	void init_set(const _Vec & _BlockList, z_blockstate & _InitSet)
		{	// 블록리스트에 명세된 블록들을 위치한 자리에 세팅
		_InitSet._State = 0;
		for (int i = 0; i < (int)_BlockList.size(); ++i)
			{
			// 겹치는 블록이 있으면 블록 위치 오류 예외
			if (_InitSet._State & _BlockList[i]._Position)
				throw(1);
			_InitSet._State |= _BlockList[i]._Position;
			}
		}
	
	void set2str(std::string & _Str, _Vec & _Bvec)
		{	// 해쉬를 위해 세트 위의 블록들의 위치 참조, 스트링으로 파싱
		_Myl _Bpos;
		for (int i = 0; i < (int)_Bvec.size(); ++i)
			{
			_Bpos = _Bvec[i]._Position;
			_Str += 33 + i;	// 블록의 종류
			while (_Bpos > 0)
				{	// 비트 5개씩 참조하여 대응시킴
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
		{	// 스레드 큐 관리 함수
		int thread_cnt = 0;
		while (true && _DoneFlag)
			{
			if (!_ThreadQueue.empty())
				{
				_Mtx.lock();
				if (_ThreadQueue.front()->joinable())
					_ThreadQueue.front()->join();
				_Mtx.unlock();
				// 스레드에 할당된 임시 큐의 노드들을 진짜 큐로 이식
				bfs_queue.splice(bfs_queue.end(),_TempBfsQueue[thread_cnt]);
				if (++thread_cnt == THREAD_NUM)
					thread_cnt = 0;
				delete _ThreadQueue.front();
				_Mtx.lock();
				_ThreadQueue.pop_front();
				_Mtx.unlock();
				}
			}
		// 정답을 찾고 나면 진행 중인 스레드를 모두 종료
		while (!_ThreadQueue.empty())
			{
			if (_ThreadQueue.front()->joinable())
				_ThreadQueue.front()->join();
			_ThreadQueue.pop_front();
			}
		}

	void myBFS(z_blockstate& _ground, int _thnum)
		{	// 스레드에 할당된, BFS 중인 노드 확인 함수
			// 블록들을 움직일 수 있는 방향으로 N칸 씩 움직여 봄
		int _MovingCnt;
		_Myl _CheckWall, _TempPos, _TempSet = _ground._State;
		for (int i = 0; i < (int)_ground._BlockList.size(); ++i)
			{
			if (!_ground._Process.empty())
				{	// 방금 전에 움직인 블록을 다시 움직이려 하는 경우
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
				// 오른쪽 벽에 부딪히나 확인
				while (!(_TempPos & _CheckWall))
					{	// 오른쪽으로 밀기 (+)
					++_MovingCnt;
					_TempPos >>= 1;
					// 다른 블록에 부딪히나 확인
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
				// 왼쪽 벽에 부딪히나 확인
				while (!(_TempPos & _CheckWall))
					{	// 왼쪽으로 밀기 (-)
					--_MovingCnt;
					_TempPos <<= 1;
					// 다른 블록에 부딪히나 확인
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
				// 아래쪽 벽에 부딪히나 확인
				while (!(_TempPos & _CheckWall))
					{	// 아래쪽으로 밀기 (+)
					++_MovingCnt;
					_TempPos >>= SET_CLEN;
					// 다른 블록에 부딪히나 확인
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
					{	// 위쪽으로 밀기 (-)
					--_MovingCnt;
					_TempPos <<= SET_CLEN;
					// 위쪽 벽에 부딪히나 확인, 다른 블록에 부딪히나 확인
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
		{	// 답을 찾는 함수
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
				{	// 큐가 비었을 경우, 돌고 있는 스레드를 확인
				_Mtx.lock();
				if (bfs_queue.empty() && _ThreadQueue.empty())
					{	// BFS가 모두 끝났다면 정답이 없는 경우
					_Mtx.unlock();
					_DoneFlag = false;
					manage.join();
					return (_List(0));
					}
				_Mtx.unlock();
				continue;
				}
			// 이미 출현한 스테이트인지 확인
			set2str(set_string, bfs_queue.front()._BlockList);
			if (mem_map.find(set_string) == mem_map.end())
				{
				if (DST_POS & bfs_queue.front()._BlockList[DST_BLOCK]._Position)
					{	// 목표 블록이 목표 좌표를 덮고 있는 경우 정답에 도달, 경로 리스트 리턴
					_DoneFlag = false;
					manage.join();
					return bfs_queue.front()._Process;
					}
				mem_map.insert(_MemoPair(set_string, NULL));
				while (_ThreadQueue.size() > THREAD_NUM);
				_Mtx.lock();
				// 스레드 할당하고 BFS 큐에 삽입
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
							std::cout << ++_BestCnt << " : " << iter->first << "번 블록을 오른쪽으로 " << iter->second << "칸 이동\n";
						else
							std::cout << ++_BestCnt << " : " << iter->first << "번 블록을 왼쪽으로 " << -(iter->second) << "칸 이동\n";
						}
					else
						{
						if (iter->second > 0)
							std::cout << ++_BestCnt << " : " << iter->first << "번 블록을 아래쪽으로 " << iter->second << "칸 이동\n";
						else
							std::cout << ++_BestCnt << " : " << iter->first << "번 블록을 위쪽으로 " << -(iter->second) << "칸 이동\n";
						}
					}
				}
			else
				std::cout << "노답\n";
			}
		catch (int e)
			{
			switch (e)
				{
				case 1:				
					std::cout << "블록 위치 오류\n";
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
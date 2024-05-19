// z_blockobj header
#pragma once
#ifndef _Z_BLOCKOBJ_H_
#define _Z_BLOCKOBJ_H_
#ifndef RC_INVOKED
#include "drzix.h"

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
#pragma push_macro("new")
#undef new

_DRZIX_BEGIN
enum DIRECTION
	{
	HORZ,
	VERT
	};

class z_blockobj
	{
	typedef long long int _Myl;
	typedef z_blockobj _Myt;
	
	private:
		// ����� ����
		int _Direction;

		// ����� ����
		int _Size;

	public:
		// ����� ��ġ
		_Myl _Position;

		// z_blockobj �⺻ ������
		_Myt(int _dir, int _siz, _Myl _pos)
			: _Direction(_dir),
			_Size(_siz),
			_Position(_pos)
			{
			}

		inline int dir() const
			{
			return _Direction;
			}

		inline int size() const
			{
			return _Size;
			}
	};
_DRZIX_END
#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _Z_BLOCKOBJ_H_ */
//해야할 일
//미친 스레드

#include <iostream>
#include "z_unblock.h"

int main()
	{
	std::ios_base::sync_with_stdio(false);
	std::vector<drzix::z_blockobj> qwerqwerqwer;

	/*qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, std::pair<int, int>(0, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, std::pair<int, int>(2, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 3, std::pair<int, int>(1, 1)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, std::pair<int, int>(0, 3)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, std::pair<int, int>(4, 4)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(0, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(4, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(2, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(3, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(4, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(3, 4)));*/

	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, 0x300000));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, 0xC00000000));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 3, 0x7000));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, 0x30));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, 0xC0));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, 0x208000000));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, 0x2080000));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 3, 0x41040000));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 3, 0x820800));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, 0x10400));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, 0x208));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, 0x104));

	/*qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, std::pair<int, int>(0, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(0, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, std::pair<int, int>(1, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, std::pair<int, int>(3, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(2, 1)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 3, std::pair<int, int>(3, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 3, std::pair<int, int>(4, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 3, std::pair<int, int>(0, 4)));*/

	/*qwerqwerqwer.push_back(drzix::z_blockobj(drzix::HORZ, 2, 0x3));*/

	/*qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(0, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(0, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(1, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(1, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(2, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(2, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(3, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(3, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(4, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(4, 2)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(5, 0)));
	qwerqwerqwer.push_back(drzix::z_blockobj(drzix::VERT, 2, std::pair<int, int>(5, 2)));*/
	drzix::unblocker(qwerqwerqwer);

	return 0;
	}
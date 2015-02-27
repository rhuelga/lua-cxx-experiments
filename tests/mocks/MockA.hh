/*********************************************************************
 *  File:        MockA.hh
 *  created:     Wed Feb 25 12:58:20 2015
 *
 *  Author:      Roberto Huelga Diaz <rhuelga@gmail.com>
 *
 *  Description:
 *
 *********************************************************************/

#pragma once

#include <gmock/gmock.h>

#include <iostream>

class MockA {
public:

	MOCK_METHOD0(callMe, void());
	MOCK_METHOD0(Die, void());

	MockA() {
		std::cout << "Created MockA : " << this << std::endl;

		//EXPECT_CALL(*this, Die()).Times(1);
	}
	~MockA() {
		std::cout << "Destroy MockA : " << this << std::endl;

		Die();
	}
};

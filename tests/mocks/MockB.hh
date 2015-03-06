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

#include "MockA.hh"

class MockB : public MockA {
public:

	MOCK_METHOD0(callMeB, void());
	MOCK_METHOD0(Die, void());

	MockB() {
		std::cout << "Created MockB : " << this << std::endl;

		//EXPECT_CALL(*this, Die()).Times(1);
	}

	~MockB() {
		std::cout << "Destroy MockB : " << this << std::endl;

		Die();
	}
};

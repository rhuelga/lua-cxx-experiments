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

class MockA {
public:
	MOCK_METHOD0(callMe, void());
	MOCK_METHOD0(die, void());

	~MockA() { die(); }
};

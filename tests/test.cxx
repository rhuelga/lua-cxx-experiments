//-*- c-basic-offset: 4 -*-
/*********************************************************************
 *  File:        test.cxx
 *  created:     Mon Feb 23 22:56:31 2015
 *
 *  Author:      Roberto Huelga Diaz <rhuelga@gmail.com>
 *
 *  Description:
 *
 *********************************************************************/

#define __TEST__

#include <bandit/bandit.h>
#include <gmock/gmock.h>

#include "mocks/MockA.hh"

using namespace bandit;


// Tell bandit there are tests here.
go_bandit([](){
	// We're describing how a fuzzbox works.
	describe("MockA:", [](){
		std::shared_ptr<MockA> mocka;

		// Make sure each test has a fresh setup with
		// a guitar with a fuzzbox connected to it.
		before_each([&](){
			mocka = std::make_shared<MockA>();
		    });

		it("Should call callMe", [&](){
			EXPECT_CALL(*mocka, callMe()).Times(1);
			mocka->callMe();
		    });

		it("Should be destroy", [&]() {
			EXPECT_CALL(*mocka, die()).Times(1);
		    });
	    });

	describe("Call from lua", []() {
		std::shared_ptr<MockA> mocka;

		before_each([&]() {
			mocka = std::make_shared<MockA>();
		    });

	    });

    });

int main(int argc, char* argv[]) {
    // Run the tests.
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);
    return bandit::run(argc, argv);
}

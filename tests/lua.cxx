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

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

using namespace bandit;

lua_State *L;

// Tell bandit there are tests here.
go_bandit([](){
	// We're describing how a fuzzbox works.
	describe("MockA + Lua", []() {

		it("Should lua push global number", [&]() {
			const char *code = "function num() return 10 end; print(\"holaaaaaaaaaaaa\"); _G.hola = 10";
		 	luaL_loadstring(L, code);
			if(0 != lua_pcall(L, 0, LUA_MULTRET, 0)) {
			    std::cerr << "--Lua error from C++ file " __FILE__ ":" << __LINE__ << std::endl;
			}

			lua_getglobal(L, "hola");

			std::cerr << "Lua Stack size for num global  is: " << lua_gettop(L) << std::endl;

			lua_pop(L, -1);
		    });
	    });
    });

int main(int argc, char* argv[]) {
    L = luaL_newstate();
    luaL_openlibs(L);

    //luaL_requiref(L, MOCKA, &luaopen_mockalib, 1);
    //lua_pop(L, 1);

    // Run the tests.
    // ::testing::GTEST_FLAG(throw_on_failure) = true;
    // ::testing::InitGoogleMock(&argc, argv);

    int bandit_ret = bandit::run(argc, argv);

    std::cout << "Bandit runs end" << std::endl;
    lua_close(L);

    return bandit_ret;
}

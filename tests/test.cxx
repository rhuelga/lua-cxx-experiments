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

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "mocks/MockA.hh"

using namespace bandit;

lua_State *L;

#define MOCKA "MockA"

typedef std::shared_ptr<MockA> MockA_sp;

static MockA_sp push_mocka(lua_State *L, MockA_sp mocka) {
    MockA_sp **pmock = reinterpret_cast<MockA_sp** >(lua_newuserdata(L, sizeof(MockA_sp *)));

    *pmock = new MockA_sp(mocka);

    // [LUA5.2] Change next 2 lines with luaL_setmetatable(L, MOCKA)
    luaL_getmetatable(L, MOCKA);
    lua_setmetatable(L, -2);

    return **pmock;
}

static MockA_sp check_mocka(lua_State *L, int index) {
    MockA_sp *mocka;
    luaL_checktype(L, index, LUA_TUSERDATA);
    void* udata = luaL_checkudata(L, index, MOCKA);

    if(udata != NULL) {
	mocka = *reinterpret_cast<MockA_sp **>(udata);
    } else {
	luaL_error(L, "error");
	return MockA_sp();  // return empty shared_ptr
    }

    return *mocka;
}

// Lua bind to MockA
static int mocka_new(lua_State *l) {
    MockA_sp mock = std::make_shared<MockA>();

    push_mocka(L, mock);

    return 1;
}

static int mocka_call_me(lua_State *L) {
    MockA_sp mock = check_mocka(L, 1);
    luaL_argcheck(L, mock != NULL, 1, "Error not MockA");

    std::cout << "Call me called" << std::endl;
    mock->callMe();
    return 0;
}

static int mocka_gc(lua_State *L) {
    void *udata = luaL_checkudata(L,1,MOCKA);
    luaL_argcheck(L, udata != NULL, 1, "Error gc MockA");
    if(udata != NULL) {
	MockA_sp *pmock = *reinterpret_cast<MockA_sp **>(udata);
	delete pmock;
    } else {
	std::cerr << "mocka_gc error destroy not mocka" << std::endl;
    }

    return 0;
}

int luaopen_mockalib(lua_State *L) {

    static const luaL_Reg LuaLib_MockA[] = {
	{ "new", &mocka_new },
	{ "call_me", &mocka_call_me },
	{ "__gc", &mocka_gc},
	{ NULL, NULL }
    };

    // [LUA5.2] should use luaL_newlib(L,R)
    // luaL_newlib(L, LuaLib_MockA);

    luaL_newmetatable(L, MOCKA);
    luaL_register(L, NULL, LuaLib_MockA);

    lua_pushvalue(L, -1);
    lua_setfield(L, -1, "__index");

    lua_setglobal(L, MOCKA);

    return 1;
}

// Tell bandit there are tests here.
go_bandit([](){

	describe("MockA:", [](){
		MockA_sp mocka;

		before_each([&](){
			mocka = std::make_shared<MockA>();
		    });

		it("Should call callMe", [&](){
			EXPECT_CALL(*mocka, callMe()).Times(1);
			mocka->callMe();
		    });

		it("Should be destroy", [&]() {
		 	EXPECT_CALL(*mocka, Die()).Times(1);
		    });
	    });

	describe("MockA + Lua", []() {

		it("Should lua push global number", [&]() {
			const char *code = "function num() return 10 end; print(\"holaaaaaaaaaaaa\"); _G.hola = 10";
		 	luaL_loadstring(L, code);
			if(0 != lua_pcall(L, 0, LUA_MULTRET, 0)) {
			    std::cerr << "--Lua error from C++ file " __FILE__ ":" << __LINE__ << std::endl;
			}

			lua_getglobal(L, "hola");

			AssertThat(lua_gettop(L), Equals(1));
			AssertThat(lua_tonumber(L, -1), Equals(10));

			lua_pop(L, -1);
		    });
		it("Should callMe from lua", [&]() {
			const char *code = "mock = MockA.new()";
		 	luaL_loadstring(L, code);
			if(0 != lua_pcall(L, 0, LUA_MULTRET, 0)) {
			    std::cerr << "--Lua error from C++ file " __FILE__ ":" << __LINE__ << std::endl;
			}

			lua_getglobal(L, "mock");
			std::cerr << "Lua Stack size is: " << lua_gettop(L) << std::endl;

			MockA_sp mocka = check_mocka(L,1);
			EXPECT_CALL(*mocka, Die()).Times(1);
			lua_pop(L,1);

		  	luaL_dostring(L, "mock = nil; collectgarbage();");
		 	luaL_dostring(L, "collectgarbage()");

		    });

		it("Should callMe from lua", [&]() {
			MockA_sp mock2 = std::make_shared<MockA>();
			push_mocka(L, mock2);

			lua_setglobal(L, "mock2");

			EXPECT_CALL(*mock2, callMe()).Times(1);

			const char *code = "mock2:call_me()";
			luaL_dostring(L, code);
		    });

		it("Should die on grabage collect", [&]() {
			MockA *pmock;
			{
			    MockA_sp mock3 = std::make_shared<MockA>();
			    push_mocka(L, mock3);
			    pmock = mock3.get();
			}


			lua_setglobal(L, "mock3");

			EXPECT_CALL(*pmock, Die()).Times(1);

			const char *code = "mock3 = nil; collectgarbage()";
			luaL_dostring(L, code);
		    });
	    });
    });

int main(int argc, char* argv[]) {
    L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_mockalib(L);

    //luaL_requiref(L, MOCKA, &luaopen_mockalib, 1);
    //lua_pop(L, 1);

    // Run the tests.
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    int bandit_ret = bandit::run(argc, argv);

    std::cout << "Bandit runs end" << std::endl;
    lua_close(L);

    return bandit_ret;
}

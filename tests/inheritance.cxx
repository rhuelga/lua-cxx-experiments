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
#include "mocks/MockB.hh"

#define LUA_EXECSTRING(str)			\
    luaL_loadstring(L, str);			\
    if(0 != lua_pcall(L, 0, LUA_MULTRET, 0)) {				\
	std::cerr << "--Lua error from C++ file " __FILE__ ":" << __LINE__ << std::endl; \
	std::cerr << luaL_checkstring(L,-1) << std::endl;		\
    }

using namespace bandit;

lua_State *L;

#define MOCKA "test.MockA"
#define MOCKB "test.MockB"

typedef std::shared_ptr<MockA> MockA_sp;
typedef std::shared_ptr<MockB> MockB_sp;

static MockA_sp push_mocka(lua_State *L, MockA_sp mocka) {
    MockA_sp **pmock = reinterpret_cast<MockA_sp** >(lua_newuserdata(L, sizeof(MockA_sp *)));

    *pmock = new MockA_sp(mocka);

    // [LUA5.2] Change next 2 lines with luaL_setmetatable(L, MOCKA)
    luaL_getmetatable(L, MOCKA);
    lua_setmetatable(L, -2);

    return **pmock;
}

void *
klua_checkudata_inheritance(lua_State *L, int index, const char *clas) {
    int reset = lua_gettop(L);

    if(index < 0) {	// convert index to positive
	index = reset + index + 1;
    }


    luaL_getmetatable(L, clas);
    int target = lua_gettop(L);

    lua_pushvalue(L, index);
    lua_gettop(L);

    void *ret = nullptr;

    while(lua_getmetatable(L, -1)) {
	if(lua_equal(L, target, -1)) {
	    ret = lua_touserdata(L, index);
	    break;
	}
    }

    lua_settop(L,reset);

    return ret;
}

static MockA_sp check_mocka(lua_State *L, int index) {
    MockA_sp *mocka;
    luaL_checktype(L, index, LUA_TUSERDATA);
    //void* udata = luaL_checkudata(L, index, MOCKB);
    void* udata = klua_checkudata_inheritance(L, index, MOCKA);

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

static MockB_sp push_mockb(lua_State *L, MockB_sp mockb) {
    MockB_sp **pmock = reinterpret_cast<MockB_sp** >(lua_newuserdata(L, sizeof(MockB_sp *)));

    *pmock = new MockB_sp(mockb);

    // [LUA5.2] Change next 2 lines with luaL_setmetatable(L, MOCKB)
    luaL_getmetatable(L, MOCKB);
    lua_setmetatable(L, -2);

    return **pmock;
}

static MockB_sp check_mockb(lua_State *L, int index) {
    MockB_sp *mockb;
    luaL_checktype(L, index, LUA_TUSERDATA);
    void* udata = luaL_checkudata(L, index, MOCKB);

    if(udata != NULL) {
	mockb = *reinterpret_cast<MockB_sp **>(udata);
    } else {
	luaL_error(L, "error");
	return MockB_sp();  // return empty shared_ptr
    }

    return *mockb;
}

// Lua bind to MockB
static int mockb_new(lua_State *l) {
    MockB_sp mock = std::make_shared<MockB>();

    push_mockb(L, mock);

    return 1;

}

static int mockb_call_me_b(lua_State *L) {
    MockB_sp mock = check_mockb(L, 1);
    luaL_argcheck(L, mock != NULL, 1, "Error not MockB");

    std::cout << "Call me called" << std::endl;
    mock->callMeB();
    return 0;
}

static int mockb_gc(lua_State *L) {
    void *udata = luaL_checkudata(L,1,MOCKB);
    luaL_argcheck(L, udata != NULL, 1, "Error gc MockB");
    if(udata != NULL) {
	MockB_sp *pmock = *reinterpret_cast<MockB_sp **>(udata);
	delete pmock;
    } else {
	std::cerr << "mockb_gc error destroy not mockb" << std::endl;
    }

    return 0;
}

int luaopen_mockalib(lua_State *L) {

    static const luaL_Reg LuaLib_MockA_methods[] = {
	//{ "new", &mocka_new },
	{ "call_me", &mocka_call_me },
	{ "__gc", &mocka_gc},
	{ NULL, NULL }
    };

    static const luaL_Reg LuaLib_MockA_statics[] = {
	{ "new", &mocka_new },
	{ NULL, NULL }
    };

    static const luaL_Reg LuaLib_MockB_methods[] = {
	//{ "new", &mocka_new },
	{ "call_me_b", &mockb_call_me_b },
	{ "__gc", &mockb_gc},
	{ NULL, NULL }
    };

    static const luaL_Reg LuaLib_MockB_statics[] = {
	{ "new", &mockb_new },
	{ NULL, NULL }
    };

    // [LUA5.2] should use luaL_newlib(L,R)
    // luaL_newlib(L, LuaLib_MockA);

    // Create Package
    lua_newtable(L);			// Create Package table
    int testtab = lua_gettop(L);
    lua_pushvalue(L, -1);		// duplicate table in stack
    lua_setglobal(L, "test");		// Set global Pacakge name as "test", and pop


    // Create MockA
    luaL_newmetatable(L, MOCKA);	// create metatable and name it
    lua_pushvalue(L, -1);		// duplicate metatable in the stack
    lua_setfield(L, -2, "__index");	// pop one of those copies and assign it to __index
					// of the 1st metatable

    luaL_register(L, NULL, LuaLib_MockA_methods);  // register functions in the metatable

    lua_newtable(L);				// Create the table for static methods of MockA
    luaL_register(L, NULL, LuaLib_MockA_statics); // Set all statics methods of MockA
    lua_setfield(L, testtab, "MockA");		// test.MockA = this new table

    lua_settop(L, testtab);		// Pop Stack till testtab

    //Create MockB
    luaL_newmetatable(L, MOCKB);	// create metatable and name it
    lua_pushvalue(L, -1);		// duplicate metatable in the stack
    lua_setfield(L, -2, "__index");	// pop one of those copies and assign it to __index
					// of the 1st metatable

    luaL_register(L, NULL, LuaLib_MockB_methods);  // register functions in the metatable

    luaL_getmetatable(L, MOCKA);
    lua_setmetatable(L, -2);

    lua_newtable(L);				// Create the table for static methods of MockA
    luaL_register(L, NULL, LuaLib_MockB_statics); // Set all statics methods of MockA
    lua_setfield(L, testtab, "MockB");		// test.MockA = this new table

    lua_settop(L, testtab);		// Pop Stack till testtab

    lua_settop(L, 0);			// Empty Stack
    return 1;
}

// Tell bandit there are tests here.
go_bandit([](){

	xdescribe("MockA:", [](){
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
			const char *code = "mock = test.MockA.new()";
		 	luaL_loadstring(L, code);
			if(0 != lua_pcall(L, 0, LUA_MULTRET, 0)) {
			    std::cerr << "--Lua error from C++ file " __FILE__ ":" << __LINE__ << std::endl;
			    std::cerr << luaL_checkstring(L,-1);
			}

			lua_getglobal(L, "mock");
			std::cerr << "Lua Stack size is: " << lua_gettop(L) << std::endl;

			MockA_sp mocka = check_mocka(L,1);
			EXPECT_CALL(*mocka, Die()).Times(1);
			lua_pop(L,1);

		  	luaL_dostring(L, "mock = nil; collectgarbage();");
		 	luaL_dostring(L, "collectgarbage()");

		    });

		it("Should callMe from lua, created from C", [&]() {
			MockA_sp mock2 = std::make_shared<MockA>();
			push_mocka(L, mock2);

			lua_setglobal(L, "mock2");

			EXPECT_CALL(*mock2, callMe()).Times(1);

			const char *code = "mock2:call_me()";
			luaL_dostring(L, code);
		    });
		it("Should callMe from lua, created from Lua", [&]() {
			LUA_EXECSTRING("mock2 = test.MockA.new()");

			lua_getglobal(L, "mock2");

			MockA_sp mock2 = check_mocka(L, -1);
			EXPECT_CALL(*mock2, callMe()).Times(1);
			std::cerr << "mock2 callme on pointer " << mock2.get() << std::endl;

			lua_pop(L, 1);

			LUA_EXECSTRING("mock2:call_me()");
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
		it("Should inhereted callMeB from lua", [&]() {
			LUA_EXECSTRING("mock6 = test.MockB.new()")

			lua_getglobal(L, "mock6");
			std::cerr << "Lua Stack size is: " << lua_gettop(L) << std::endl;

			MockB_sp mockb = check_mockb(L,1);
			EXPECT_CALL(*mockb, callMeB()).Times(1);
			lua_pop(L,1);

			LUA_EXECSTRING("mock6:call_me_b()");
		    });

		it("Should inhereted callMe from lua", [&]() {
			LUA_EXECSTRING("mock6 = test.MockB.new()")

			    lua_getglobal(L, "mock6");
			std::cerr << "Lua Stack size is: " << lua_gettop(L) << std::endl;

			MockB_sp mockb = check_mockb(L,1);
			EXPECT_CALL(*mockb, callMe()).Times(1);
			lua_pop(L,1);

			LUA_EXECSTRING("mock6:call_me()");
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

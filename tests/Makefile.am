TESTS = test lua inheritance
check_PROGRAMS = test lua inheritance

test_SOURCES = test.cxx
test_CXXFLAGS = ${LIBLUAJIT_CFLAGS} ${TEST_CFLAGS} ${GMOCK_CFLAGS}
test_LDFLAGS = ${LIBLUAJIT_LIBS} ${GMOCK_LIBS}

lua_SOURCES = lua.cxx
lua_CXXFLAGS = ${LIBLUAJIT_CFLAGS} ${TEST_CFLAGS} ${GMOCK_CFLAGS}
lua_LDFLAGS = ${LIBLUAJIT_LIBS} ${GMOCK_LIBS}

inheritance_SOURCES = inheritance.cxx
inheritance_CXXFLAGS = ${LIBLUAJIT_CFLAGS} ${TEST_CFLAGS} ${GMOCK_CFLAGS}
inheritance_LDFLAGS = ${LIBLUAJIT_LIBS} ${GMOCK_LIBS}

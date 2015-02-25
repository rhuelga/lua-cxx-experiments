
bin_PROGRAMS = run

TESTS = test
check_PROGRAMS = test

run_SOURCES = run.cxx

run_CXXFLAGS = ${LIBLUAJIT_CFLAGS}
run_LDFLAGS = ${LIBLUAJIT_LIBS}

test_SOURCES = test.cxx

test_CXXFLAGS = ${TEST_CFLAGS} ${GMOCK_CFLAGS}
test_LDFLAGS = ${GMOCK_LIBS}

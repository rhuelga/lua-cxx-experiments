/*********************************************************************
 *  File:        run.cxx
 *  created:     Mon Jan 26 18:23:35 2015
 *
 *  Author:      Roberto Huelga Diaz <rhuelga@gmail.com>
 *
 *  Description:
 *
 *********************************************************************/

#define __RUN__

#include <iostream>

#include <stdio.h>
#include <curses.h>
#include <unistd.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


int kbhit() {
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}

int main (int argc, char **argv) {
	if (argc > 1) {
		std::cout << "string command '" << argv[1] << "'" << std::endl;

		lua_State *L = luaL_newstate();

		if(!L) {
			std::cout << "newstate return NULL" << std::endl;
		}
		luaL_openlibs(L);


		int i =0;
		int count = 0;

		while(!i) {
			usleep(1);
			//i=kbhit();
			++count;
			if(kbhit()) {
				char mystring[100];

				fgets(mystring, 100, stdin);

				std::cout << "typed string '" << mystring << "'";

				int ret = luaL_dostring(L, mystring); //argv[1]);
				std::cout << " return: " << ret << std::endl;
			}

		}
		std::cout << "count is " << count << std::endl;


		lua_close(L);
	}
	else {
		fprintf(stderr, "Usage: %s [lua-code]\n", argv[0]);
	}
}

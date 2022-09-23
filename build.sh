#!/bin/bash

GAME_FILES=src/game/*.c

gcc ${GAME_FILES} -c -L"/lib/" -lSDL2 -L"lib/" -lecs -I"include/" -I"src/engine/" -I"/usr/local/include/" -o lib/debuggame.o -DDEBUG

gcc ${GAME_FILES} -c -L"/lib/" -lSDL2 -L"lib/" -lecs -I"include/" -I"src/engine/" -I"/usr/local/include/" -o lib/releasegame.o -DNDEBUG

if [ $? -eq 0 ]; then
	ar rs lib/libreleasegame.a lib/releasegame.o
	ar rs lib/libdebuggame.a lib/debuggame.o

	if [ $? -eq 0 ]; then
		ENGINE_FILES=src/engine/*.c
		rm lib/releasegame.o
		rm lib/debuggame.o
		gcc ${ENGINE_FILES} -L"/lib/" -lSDL2 -L"lib/" -ldebuggame -lecs -I "include/" -I"/usr/local/include/" -o bin/debug/engine-deb -DDEBUG
	
		if [ $? -eq 0 ]; then
			gcc ${ENGINE_FILES} -L"/lib/" -lSDL2 -L"lib/" -lreleasegame -lecs -I "include/" -I"/usr/local/include/" -o bin/release/engine-rel -DNDEBUG
		fi
	fi
fi

#!/bin/bash

GAME_FILES=src/game/*.c
COMPILER='gcc'

${COMPILER} ${GAME_FILES} -c -L"/lib/" -lSDL2 -L"lib/" -lecs -I"include/" -I"src/engine/" -I"/usr/local/include/" -o lib/debuggame.o -DDEBUG

if [ $? -ne 0 ]; then
	exit 0
fi

${COMPILER} ${GAME_FILES} -c -L"/lib/" -lSDL2 -L"lib/" -lecs -I"include/" -I"src/engine/" -I"/usr/local/include/" -o lib/releasegame.o -DNDEBUG

if [ $? -ne 0 ]; then
	exit 0
fi

ar rs lib/libreleasegame.a lib/releasegame.o
ar rs lib/libdebuggame.a lib/debuggame.o

if [ $? -ne 0 ]; then
	exit 0
fi

ENGINE_FILES=src/engine/*.c
rm lib/releasegame.o lib/debuggame.o

${COMPILER} ${ENGINE_FILES} -L"/lib/" -lSDL2 -L"lib/" -ldebuggame -lecs -I "include/" -I"/usr/local/include/" -o bin/debug/engine-deb -DDEBUG

if [ $? -ne 0 ]; then
	exit 0
fi

${COMPILER} ${ENGINE_FILES} -L"/lib/" -lSDL2 -L"lib/" -lreleasegame -lecs -I "include/" -I"/usr/local/include/" -o bin/release/engine-rel -DNDEBUG

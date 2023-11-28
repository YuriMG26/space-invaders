CLANG_INCLUDE_FLAGS = -Ic:/handmade/include/
CLANG_WINDOWS_LIB_PATH = -Lc:/handmade/lib/
CLANG_WINDOWS_LINK_FLAGS = -lraylib -lopengl32 -lshell32 -lmsvcrt -luser32 -lgdi32 -lwinmm

all:
	clang -MT -fsanitize=address -Wall --debug main.c Game.c Logger.c -o ../build/space.exe $(CLANG_INCLUDE_FLAGS) $(CLANG_WINDOWS_LIB_PATH) $(CLANG_WINDOWS_LINK_FLAGS)
	

o:
	clang -Wall -O2 --debug --shared Game.c Logger.c -o ../build/game.dll $(CLANG_INCLUDE_FLAGS) $(CLANG_WINDOWS_LIB_PATH) $(CLANG_WINDOWS_LINK_FLAGS)
	clang -Wall -O2 --debug main.c Logger.c -o ../build/space.exe $(CLANG_INCLUDE_FLAGS) $(CLANG_WINDOWS_LIB_PATH) $(CLANG_WINDOWS_LINK_FLAGS)

m:
	clang -Wall --debug audio_test.c -o ../build/music.exe $(CLANG_INCLUDE_FLAGS) $(CLANG_WINDOWS_LIB_PATH) $(CLANG_WINDOWS_LINK_FLAGS)

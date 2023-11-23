INCLUDE_FLAGS = -Ic:/handmade/include/
WINDOWS_LIB_PATH = -Lc:/handmade/lib/
WINDOWS_LINK_FLAGS = -lraylib -lopengl32 -lshell32 -lmsvcrt -luser32 -lgdi32 -lwinmm

all:
	clang -Wall --debug *.c -o ../build/space.exe $(INCLUDE_FLAGS) $(WINDOWS_LIB_PATH) $(WINDOWS_LINK_FLAGS)

.PHONY : all clean

CC := gcc.exe
CFLAGS := -march=pentium4 -Wall -O3
CPPFLAGS :=
LDFLAGS :=

OUT_EXE := a5.exe

all : $(OUT_EXE)
	$(OUT_EXE)

clean:
	DEL /S *.o
	DEL /S *.exe

OBJECTS := \
	src/main.o


$(OUT_EXE) : $(OBJECTS)
	$(CC) -o $@  $(CFLAGS) $(LDFLAGS) $^

%.o : %.c
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

src/main.o : src/main.c

CC = gcc
CFLAGS = -Wall -O3
DEPS = stb_image.h stb_image_write.h
LIBS = -lm
OBJ = main.c
OUT = NegativeQuickConverterC

$(OUT):
	$(CC) -o $@ $(OBJ) $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o $(OUT)
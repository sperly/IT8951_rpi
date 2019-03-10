SRCS=IT8951.c IT8951_HW.c gfx.c bmp.c main.c AsciiLib.c
CC=gcc
TARGET=IT8951

$(TARGET):$(SRCS)
	$(CC) -Wall $(SRCS) -o $(TARGET) -lbcm2835
	
clean:
	rm -f $(TARGET)

#ifndef _BMP_H_
#define _BMP_H_

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

uint8_t BMP_DrawImage(uint32_t x, uint32_t y,char *path);

#endif //_BMP_H_
#ifndef LCD_H
#define LCD_H

#define POINTSX 320.
#define POINTSY 242.
#define RMAX  2.
#define RMIN -3.
#define IMAX  2.5
#define IMIN -2.5

int  init_lcd ();
void close_LCD(int fbfd);
void print_pix(int x, int y, int r, int g, int b);

#endif /* LCD_H */

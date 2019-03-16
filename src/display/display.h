#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <TFT_eSPI.h>
#include <display/font.h>

class Display
{
  public:
    Display(TFT_eSPI *tft);

    void setFont(const FONT_INFO *font);

    void drawChar(int32_t x, int32_t y, unsigned char c, uint32_t color, uint32_t bg);

    void print(const char *str);

    void println(const char *str = nullptr);    

  private:
    TFT_eSPI *_tft;
    const FONT_INFO *_font;
};

#endif
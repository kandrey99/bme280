#include <display/display.h>

Display::Display(TFT_eSPI *tft)
{
  _tft = tft;
}

void Display::setFont(const FONT_INFO *font)
{
  _font = font;
}

void Display::drawChar(int32_t x, int32_t y, unsigned char c, uint32_t color, uint32_t bg)
{
	const FONT_CHAR_INFO charInfo = _font->charInfo[c - _font->startChar];

	uint8_t w = charInfo.widthBits;
	uint8_t h = _font->heightBits;

	uint16_t offset = charInfo.offset;
	uint8_t line = 0;

	for (uint8_t i = 0; i < h; i++)
	{
		for (uint8_t j = 0; j < w; j++)
		{
			if (j % 8 == 0)
				line = _font->data[offset++];
			if (line & 0x80)
				_tft->drawPixel(x + j, y + i, color);
			else
				_tft->drawPixel(x + j, y + i, bg);
			line <<= 1;
		}
	} 
}

void Display::print(const char *str)
{
	for (uint8_t i = 0; i < strlen(str); i++)
	{
		unsigned char sym = str[i];
		int symWidth = _font->charInfo[sym - _font->startChar].widthBits;
		if (_tft->cursor_x + symWidth >= _tft->width())
		{
			_tft->cursor_x = 0;
			_tft->cursor_y += _font->heightBits + 1;
		}
		drawChar(_tft->cursor_x, _tft->cursor_y, sym, _tft->textcolor, _tft->textbgcolor);
		_tft->cursor_x += symWidth + _font->spacePixels;
	}   
}

void Display::println(const char *str)
{
	if (str != nullptr) print(str);	
	_tft->cursor_x = 0;
	_tft->cursor_y += _font->heightBits + 1;
}
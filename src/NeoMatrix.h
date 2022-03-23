#ifndef _NEO_MATRIX_H
#define _NEO_MATRIX_H

#include "Arduino.h"
#include "FastLED.h"
// Класс для работы с адресной матрицой с библиотекой FastLED с удобными функциями от AlexGyver.
template <uint16_t width, uint16_t height>
class Matrix
{
public:
	Matrix(CFastLED &userFastLed, CRGB *userLeds)
	{
		fastled = &userFastLed;
		leds = userLeds;
	}

	// получить номер пикселя в ленте по координатам
	uint16_t getPixelNumber(int8_t x, int8_t y)
	{
		if ((y & 1) == 0) // y & 1 == y % 2
			return y * width + x;
		// если чётная строка
		else
			return y * width + width - x - 1;
		// если нечётная строка
	}

	void setPixel(int8_t x, int8_t y, CRGB col)
	{
		leds[this->getPixelNumber(x, y)] = col;
	}
	/*
	def hex_to_rgb(colorcode):
    r = (colorcode >> 16) & 0xFF
    g = (colorcode >> 8) & 0xFF
    b = (colorcode >> 0) & 0xFF
    return r, g, b

	*/
	uint32_t getPixelColor(int8_t x, int8_t y)
	{
		CRGB col = leds[this->getPixelNumber(x, y)];
		return (uint32_t)(col.r << 16) | (col.g << 8) | col.b; 
	}

	void fillAll(CRGB col)
	{
		for (uint16_t i = 0; i < width * height; i++)
			leds[i] = col;
//		this->fastled->show();
	}

private:
	CFastLED *fastled;
	CRGB *leds;
};

#endif
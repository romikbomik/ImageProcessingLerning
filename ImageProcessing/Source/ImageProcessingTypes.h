#pragma once
#include <algorithm>

struct frame
{
	uint8_t* pixels = nullptr;
	int nFrameWidth;
	int nFrameHeight;


	frame()
	{
		frame(320, 240);
	}

	frame(int Width, int Height)
	{
		resize(Width, Height);
	}

	frame(const frame& other) {
		if (pixels)
		{
			delete[] pixels;
		}
		nFrameWidth = other.nFrameWidth;
		nFrameHeight = other.nFrameHeight;

		pixels = new uint8_t[nFrameWidth * nFrameHeight];
		std::copy(other.pixels, other.pixels + nFrameWidth * nFrameHeight, pixels);
	}

	~frame()
	{
		if (pixels)
		{
			delete[] pixels;
		}
	}


	uint8_t get(int x, int y) const
	{
		if (x >= 0 && x < nFrameWidth && y >= 0 && y < nFrameHeight)
		{
			return pixels[y * nFrameWidth + x];
		}
		else
			return 0;
	}

	void set(int x, int y, float p)
	{
		if (x >= 0 && x < nFrameWidth && y >= 0 && y < nFrameHeight)
		{
			pixels[y * nFrameWidth + x] = p;
		}
	}


	void operator=(const frame& other)
	{
		if (nFrameWidth != other.nFrameWidth
			|| nFrameHeight != other.nFrameHeight)
		{
			nFrameWidth = other.nFrameWidth;
			nFrameHeight = other.nFrameHeight;
			if (pixels)
			{
				delete[] pixels;
			}
			pixels = new uint8_t[nFrameWidth * nFrameHeight];
		}

		std::copy(other.pixels, other.pixels + nFrameWidth * nFrameHeight, pixels);
	}

	void resize(int Width, int Height)
	{
		if (pixels)
		{
			delete[] pixels;
		}
		nFrameWidth = Width;
		nFrameHeight = Height;
		pixels = new uint8_t[nFrameWidth * nFrameHeight];
		std::memset(pixels, 0, nFrameWidth * nFrameHeight * sizeof(uint8_t));
	}
};
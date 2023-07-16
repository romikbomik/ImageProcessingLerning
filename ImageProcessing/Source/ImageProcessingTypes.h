#pragma once
#include <algorithm>

struct frame
{
	float* pixels = nullptr;
	int nFrameWidth;
	int nFrameHeight;


	frame()
	{
		frame(320, 240);
	}

	frame(int Width, int Height)
	{
		nFrameWidth = Width;
		nFrameHeight = Height;
		pixels = new float[nFrameWidth * nFrameHeight];
		std::memset(pixels, 0, nFrameWidth * nFrameHeight * sizeof(float));
	}

	frame(const frame& other) {
		if (pixels)
		{
			delete[] pixels;
		}
		nFrameWidth = other.nFrameWidth;
		nFrameHeight = other.nFrameHeight;

		pixels = new float[nFrameWidth * nFrameHeight];
		std::copy(other.pixels, other.pixels + nFrameWidth * nFrameHeight, pixels);
	}

	~frame()
	{
		if (pixels)
		{
			delete[] pixels;
		}
	}


	float get(int x, int y) const
	{
		if (x >= 0 && x < nFrameWidth && y >= 0 && y < nFrameHeight)
		{
			return pixels[y * nFrameWidth + x];
		}
		else
			return 0.0f;
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
			pixels = new float[nFrameWidth * nFrameHeight];
		}

		std::copy(other.pixels, other.pixels + nFrameWidth * nFrameHeight, pixels);
	}
};
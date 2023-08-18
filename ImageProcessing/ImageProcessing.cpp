/*
	8-Bits Of Image Processing You Should Know
	"Colin, at least you'll always get 700s now..." - javidx9
	License (OLC-3)
	~~~~~~~~~~~~~~~
	Copyright 2018-2019 OneLoneCoder.com
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	Instructions:
	~~~~~~~~~~~~~
	Choose algorithm 1-8, instructions on screen
	Relevant Video: https://youtu.be/mRM5Js3VLCk
	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com
	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019
*/


#define OLC_PGE_APPLICATION
#define _USE_MATH_DEFINES
#include "olcPixelGameEngine.h"
#include "escapi.h"
#include "ImageProcessingTypes.h"
#include <ImageHelper.h>
#include <ImageHelperOpenCV.h>
#include <ImageProcessingInterface.h>
#include <memory>


int nFrameWidth = 320;
int nFrameHeight = 240;


class WIP_ImageProcessing : public olc::PixelGameEngine
{
public:
	WIP_ImageProcessing()
	{
		sAppName = "WIP_ImageProcessing";
	}

	union RGBint
	{
		int rgb;
		unsigned char c[4];
	};

	int nCameras = 0;
	SimpleCapParams capture;

	enum ALGORITHM
	{
		THRESHOLD, MOTION, LOWPASS, CONVOLUTION,
		SOBEL, MORPHO, MEDIAN, ADAPTIVE, OTSU,
		ROTATE
	};


	frame input = frame(nFrameWidth, nFrameHeight);
	frame output = frame(nFrameWidth, nFrameHeight);
	frame prev_input = frame(nFrameWidth, nFrameHeight);
	std::string ImplementationLibraryName = "Open CV implementation";
	std::unique_ptr<ImageProcessingInterface> ImageProcessor = std::make_unique<ImageHelperOpenCV>();

public:
	bool OnUserCreate() override
	{
		// Initialise webcam to screen dimensions
		nCameras = setupESCAPI();
		if (nCameras == 0)	return false;
		capture.mWidth = nFrameWidth;
		capture.mHeight = nFrameHeight;
		capture.mTargetBuf = new int[nFrameWidth * nFrameHeight];
		if (initCapture(0, &capture) == 0)	return false;
		return true;
	}

	void DrawFrame(frame& f, int x, int y)
	{
		for (int i = 0; i < nFrameWidth; i++)
			for (int j = 0; j < nFrameHeight; j++)
			{
				int c = (int)std::min(std::max(0, static_cast<int>(f.pixels[j * nFrameWidth + i])), 255);
				Draw(x + i, y + j, olc::Pixel(c, c, c));
			}
	}



	// Algorithm Currently Running
	ALGORITHM algo = THRESHOLD;
	IImgaeProcessor::MORPHOP morph = IImgaeProcessor::MORPHOP::DILATION;
	IImgaeProcessor::INTERPOLATION interpol = IImgaeProcessor::INTERPOLATION::NEAREST_NEIGHBOR;
	int nMorphCount = 1;

	uint8_t fThresholdValue = 127;
	float fLowPassRC = 0.1f;
	float fAdaptiveBias = 1.1f;
	float fAngle = 0.0f;

	std::vector<float> pConvoKernel = IImgaeProcessor::kernel_blur;



	bool OnUserUpdate(float fElapsedTime) override
	{
		// CAPTURING WEBCAM IMAGE
		prev_input = input;
		doCapture(0); while (isCaptureDone(0) == 0) {}
		for (int y = 0; y < capture.mHeight; y++)
			for (int x = 0; x < capture.mWidth; x++)
			{
				RGBint col;
				int id = y * capture.mWidth + x;
				col.rgb = capture.mTargetBuf[id];
				input.pixels[y * nFrameWidth + x] = (col.c[0] +col.c[1] + col.c[2]) / 3;
			}

		if (GetKey(olc::Key::K1).bReleased) algo = THRESHOLD;
		if (GetKey(olc::Key::K2).bReleased) algo = MOTION;
		if (GetKey(olc::Key::K3).bReleased) algo = LOWPASS;
		if (GetKey(olc::Key::K4).bReleased) algo = CONVOLUTION;
		if (GetKey(olc::Key::K5).bReleased) algo = SOBEL;
		if (GetKey(olc::Key::K6).bReleased) algo = MORPHO;
		if (GetKey(olc::Key::K7).bReleased) algo = MEDIAN;
		if (GetKey(olc::Key::K8).bReleased) algo = ADAPTIVE;
		if (GetKey(olc::Key::K9).bReleased) algo = OTSU;
		if (GetKey(olc::Key::Q).bReleased) algo = ROTATE;
		if (GetKey(olc::Key::P).bReleased) {
			if (dynamic_cast<ImageHelperOpenCV*>(ImageProcessor.get()) == nullptr)
			{
				ImageProcessor = std::make_unique<ImageHelperOpenCV>();
				ImplementationLibraryName = "Open CV implementation";
			}
			else if (dynamic_cast<ImageHelper*>(ImageProcessor.get()) == nullptr)
			{
				ImageProcessor = std::make_unique<ImageHelper>();
				ImplementationLibraryName = "Custom library implementation";
			}
			output = input;
		}


		switch (algo)
		{
		case THRESHOLD:

			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) fThresholdValue -= 1;
			if (GetKey(olc::Key::X).bHeld) fThresholdValue += 1;
			if (fThresholdValue > 255) fThresholdValue = 255;
			if (fThresholdValue < 0) fThresholdValue = 0.;

			ImageProcessor->ThresholdMethod(input, output, fThresholdValue);
			break;

		case MOTION:
			// Returns the absolute difference between successive frames per pixel
			ImageProcessor->MotionDetection(input, output);
			break;


		case LOWPASS:

			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) fLowPassRC -= 0.1f * fElapsedTime;
			if (GetKey(olc::Key::X).bHeld) fLowPassRC += 0.1f * fElapsedTime;
			if (fLowPassRC > 1.0f) fLowPassRC = 1.0f;
			if (fLowPassRC < 0.0f) fLowPassRC = 0.0f;

			ImageProcessor->LowpassFilter(input, output, fLowPassRC);
			break;

		case CONVOLUTION:
			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) pConvoKernel = IImgaeProcessor::kernel_blur;
			if (GetKey(olc::Key::X).bHeld) pConvoKernel = IImgaeProcessor::kernel_sharpen;

			ImageProcessor->ApplyKernel(input, output, pConvoKernel);
			break;

		case SOBEL:
			ImageProcessor->Soblel(input, output);
			break;

		case MORPHO:

			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) morph = IImgaeProcessor::MORPHOP::DILATION;
			if (GetKey(olc::Key::X).bHeld) morph = IImgaeProcessor::MORPHOP::EROSION;
			if (GetKey(olc::Key::C).bHeld) morph = IImgaeProcessor::MORPHOP::EDGE;

			if (GetKey(olc::Key::A).bReleased) nMorphCount--;
			if (GetKey(olc::Key::S).bReleased) nMorphCount++;
			if (nMorphCount > 10.0f) nMorphCount = 10.0f;
			if (nMorphCount < 1.0f) nMorphCount = 1.0f;

			ImageProcessor->MorphOperation(input, output, morph, nMorphCount);
			break;

		case MEDIAN:
			ImageProcessor->MedianFilter(input, output);
			break;

		case ADAPTIVE:
			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) fAdaptiveBias -= 0.5f;
			if (GetKey(olc::Key::X).bHeld) fAdaptiveBias += 0.5f;
			if (fAdaptiveBias > 20.f) fAdaptiveBias = 20.f;
			if (fAdaptiveBias < -20.f) fAdaptiveBias = -20.f;

			ImageProcessor->Adaptive(input, output, fAdaptiveBias);
			break;
		case OTSU: {
			ImageProcessor->Otsu(input, output, fThresholdValue);
			break;
		}
		case ROTATE:
			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) interpol = IImgaeProcessor::INTERPOLATION::NEAREST_NEIGHBOR;
			if (GetKey(olc::Key::X).bHeld) interpol = IImgaeProcessor::INTERPOLATION::BILINEAR;

			if (GetKey(olc::Key::A).bHeld) fAngle -= 1.0f;
			if (GetKey(olc::Key::S).bHeld) fAngle += 1.0f;

			ImageProcessor->Rotation(input, output, interpol, fAngle);
			break;
		}

		// DRAW STUFF ONLY HERE
		Clear(olc::DARK_BLUE);
		DrawFrame(input, 10, 10);
		DrawFrame(output, 340, 10);

		DrawString(150, 255, "INPUT");
		DrawString(480, 255, "OUTPUT");

		DrawString(10, 275, "1) Threshold");
		DrawString(10, 285, "2) Absolute Motion");
		DrawString(10, 295, "3) Low-Pass Temporal Filtering");
		DrawString(10, 305, "4) Convolution (Blurring/Sharpening)");
		DrawString(10, 315, "5) Sobel Edge Detection");
		DrawString(10, 325, "6) Binary Morphological Operations (Erosion/Dilation)");
		DrawString(10, 335, "7) Median Filter");
		DrawString(10, 345, "8) Adaptive Threshold");
		DrawString(10, 355, "9) OTSU Threshold");
		DrawString(10, 365, "Q) Rotate");
		auto startPos = 395;

		switch (algo)
		{
		case THRESHOLD:
			DrawString(10, startPos, "Change threshold value with Z and X keys");
			DrawString(10, startPos + 10, "Current value = " + std::to_string(fThresholdValue));
			break;

		case LOWPASS:
			DrawString(10, startPos, "Change RC constant value with Z and X keys");
			DrawString(10, startPos +10, "Current value = " + std::to_string(fLowPassRC));
			break;

		case CONVOLUTION:
			DrawString(10, startPos, "Change convolution kernel with Z and X keys");
			DrawString(10, startPos + 10, "Current kernel = " + std::string((pConvoKernel == IImgaeProcessor::kernel_blur) ? "Blur" : "Sharpen"));
			break;

		case MORPHO:
			DrawString(10, startPos, "Change operation with Z and X and C keys");
			if (morph == IImgaeProcessor::MORPHOP::DILATION) DrawString(10, startPos + 10, "Current operation = DILATION");
			if (morph == IImgaeProcessor::MORPHOP::EROSION) DrawString(10, startPos + 10, "Current operation = EROSION");
			if (morph == IImgaeProcessor::MORPHOP::EDGE) DrawString(10, startPos + 10, "Current operation = EDGE");
			DrawString(10, startPos + 20, "Change Iterations with A and S keys");
			DrawString(10, startPos + 30, "Current iteration count = " + std::to_string(nMorphCount));


			break;

		case ADAPTIVE:
			DrawString(10, startPos, "Change adaptive threshold bias with Z and X keys");
			DrawString(10, startPos + 10, "Current value = " + std::to_string(fAdaptiveBias));
			break;

		case OTSU:
			DrawString(10, startPos, "Threshold value = " + std::to_string(fThresholdValue));
			break;

		case ROTATE:
			DrawString(10, startPos, "Change operation with Z and X keys");
			if (interpol == IImgaeProcessor::INTERPOLATION::NEAREST_NEIGHBOR) DrawString(10, startPos + 10, "Current operation = NEAREST_NEIGHBOR");
			if (interpol == IImgaeProcessor::INTERPOLATION::BILINEAR) DrawString(10, startPos + 10, "Current operation = BILINEAR");
			DrawString(10, startPos + 20, "Change radian angle with A and S keys");
			DrawString(10, startPos + 30, "Current value = " + std::to_string(fAngle));
			break;
		default:
			break;
		}
		DrawString(10, startPos + 50, ImplementationLibraryName);
		if (GetKey(olc::Key::ESCAPE).bPressed) return false;
		return true;
	}

	void thresholdBinarization(const frame& input, frame& output, const float fThresholdValue) {
		for (int i = 0; i < nFrameWidth; i++)
			for (int j = 0; j < nFrameHeight; j++)
				output.set(i, j, input.get(i, j) >= fThresholdValue ? 1.0f : 0.0f);
	}
};


int main()
{
	WIP_ImageProcessing demo;
	if (demo.Construct(670, 460, 2, 2)){

		auto a = demo.Start();
		std::cout << a;
	}
	return 0;
}

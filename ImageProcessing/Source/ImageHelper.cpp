#include "ImageHelper.h"
#define _USE_MATH_DEFINES
#include <math.h>

void ImageHelper::ThresholdMethod(const frame& Input, frame& Output, const float ThresholdValue)
{
	thresholdBinarization(Input, Output, ThresholdValue);
	// Perform threshold per pixel
	for (int i = 0; i < Output.nFrameWidth; i++)
		for (int j = 0; j < Output.nFrameHeight; j++)
			Output.set(i, j, Input.get(i, j) >= ThresholdValue ? 1.0f : 0.0f);
}

void ImageHelper::MotionDetection(const frame& Input, frame& Output)
{
	for (int i = 0; i < Output.nFrameWidth; i++)
		for (int j = 0; j < Output.nFrameWidth; j++)
			Output.set(i, j, fabs(Input.get(i, j) - CachedInput.get(i, j)));

	CachedInput = Input;
}

void ImageHelper::LowpassFilter(const frame& Input, frame& Output, const float LowPassRC)
{
	// Pass each pixel through a temporal RC filter
	for (int i = 0; i < Output.nFrameWidth; i++)
		for (int j = 0; j < Output.nFrameHeight; j++)
		{
			float dPixel = Input.get(i, j) - Output.get(i, j);
			dPixel *= LowPassRC;
			Output.set(i, j, dPixel + Output.get(i, j));
		}
	
}

void ImageHelper::ApplyKernel(const frame& Input, frame& Output, const std::vector<float> Kernel)
{
	if (Kernel.size() < 3 * 3)
	{
		printf("ImageHelper: Invalid Kernel size. ApplyKernel supports 3*3 Kernel only");
		Output = Input;
		return;
	}

	for (int i = 0; i < Output.nFrameWidth; i++)
		for (int j = 0; j < Output.nFrameHeight; j++)
		{
			float fSum = 0.0f;
			for (int n = -1; n < +2; n++)
				for (int m = -1; m < +2; m++)
						fSum += Input.get(i + n, j + m) * Kernel.at((m + 1) * 3 + (n + 1));

			Output.set(i, j, fSum);
		}
}

void ImageHelper::Soblel(const frame& Input, frame& Output)
{
	for (int i = 0; i < Output.nFrameWidth; i++)
		for (int j = 0; j < Output.nFrameHeight; j++)
		{
			float fKernelSumH = 0.0f;
			float fKernelSumV = 0.0f;

			for (int n = -1; n < +2; n++)
				for (int m = -1; m < +2; m++)
				{
					fKernelSumH += Input.get(i + n, j + m) * IImgaeProcessor::kernel_sobel_h[(m + 1) * 3 + (n + 1)];
					fKernelSumV += Input.get(i + n, j + m) * IImgaeProcessor::kernel_sobel_v[(m + 1) * 3 + (n + 1)];
				}

			Output.set(i, j, fabs((fKernelSumH + fKernelSumV) / 2.0f));
		}
}

void ImageHelper::MorphOperation(const frame& Input, frame& Output, IImgaeProcessor::MORPHOP operation, int MorphCount)
{
	frame Activity = frame(Output.nFrameWidth, Output.nFrameHeight);

	// Threshold First to binarise image
	thresholdBinarization(Input, Activity, 0.5f);
	switch (operation)
	{
	case IImgaeProcessor::MORPHOP::DILATION:
		for (int n = 0; n < MorphCount; n++)
		{
			Output = Activity;

			for (int i = 0; i < Output.nFrameWidth; i++)
				for (int j = 0; j < Output.nFrameHeight; j++)
				{
					if (Activity.get(i, j) == 1.0f)
					{
						Output.set(i, j, 1.0f);
						Output.set(i - 1, j, 1.0f);
						Output.set(i + 1, j, 1.0f);
						Output.set(i, j - 1, 1.0f);
						Output.set(i, j + 1, 1.0f);
						Output.set(i - 1, j - 1, 1.0f);
						Output.set(i + 1, j + 1, 1.0f);
						Output.set(i + 1, j - 1, 1.0f);
						Output.set(i - 1, j + 1, 1.0f);
					}
				}

			//Activity = Output;
		}
		break;

	case IImgaeProcessor::MORPHOP::EROSION:
		for (int n = 0; n < MorphCount; n++)
		{
			Output = Activity;
			for (int i = 0; i < Output.nFrameWidth; i++)
				for (int j = 0; j < Output.nFrameHeight; j++)
				{

					float sum = Activity.get(i - 1, j) + Activity.get(i + 1, j) + Activity.get(i, j - 1) + Activity.get(i, j + 1) +
						Activity.get(i - 1, j - 1) + Activity.get(i + 1, j + 1) + Activity.get(i + 1, j - 1) + Activity.get(i - 1, j + 1);

					if (Activity.get(i, j) == 1.0f && sum < 8.0f)
					{
						Output.set(i, j, 0.0f);
					}
				}
			//activity = output;
		}
		break;

	case IImgaeProcessor::MORPHOP::EDGE:
		Output = Activity;
		for (int i = 0; i < Output.nFrameWidth; i++)
			for (int j = 0; j < Output.nFrameHeight; j++)
			{

				float sum = Activity.get(i - 1, j) + Activity.get(i + 1, j) + Activity.get(i, j - 1) + Activity.get(i, j + 1) +
					Activity.get(i - 1, j - 1) + Activity.get(i + 1, j + 1) + Activity.get(i + 1, j - 1) + Activity.get(i - 1, j + 1);

				if (Activity.get(i, j) == 1.0f && sum == 8.0f)
				{
					Output.set(i, j, 0.0f);
				}
			}
		break;

	}
}

void ImageHelper::MedianFilter(const frame& Input, frame& Output)
{
	for (int i = 0; i < Output.nFrameWidth; i++)
		for (int j = 0; j < Output.nFrameHeight; j++)
		{
			std::vector<float> v(25, 0);

			for (int n = -2; n < +3; n++)
				for (int m = -2; m < +3; m++)
					v.push_back(Input.get(i + n, j + m));

			std::sort(v.begin(), v.end(), std::greater<float>());
			Output.set(i, j, v[12]);
		}
}

void ImageHelper::Adaptive(const frame& Input, frame& Output, const float AdaptiveBias)
{
	for (int i = 0; i < Output.nFrameWidth; i++)
		for (int j = 0; j < Output.nFrameHeight; j++)
		{
			float fRegionSum = 0.0f;

			for (int n = -2; n < +3; n++)
				for (int m = -2; m < +3; m++)
					fRegionSum += Input.get(i + n, j + m);

			fRegionSum /= 25.0f;
			Output.set(i, j, Input.get(i, j) > (fRegionSum * AdaptiveBias) ? 1.0f : 0.0f);
		}
}

void ImageHelper::Otsu(const frame& Input, frame& Output, float& ThresholdValue)
{
	std::vector<int> histogram(256, 0);
	float threshold_otsu = 0;
	int total = Output.nFrameWidth * Output.nFrameHeight;
	int wb = 0, wf = 0, sumb = 0, sum = 0;
	double  otsu_max = 0, mb, mf, between;
	for (int i = 0; i < Output.nFrameWidth; i++)
		for (int j = 0; j < Output.nFrameHeight; j++)
		{
			auto pos = (Input.get(i, j) * 255);
			sum += pos;
			histogram.at(pos)++;
		}

	for (int i = 0; i < 255; i++) {
		wb += histogram.at(i);
		if (wb == 0) { continue; }
		wf = total - wb;
		if (wf == 0) { continue; }
		sumb += i * histogram.at(i);
		mb = sumb / wb;
		mf = (sum - sumb) / wf;
		between = wb * wf * ((mb - mf) * (mb - mf));
		if (between > otsu_max) {
			otsu_max = between;
			threshold_otsu = i;
		}
	}
	ThresholdValue = threshold_otsu / 255;
	// Perform threshold per pixel
	thresholdBinarization(Input, Output, ThresholdValue);
}

void ImageHelper::Rotation(const frame& Input, frame& Output, IImgaeProcessor::INTERPOLATION Interpol, const float Angle)
{
	double centerX = (Output.nFrameWidth - 1) / 2.0;
	double centerY = (Output.nFrameHeight - 1) / 2.0;
	double dx, dy;
	double newX, newY;
	double cosV = cos(-(Angle * (M_PI / 180)));
	double sinV = sin(-(Angle * (M_PI / 180)));
	double halfTanV = tan(-((Angle / 2) * (M_PI / 180)));
	for (int i = 0; i < Output.nFrameWidth; i++) {
		for (int j = 0; j < Output.nFrameHeight; j++) {
			dx = (double)i - centerX;
			dy = (double)j - centerY;


			newX = cosV * dx + sinV * dy + centerX;
			newY = cosV * dy - sinV * dx + centerY;

			Output.set(i, j, GetValueInterpolated(Interpol, Input, newX, newY));
		}
	}
}

float ImageHelper::GetValueInterpolated(IImgaeProcessor::INTERPOLATION Interpol, const frame& Input, const float  x, const float y)
{
	float result = Input.get(x, y);
	if (floor(x) == ceil(x) && floor(y) == ceil(y)) 
	{
		//coordiantes of existing pixel no need to interpolate
		return result;
	}
	//values for x1y1 x1y2 ... x2y2
	float q11, q12, q21, q22, dx, dy, intepolX, interpolY;
	switch (Interpol)
	{
	case IImgaeProcessor::INTERPOLATION::NEAREST_NEIGHBOR:
		result = Input.get((int)(x + 0.5), (int)(y + 0.5));
		break;
	case IImgaeProcessor::INTERPOLATION::BILINEAR:
		// get four values surounding the point
		q11 = Input.get(floor(x), floor(y));
		q12 = Input.get(ceil(x), floor(y));
		q21 = Input.get(floor(x), ceil(y));
		q22 = Input.get(ceil(x), ceil(y));
		dx = x - floor(x);
		dy = y - floor(y);
		intepolX = q11* (ceil(x) - x) + q12 * (x - floor(x));
		interpolY = q21 * (ceil(x) - x) + q22 * (x - floor(x));
		result = intepolX * (ceil(y) - y) + interpolY * (y - floor(y));
		break;
	}
	return result;
}

void ImageHelper::thresholdBinarization(const frame& input, frame& output, const float ThresholdValue)
{
	for (int i = 0; i < output.nFrameWidth; i++)
		for (int j = 0; j < output.nFrameHeight; j++)
			output.set(i, j, input.get(i, j) >= ThresholdValue ? 1.0f : 0.0f);
}

#undef _USE_MATH_DEFINES
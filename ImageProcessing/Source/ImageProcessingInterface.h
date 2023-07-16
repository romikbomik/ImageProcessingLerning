#pragma once
#include <vector>
namespace IImgaeProcessor
{
	enum class MORPHOP
	{
		DILATION,
		EROSION,
		EDGE
	};

	enum class INTERPOLATION
	{
		NEAREST_NEIGHBOR,
		BILINEAR
	};

	static std::vector<float> kernel_blur
	{
		0.0f,   0.125,  0.0f,
		0.125f, 0.5f,   0.125f,
		0.0f,   0.125f, 0.0f,
	};

	static std::vector<float> kernel_sharpen
	{
		0.0f,  -1.0f,  0.0f,
		-1.0f,  5.0f, -1.0f,
		0.0f,  -1.0f,  0.0f,
	};

	static std::vector<float> kernel_sobel_v
	{
		-1.0f, 0.0f, +1.0f,
		-2.0f, 0.0f, +2.0f,
		-1.0f, 0.0f, +1.0f,
	};

	static std::vector<float> kernel_sobel_h
	{
		-1.0f, -2.0f, -1.0f,
		 0.0f, 0.0f, 0.0f,
		+1.0f, +2.0f, +1.0f,
	};
}
struct frame;

class ImageProcessingInterface
{
public:
	virtual  void ThresholdMethod(const frame& Input, frame& Output, const float ThresholdValue) = 0;
	virtual void MotionDetection(const frame& Input, frame& Output) = 0;
	virtual void LowpassFilter(const frame& Input, frame& Output, const float LowPassRC) = 0;
	virtual void ApplyKernel(const frame& Input, frame& Output, const std::vector<float> Kernel) = 0;
	virtual void Soblel(const frame& Input, frame& Output) = 0;
	virtual void MorphOperation(const frame& Input, frame& Output, IImgaeProcessor::MORPHOP operation, int MorphCount) = 0;
	virtual void MedianFilter(const frame& Input, frame& Output) = 0;
	virtual void Adaptive(const frame& Input, frame& Output, const float AdaptiveBias) = 0;
	virtual void Otsu(const frame& Input, frame& Output, float& ThresholdValue) = 0;
	virtual void Rotation(const frame& Input, frame& Output, IImgaeProcessor::INTERPOLATION Interpol, const float Angle) = 0;
};

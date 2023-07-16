#pragma once
#include "ImageProcessingInterface.h"
#include "ImageProcessingTypes.h"


class ImageHelper : public ImageProcessingInterface
{
public:

	//start ImageProcessingInterface 
	virtual void ThresholdMethod(const frame& Input, frame& Output, const float ThresholdValue) override;
	virtual void MotionDetection(const frame& Input, frame& Output) override;
	virtual void LowpassFilter(const frame& Input, frame& Output, const float LowPassRC) override;
	virtual void ApplyKernel(const frame& Input, frame& Output, const std::vector<float> Kernel) override;
	virtual void Soblel(const frame& Input, frame& Output) override;
	virtual void MorphOperation(const frame& Input, frame& Output, IImgaeProcessor::MORPHOP operation, int MorphCount) override;
	virtual void MedianFilter(const frame& Input, frame& Output) override;
	virtual void Adaptive(const frame& Input, frame& Output, const float AdaptiveBias) override;
	virtual void Otsu(const frame& Input, frame& Output, float& ThresholdValue) override;
	virtual void Rotation(const frame& Input, frame& Output, IImgaeProcessor::INTERPOLATION Interpol, const float Angle) override;
	// end ImageProcessingInterface

private:
	frame CachedInput;
	float GetValueInterpolated(IImgaeProcessor::INTERPOLATION Interpol, const frame& Input, const float x, const float y);
	static void thresholdBinarization(const frame& input, frame& output, const float ThresholdValue);
};

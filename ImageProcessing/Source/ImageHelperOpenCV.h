
#pragma once
#include "ImageProcessingInterface.h"
#include "ImageProcessingTypes.h"
#include <opencv2/opencv.hpp>

//TODO
class ImageHelperOpenCV : public ImageProcessingInterface
{
public:
	//start ImageProcessingInterface 
	virtual void ThresholdMethod(const frame& Input, frame& Output, const uint8_t ThresholdValue) override;
	virtual void MotionDetection(const frame& Input, frame& Output) override;
	virtual void LowpassFilter(const frame& Input, frame& Output, const float LowPassRC) override;
	virtual void ApplyKernel(const frame& Input, frame& Output, const std::vector<float> Kernel) override;
	virtual void Soblel(const frame& Input, frame& Output) override;
	virtual void MorphOperation(const frame& Input, frame& Output, IImgaeProcessor::MORPHOP operation, int MorphCount) override;
	virtual void MedianFilter(const frame& Input, frame& Output) override;
	virtual void Adaptive(const frame& Input, frame& Output, const float AdaptiveBias) override;
	virtual void Otsu(const frame& Input, frame& Output, uint8_t& ThresholdValue) override;
	virtual void Rotation(const frame& Input, frame& Output, IImgaeProcessor::INTERPOLATION Interpol, const float Angle) override;
	// end ImageProcessingInterface

private:
	cv::Mat CachedInput;
	void FrameToMatrix(const frame& Input, cv::Mat& Output);
	void MatrixToFrame(const cv::Mat& Input, frame& Output);
};

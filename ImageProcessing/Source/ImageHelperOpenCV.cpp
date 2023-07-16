#include "ImageHelperOpenCV.h"

void ImageHelperOpenCV::ThresholdMethod(const frame& Input, frame& Output, const float ThresholdValue)
{
}

void ImageHelperOpenCV::MotionDetection(const frame& Input, frame& Output)
{
}

void ImageHelperOpenCV::LowpassFilter(const frame& Input, frame& Output, const float LowPassRC)
{
}

void ImageHelperOpenCV::ApplyKernel(const frame& Input, frame& Output, const std::vector<float> Kernel)
{
}

void ImageHelperOpenCV::Soblel(const frame& Input, frame& Output)
{
}

void ImageHelperOpenCV::MorphOperation(const frame& Input, frame& Output, IImgaeProcessor::MORPHOP operation, int MorphCount)
{
}

void ImageHelperOpenCV::MedianFilter(const frame& Input, frame& Output)
{
}

void ImageHelperOpenCV::Adaptive(const frame& Input, frame& Output, const float AdaptiveBias)
{
}

void ImageHelperOpenCV::Otsu(const frame& Input, frame& Output, float& ThresholdValue)
{
}

void ImageHelperOpenCV::Rotation(const frame& Input, frame& Output, IImgaeProcessor::INTERPOLATION Interpol, const float Angle)
{
}

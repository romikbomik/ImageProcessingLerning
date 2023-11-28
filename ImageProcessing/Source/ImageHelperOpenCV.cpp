#include "ImageHelperOpenCV.h"
#include <fstream>
#define DEFAULT_DDEPTH CV_8UC1

template <typename T>
void printMat(const cv::Mat& mat) {
	for (int i = 0; i < mat.rows; ++i) {
		for (int j = 0; j < mat.cols; ++j) {
			std::cout << mat.at<T>(i, j) << " ";
		}
		std::cout << std::endl;
	}
}

void savePixelValuesToFile(const cv::Mat& image, const std::string& filename) {
	std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc);

	if (!file.is_open()) {
		std::cerr << "Unable to open file: " << filename << std::endl;
		return;
	}

	for (int i = 0; i < image.rows; ++i) {
		for (int j = 0; j < image.cols; ++j) {
			file << image.at<uint8_t>(i, j);

			// Add a comma or space to separate values (use appropriate delimiter)
			if (j < image.cols - 1) {
				file << "\t"; // You can use other delimiters like '\t' or ' ' (space)
			}
		}
		file << std::endl;
	}

	file.close();
}


template <typename T>
void FrameToMatrixCasted(const frame& Input, cv::Mat& Output, int Depth = DEFAULT_DDEPTH)
{
	Output = cv::Mat(Input.nFrameWidth, Input.nFrameHeight, DEFAULT_DDEPTH);
	for (int i = 0; i < Input.nFrameWidth; i++)
		for (int j = 0; j < Input.nFrameHeight; j++)
			Output.at<T>(i, j) = static_cast<T>(Input.get(i, j));

}

void ImageHelperOpenCV::ThresholdMethod(const frame& Input, frame& Output, const uint8_t ThresholdValue)
{
	cv::Mat MatInput;
	cv::Mat MatOutput;
	FrameToMatrix(Input, MatInput);
	int thresholdType = cv::THRESH_BINARY;

	cv::threshold(MatInput, MatOutput, ThresholdValue, 255, thresholdType);

	MatrixToFrame(MatOutput, Output);

}

void ImageHelperOpenCV::MotionDetection(const frame& Input, frame& Output)
{
	cv::Mat MatInput;
	cv::Mat MatOutput;
	FrameToMatrix(Input, MatInput);
	if (MatInput.size() != CachedInput.size())
	{
		//cached size incompatible reset cache and skip 1 frame;
		CachedInput = MatInput;
		return;
	}
	cv::absdiff(MatInput, CachedInput, MatOutput);
	MatrixToFrame(MatOutput, Output);
	CachedInput = MatInput;
}

void ImageHelperOpenCV::LowpassFilter(const frame& Input, frame& Output, const float LowPassRC)
{
	cv::Mat MatInput;
	cv::Mat MatOutput;
	cv::Mat MatTemp;
	FrameToMatrix(Input, MatInput);
	FrameToMatrix(Output, MatOutput);

	cv::subtract(MatInput, MatOutput, MatTemp);
	MatTemp *= LowPassRC;
	cv::add(MatTemp, MatOutput, MatOutput);

	MatrixToFrame(MatOutput, Output);
}

void ImageHelperOpenCV::ApplyKernel(const frame& Input, frame& Output, const std::vector<float> Kernel)
{
	cv::Mat MatInput;
	cv::Mat MatOutput;
	FrameToMatrix(Input, MatInput);
	cv::Mat MatKernel = cv::Mat(Kernel).reshape(1, 3);
	cv::filter2D(MatInput, MatOutput, -1, MatKernel);
	MatrixToFrame(MatOutput, Output);
}

void ImageHelperOpenCV::Soblel(const frame& Input, frame& Output)
{
	cv::Mat MatInput;
	cv::Mat MatOutput;
	FrameToMatrix(Input, MatInput);
	cv::Sobel(MatInput, MatOutput, DEFAULT_DDEPTH, 1, 1, 3);
	MatrixToFrame(MatOutput, Output);
}

void ImageHelperOpenCV::MorphOperation(const frame& Input, frame& Output, IImgaeProcessor::MORPHOP operation, int MorphCount)
{
	const int thresholdType = cv::THRESH_BINARY;
	const int MorhpSize = 1;
	cv::Mat MatInput;
	cv::Mat MatOutput;
	cv::Mat MatMorpth;
	cv::Mat MatCopy;


	FrameToMatrix(Input, MatInput);
	cv::threshold(MatInput, MatOutput, 127, 255, thresholdType);

	switch (operation)
	{
	case IImgaeProcessor::MORPHOP::DILATION:
		MatMorpth = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * MorhpSize + 1, 2 * MorhpSize + 1));
		for (int n = 0; n < MorphCount; n++)
		{
			cv::dilate(MatOutput, MatOutput, MatMorpth);
		}
		break;
	case IImgaeProcessor::MORPHOP::EROSION:
		MatMorpth = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * MorhpSize + 1, 2 * MorhpSize + 1));
		for (int n = 0; n < MorphCount; n++)
		{
			cv::erode(MatOutput, MatOutput, MatMorpth);
		}
		break;
	case IImgaeProcessor::MORPHOP::EDGE:
	{
		// Define the custom filter
		MatMorpth = (cv::Mat_<int>(3, 3) <<
			1, 1, 1,
			1, 1, 1,
			1, 1, 1);
		// Apply the custom filter
		MatOutput.convertTo(MatCopy, CV_32FC1);
		cv::filter2D(MatOutput, MatCopy, CV_32FC1, MatMorpth);
		for (int i = 0; i < MatOutput.rows; ++i) {
			for (int j = 0; j < MatOutput.cols; ++j) {
				if (MatCopy.at<float>(i, j) == 9 * 255)
				{
					MatOutput.at<uint8_t>(i, j) = 0;
				}
			}
		}
		break;
	}
	default:
		break;
	}
	MatrixToFrame(MatOutput, Output);
}

void ImageHelperOpenCV::MedianFilter(const frame& Input, frame& Output)
{
	cv::Mat MatInput;
	cv::Mat MatOutput;
	FrameToMatrix(Input, MatInput);
	cv::medianBlur(MatInput, MatOutput, 5);
	MatrixToFrame(MatOutput, Output);
}

void ImageHelperOpenCV::Adaptive(const frame& Input, frame& Output, const float AdaptiveBias)
{
	try {
		cv::Mat MatInput;
		cv::Mat MatOutput;
		FrameToMatrix(Input, MatInput);
		cv::adaptiveThreshold(MatInput, MatOutput, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 5, AdaptiveBias);
		MatOutput.convertTo(MatOutput, DEFAULT_DDEPTH);
		MatrixToFrame(MatOutput, Output);
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

}

void ImageHelperOpenCV::Otsu(const frame& Input, frame& Output, uint8_t& ThresholdValue)
{
	cv::Mat MatInput;
	cv::Mat MatOutput;
	FrameToMatrix(Input, MatInput);
	ThresholdValue = cv::threshold(MatInput, MatOutput, 0, 255, cv::THRESH_OTSU);
	MatrixToFrame(MatOutput, Output);
}

void ImageHelperOpenCV::Rotation(const frame& Input, frame& Output, IImgaeProcessor::INTERPOLATION Interpol, const float Angle)
{
	cv::Mat MatInput;
	cv::Mat MatOutput;
	FrameToMatrix(Input, MatInput);
	cv::Point2f center(MatInput.cols / 2.0, MatInput.rows / 2.0);
	cv::Mat MatRot = cv::getRotationMatrix2D(center, Angle, 1.0);
	int InterpolationFlags = 0;
	switch (Interpol)
	{
	case IImgaeProcessor::INTERPOLATION::NEAREST_NEIGHBOR:
		InterpolationFlags |= cv::INTER_NEAREST;
		break;
	case IImgaeProcessor::INTERPOLATION::BILINEAR:
		InterpolationFlags |= cv::INTER_LINEAR;
		break;
	}


	cv::warpAffine(MatInput, MatOutput, MatRot, MatInput.size(), InterpolationFlags);
	MatrixToFrame(MatOutput, Output);
}


void ImageHelperOpenCV::FrameToMatrix(const frame& Input, cv::Mat& Output)
{
	Output = cv::Mat(Input.nFrameWidth, Input.nFrameHeight, DEFAULT_DDEPTH);
	for (int i = 0; i < Input.nFrameWidth; i++)
		for (int j = 0; j < Input.nFrameHeight; j++)
			Output.at<uint8_t>(i, j) = Input.get(i, j);

}

void ImageHelperOpenCV::MatrixToFrame(const cv::Mat& Input, frame& Output)
{
	Output.resize(Input.rows, Input.cols);
	for (int i = 0; i < Input.rows; i++)
		for (int j = 0; j < Input.cols; j++)
			Output.set(i, j, Input.at<uint8_t>(i, j));
}

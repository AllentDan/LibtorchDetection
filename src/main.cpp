#include"Detector.h"

int main()
{

	cv::Mat image = cv::imread("../dataset/val/images/2007_005331.jpg");
	Detector detector;
	detector.Initialize(0, 416, 416, "../dataset/voc_classes.txt");
	// detector.Train("dataset", ".jpg", 30,
	// 	4, 0.001, "weights/detector.pt", "weights/yolo4_tiny.pt");

	detector.LoadWeight("../weights/detector.pt");
	detector.Predict(image, true, 0.1);

	//speed test
	int64 start = cv::getTickCount();
	int loops = 10;
	for (int i = 0; i < loops; i++) {
		detector.Predict(image, false);
	}
	double duration = (cv::getTickCount() - start) / cv::getTickFrequency();
	std::cout << duration/ loops <<" s per prediction" << std::endl;

	return 0;
}
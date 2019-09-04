#include <iomanip>
#include <string>
#include "mtcnn.h"
#include "select_file.h"


using namespace std;

int main(int argc, char **argv)
{
	std::cout << "start!!!!!!!!!!!!!!!" << endl;
	string root = "D:/Caffe_Dev/Myfacedetection_mtcnn/Myfacedetection_mtcnn/img/";

	int select_num = 0;
	vector<string> name_list;
	char flag = 'n';
	do
	{
		cout << "choose" << endl;
		flag = 'n';
		string path = get_path();
		if (path != "") {
			name_list.push_back(path);
			std::cout << "continue?y:n->";
			std::cin >> flag;
		}
	} while (flag=='y'||flag=='Y');
/**
	string name_list[8] = {
		"0_Parade_marchingband_1_364.jpg",
		"0_Parade_marchingband_1_408.jpg",
		"img_591.jpg",
		"img_534.jpg",
		"img_561.jpg",
		"img_769.jpg",
		"tests.jpg"
	};**/
	//mtcnn detector("../../Myfacedetection_mtcnn/model_with_landmark",true);
	//mtcnn detector("../../Myfacedetection_mtcnn/MyModel_without_landmark",false);
	//mtcnn detector("D:/Caffe_Dev/Myfacedetection_mtcnn_save/Myfacedetection_mtcnn/model_with_landmark",true);
	mtcnn detector("D:/Caffe_Dev/Myfacedetection_mtcnn_save/Myfacedet ection_mtcnn/MyModel_without_lamdmark",false);
	float factor = 0.709f;
	float threshold[3] = { 0.7f, 0.6f, 0.6f };
	int minSize = 15;
	int stage = 3;

	std::cout << "load！！！！！！！！！！！！！！！！！！！！！！！！！！！！！" << endl;

	for (int n = 0; n < name_list.size(); ++n) {
		//读取图片，以BGR模式读取
		cv::Mat image = cv::imread(name_list[n], cv
			::IMREAD_COLOR);
		//changed BGR to RGB
		//cv::cvtColor(image,image, cv::COLOR_BGR2RGB);
		//cv::transpose(image, image);
		//计时
		double t = (double)cv::getTickCount();

		//调用检测
		vector<FaceCdt> faceInfo = detector.Detect(image, minSize, threshold, factor);
		//cv::transpose(image, image);

		//输出
		std::cout << name_list[n] << " time," << (double)(cv::getTickCount() - t) / cv::getTickFrequency() << "s" << std::endl;
		for (int i = 0; i < faceInfo.size(); i++) {
			int x = (int)faceInfo[i].bbox.xmin;
			int y = (int)faceInfo[i].bbox.ymin;
			int w = (int)(faceInfo[i].bbox.xmax - faceInfo[i].bbox.xmin + 1);
			int h = (int)(faceInfo[i].bbox.ymax - faceInfo[i].bbox.ymin + 1);
			cout << x <<" "<< y <<" "<< w <<" "<< h << endl;
			cv::rectangle(image, cv::Rect(x, y, w, h), cv::Scalar(255, 0, 0), 1);

			//cv::rectangle(image, cv::Rect(y, x, w, h), cv::Scalar(255, 0, 0), 1);

			cv::putText(image, to_string(faceInfo[i].bbox.score).substr(0,4), cv::Point(x, y)
				, cv::FONT_HERSHEY_COMPLEX,0.5, cv::Scalar(255, 0,0));
		}
		for (int i = 0; i < faceInfo.size(); i++) {
			float *landmark = faceInfo[i].landmark;
			for (int j = 0; j < 5; j++) {
				cv::circle(image, cv::Point((int)landmark[2 * j], (int)landmark[2 * j + 1]), 1, cv::Scalar(0, 255, 0), 2);
			}
		}
		//changed
		//cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		//cv::transpose(image, image);

		cv::imwrite(root + "_res_stage" + to_string(stage) + "_" +name_list[n].substr(name_list[n].find_last_of('/')+1), image);
		cv::imshow("image", image);
		cv::waitKey(0);
	}
	return 0;
}



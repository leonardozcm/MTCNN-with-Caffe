#pragma once
#include <caffe/caffe.hpp>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>

#include "FaceCdt.h"
#include "layerhead.h"
using namespace caffe;
using namespace std;

//pnet config
const int pnet_max_detect_num = 5000;

//mean & std
const float mean_val = 127.5f;
const float std_val = 0.0078125f;

//minibatch size
const int step_size = 128;




class mtcnn
{

public:
	mtcnn();
	~mtcnn();
	mtcnn(const string& model_dir, bool showlandmark);
	vector<FaceCdt> Detect(const cv::Mat& img, const int min_size, const float* threshold, const float factor);
	vector<FaceCdt> ProposalNet(const cv::Mat& img, float threshold, float factor);
	vector<FaceCdt> RefineNet(const cv::Mat& image, vector<FaceCdt> &pre_stage_res, int input_w, int input_h, const float threshold);
	vector<FaceCdt> OutputNet(const cv::Mat& image, vector<FaceCdt> &pre_stage_res, int input_w, int input_h, const float threshold);
	void BBoxRegression(vector<FaceCdt>& bboxes);
	void BBoxPadSquare(vector<FaceCdt>& bboxes, int width, int height);
	void BBoxPad(vector<FaceCdt>& bboxes, int width, int height);
	void GenerateBBox(Blob<float>* confidence, Blob<float>* reg_box, float scale, float thresh);
	std::vector<FaceCdt> NMS(std::vector<FaceCdt>& bboxes, float thresh, char methodType);
private:
	boost::shared_ptr<Net<float>> PNet_;
	boost::shared_ptr<Net<float>> RNet_;
	boost::shared_ptr<Net<float>> ONet_;

	std::vector<FaceCdt> candidate_boxes_;
	std::vector<FaceCdt> total_boxes_;

	bool showlandmark_ = true;
};


#include "mtcnn.h"
const int threads_num = 4;

mtcnn::mtcnn()
{
}


mtcnn::~mtcnn()
{
}

mtcnn::mtcnn(const string&  model_dir, bool showlandmark = true)
{
	Caffe::set_mode(Caffe::GPU);
	PNet_.reset(new Net<float>((model_dir + "/det1.prototxt"), TEST));
	PNet_->CopyTrainedLayersFrom(model_dir + "/det1.caffemodel");
	RNet_.reset(new Net<float>((model_dir + "/det2.prototxt"), TEST));
	RNet_->CopyTrainedLayersFrom(model_dir + "/det2.caffemodel");
	ONet_.reset(new Net<float>((model_dir + "/det3.prototxt"), TEST));
	ONet_->CopyTrainedLayersFrom(model_dir + "/det3.caffemodel");
	showlandmark_ = showlandmark;
	//读取input的图片
	Blob<float>* input_layer;
	input_layer = PNet_->input_blobs()[0];
	int num_channels_ = input_layer->channels();
	CHECK(num_channels_ == 3) << "Input layer should have 3 channels.";
}

vector<FaceCdt> mtcnn::Detect(const cv::Mat & img, const int min_size, const float * threshold, const float factor)
{
	vector<FaceCdt> pnet_res;
	vector<FaceCdt> rnet_res;
	vector<FaceCdt> onet_res;

	pnet_res = ProposalNet(img,threshold[0], factor);
	if (pnet_max_detect_num < (int)pnet_res.size()) {
		pnet_res.resize(pnet_max_detect_num);
		std::cout << "resizing" << endl;
	}
	int num = (int)pnet_res.size();
	cout << num << endl;
	int size = (int)ceil(1.f*num / step_size);

	//step_size=128,128作为一份送入检测
	for (int iter = 0; iter < size; ++iter) {

		
		int start = iter * step_size;
		int end = min(start + step_size, num);
		vector<FaceCdt> input(pnet_res.begin() + start, pnet_res.begin() + end);
		vector<FaceCdt> res = RefineNet(img, input, 24, 24,threshold[1]);
		rnet_res.insert(rnet_res.end(), res.begin(), res.end());
	}
	rnet_res = NMS(rnet_res, 0.5f, 'u');
	BBoxRegression(rnet_res);
	BBoxPadSquare(rnet_res, img.cols, img.rows);

	num = (int)rnet_res.size();
	size = (int)ceil(1.f*num / step_size);
	for (int iter = 0; iter < size; ++iter) {
		int start = iter * step_size;
		int end = min(start + step_size, num);
		vector<FaceCdt> input(rnet_res.begin() + start, rnet_res.begin() + end);
		vector<FaceCdt> res = OutputNet(img, input, 48, 48, threshold[2]);
		onet_res.insert(onet_res.end(), res.begin(), res.end());
	}
	
	onet_res = NMS(onet_res, 0.7f, 'm');
	BBoxRegression(onet_res);
	BBoxPad(onet_res, img.cols, img.rows);
	cout << "onet_size:" << onet_res.size() << endl;
	
	cout << "onet_size:" << onet_res.size() << endl;
	return onet_res;
}

vector<FaceCdt> mtcnn::ProposalNet(const cv::Mat & img,  float threshold, float factor)
{
	std::cout << "P-net-in" << endl;
	cv::Mat resized;
	int width = img.cols;
	int height = img.rows;
	float scale = factor;
	float std_len = std::min<int>(height, width)*scale;
	std::vector<float> scales;
	Blob<float>* input_layer = PNet_->input_blobs()[0];
	total_boxes_.clear();
	int ws = width;
	int hs = height;
	if (!(ws >= 12 && hs >= 12)) {
		std::cout << "Picture's lenght should reach 12 pixels at least." << endl;
	}
	ws = (int)std::ceil(width*scale);
	hs = (int)std::ceil(height*scale);
	//resize
	int counter = 0;
	while (ws >= 12&&hs>=12) {
		std::cout << "resize for " << ++counter << endl;
		cv::resize(img, resized, cv::Size(ws, hs), 0, 0, cv::INTER_LINEAR);//双线型插值resize
		input_layer->Reshape(1, 3, hs, ws);//输入层reshape成一个1张3通道高hs宽ws的卷积核，与即将输入的图片size对应
		PNet_->Reshape();
		float * input_data = input_layer->mutable_cpu_data();
		cv::Vec3b * img_data = (cv::Vec3b *)resized.data;
		int spatial_size = ws * hs;

		
		//输入图像，分别对三个通道标准化
		for (int k = 0; k < spatial_size; ++k) {
			input_data[k] = float((img_data[k][0] - mean_val)* std_val);
			input_data[k + spatial_size] = float((img_data[k][1] - mean_val) * std_val);
			input_data[k + 2 * spatial_size] = float((img_data[k][2] - mean_val) * std_val);
		}
		/*输入图像，分别对三个通道标准化changed
		for (int k = 0; k < spatial_size; ++k) {
			input_data[k] = float(img_data[k][0]);
			input_data[k + spatial_size] = float(img_data[k][1] );
			input_data[k + 2 * spatial_size] = float(img_data[k][2]);
		}*/
		PNet_->Forward();

		//softmax后的face检测结果（二分类）face classification
		Blob<float>* confidence = PNet_->blob_by_name("prob1").get();

		//bbox的回归数据bbox regression
		Blob<float>* reg = PNet_->blob_by_name("conv4-2").get();

		//反算bbox坐标
		GenerateBBox(confidence, reg, scale, threshold);
		std::vector<FaceCdt> bboxes_nms = NMS(candidate_boxes_, 0.5, 'u');
		if (bboxes_nms.size() > 0) {
			total_boxes_.insert(total_boxes_.end(), bboxes_nms.begin(), bboxes_nms.end());
		}
		scale *= factor;
		ws = (int)std::ceil(width*scale);
		hs = (int)std::ceil(height*scale);
	}
	int num_box = (int)total_boxes_.size();
	vector<FaceCdt> res_boxes;

	//进一步筛选
	if (num_box != 0) {
		res_boxes = NMS(total_boxes_, 0.7f, 'u');
		BBoxRegression(res_boxes);
		BBoxPadSquare(res_boxes, width, height);
	}
	std::cout << "P-net_out" << endl;
	return res_boxes;
}

vector<FaceCdt> mtcnn::RefineNet(const cv::Mat & image, vector<FaceCdt>& pre_stage_res, int input_w, int input_h, const float threshold)
{
	std::cout << "R-net-in" << endl;
	vector<FaceCdt> res;
	int batch_size = (int)pre_stage_res.size();
	if (batch_size == 0)
		return res;

	Blob<float>* input_layer = RNet_->input_blobs()[0];
	input_layer->Reshape(batch_size, 3, input_h, input_w);//input层reshape成128
	RNet_->Reshape();

	float * input_data = input_layer->mutable_cpu_data();
	int spatial_size = input_h * input_w;

	for (int n = 0; n < batch_size; ++n) {
		FaceBox &box = pre_stage_res[n].bbox;

		//截取bbox里的内容
		cv::Mat roi = image(cv::Rect(cv::Point((int)box.xmin, (int)box.ymin), cv::Point((int)box.xmax, (int)box.ymax))).clone();
		resize(roi, roi, cv::Size(input_w, input_h));
		float *input_data_n = input_data + input_layer->offset(n);
		cv::Vec3b *roi_data = (cv::Vec3b *)roi.data;

		//用 isContinuous()函数来判断图像数组是否为连续的(一行）
		CHECK_EQ(roi.isContinuous(), true);
		for (int k = 0; k < spatial_size; ++k) {
			input_data_n[k] = float((roi_data[k][0] - mean_val)*std_val);
			input_data_n[k + spatial_size] = float((roi_data[k][1] - mean_val)*std_val);
			input_data_n[k + 2 * spatial_size] = float((roi_data[k][2] - mean_val)*std_val);
		}
	}
	RNet_->Forward();

	//获得卷积层的结果
	Blob<float>* confidence = RNet_->blob_by_name("prob1").get();
	Blob<float>* reg_box = RNet_->blob_by_name("conv5-2").get();
	const float* confidence_data = confidence->cpu_data();
	const float* reg_data = reg_box->cpu_data();

	for (int k = 0; k < batch_size; ++k) {
		cout << confidence_data[2 * k] << endl;
		if (confidence_data[2 * k + 1] >= threshold) {
			FaceCdt facecdf(pre_stage_res[k].bbox, confidence_data[2 * k + 1]);
			facecdf.bbox.score = confidence_data[2 * k + 1];
			for (int i = 0; i < 4; ++i) {
				facecdf.bbox_reg[i] = reg_data[4 * k + i];
			}
			res.push_back(facecdf);
		}
		
	}
	std::cout << "R-net-out" << endl;
	return res;
}

vector<FaceCdt> mtcnn::OutputNet(const cv::Mat & image, vector<FaceCdt>& pre_stage_res, int input_w, int input_h, const float threshold)
{
	std::cout << "O-net-in" << endl;
	vector<FaceCdt> res;
	int batch_size = (int)pre_stage_res.size();
	if (batch_size == 0)
		return res;

	Blob<float>* input_layer = ONet_->input_blobs()[0];
	input_layer->Reshape(batch_size, 3, input_h, input_w);
	ONet_->Reshape();

	float * input_data = input_layer->mutable_cpu_data();
	int spatial_size = input_h * input_w;

	for (int n = 0; n < batch_size; ++n) {
		FaceBox &box = pre_stage_res[n].bbox;

		//截取bbox里的内容
		cv::Mat roi = image(cv::Rect(cv::Point((int)box.xmin, (int)box.ymin),cv:: Point((int)box.xmax, (int)box.ymax))).clone();
		resize(roi, roi,cv::Size(input_w, input_h));
		float *input_data_n = input_data + input_layer->offset(n);
		cv::Vec3b *roi_data = (cv::Vec3b *)roi.data;

		//用 isContinuous()函数来判断图像数组是否为连续的(一行）
		CHECK_EQ(roi.isContinuous(), true);
		for (int k = 0; k < spatial_size; ++k) {
			input_data_n[k] = float((roi_data[k][0] - mean_val)*std_val);
			input_data_n[k + spatial_size] = float((roi_data[k][1] - mean_val)*std_val);
			input_data_n[k + 2 * spatial_size] = float((roi_data[k][2] - mean_val)*std_val);
		}
	}
	ONet_->Forward();

	//获得卷积层的结果
	Blob<float>* confidence = ONet_->blob_by_name("prob1").get();
	Blob<float>* reg_box = ONet_->blob_by_name("conv6-2").get();
	Blob<float>* reg_landmark = ONet_->blob_by_name("conv6-3").get();
	const float* confidence_data = confidence->cpu_data();
	const float* reg_data = reg_box->cpu_data();
	const float* landmark_data = reg_landmark->cpu_data();

	for (int k = 0; k < batch_size; ++k) {
		if (confidence_data[2 * k + 1] >= threshold) {
			FaceCdt facecdf(pre_stage_res[k].bbox, confidence_data[2 * k + 1]);
			facecdf.bbox.score = confidence_data[2 * k + 1];
			for (int i = 0; i < 4; ++i) {
				facecdf.bbox_reg[i] = reg_data[4 * k + i];
			}
			float w = facecdf.bbox.xmax - facecdf.bbox.xmin + 1.f;
			float h = facecdf.bbox.ymax - facecdf.bbox.ymin + 1.f;
			if (showlandmark_) {
				for (int i = 0; i < 5; ++i) {
					facecdf.landmark[2 * i] = landmark_data[10 * k + 2 * i] * w + facecdf.bbox.xmin;
					facecdf.landmark[2 * i + 1] = landmark_data[10 * k + 2 * i + 1] * h + facecdf.bbox.ymin;
				}
			}
			res.push_back(facecdf);
		}

	}
	std::cout << "O-net-out" << endl;
	return res;
}

void mtcnn::BBoxRegression(vector<FaceCdt>& bboxes)
{

	for (int i = 0; i < bboxes.size(); ++i) {
		bboxes[i].regBbox();
	}
}

void mtcnn::BBoxPadSquare(vector<FaceCdt>& bboxes, int width, int height)
{

	for (int i = 0; i < bboxes.size(); ++i) {
		bboxes[i].bbox.makePadSquare(width, height);
	}
}

void mtcnn::BBoxPad(vector<FaceCdt>& bboxes, int width, int height)
{

	for (int i = 0; i < bboxes.size(); ++i) {
		bboxes[i].bbox.makePad(width, height);
	}
}

void mtcnn::GenerateBBox(Blob<float>* confidence, Blob<float>* reg_box, float scale, float thresh)
{
	int feature_map_w_ = confidence->width();
	int feature_map_h_ = confidence->height();
	int spatical_size = feature_map_h_ * feature_map_w_;
	const float* confidence_data = confidence->cpu_data() + spatical_size;//offset(spatical_size)
	const float* reg_data = reg_box->cpu_data();
	candidate_boxes_.clear();
	for (int i = 0; i < spatical_size; i++) {
	//	std::cout <<i<<" to "<< spatical_size << endl;
		if (confidence_data[i] >= thresh) {
			//std::cout << "confidence:"<<confidence_data[i] << endl;
			int y = i / feature_map_w_;
			int x = i - y * feature_map_w_;
			FaceCdt facecdt(x, y, scale, confidence_data[i], reg_data, i, spatical_size);
			candidate_boxes_.push_back(facecdt);
		}
	}
	std::cout << "Generate end" << endl;
}

std::vector<FaceCdt> mtcnn::NMS(std::vector<FaceCdt>& bboxes, float thresh, char methodType)
{
	std::vector<FaceCdt> nms_res;
	if (bboxes.size() == 0) {
		return nms_res;
	}
	std::sort(bboxes.begin(), bboxes.end());

	int select_idx = 0;
	int bbox_size= static_cast<int>(bboxes.size());
	std::vector<int> isMerged_list(bbox_size, 0);
	bool all_merged = false;

	//尝试将所有与当前所选的face重合大的去除
	while (!all_merged) {
		while (select_idx<bbox_size&&isMerged_list[select_idx]==1)
		{
			select_idx++;
		}
		if (select_idx == bbox_size) {
			all_merged = true;
			continue;
		}

		FaceCdt& select_one = bboxes[select_idx];
		nms_res.push_back(bboxes[select_idx]);
		isMerged_list[select_idx] = 1;
		select_idx++;

		for (int i = select_idx; i < bbox_size; i++) {
			if (isMerged_list[i] == 1)continue;
			if (select_one.isMerge(bboxes[i], methodType, thresh)) {
				isMerged_list[i] = 1;
			}
		}
	}
	cout << endl;
	return nms_res;
}

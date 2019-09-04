#include "FaceCdt.h"
#include <iostream>


FaceCdt::FaceCdt()
{
}

FaceCdt::~FaceCdt()
{
}

FaceCdt::FaceCdt(FaceBox x,float score)
{
	this->bbox.setFaceBox(x.xmin, x.ymin, x.xmax, x.ymax, score);
}

FaceCdt::FaceCdt(int x, int y, float scale, float score)
{
	this->bbox.setFaceBox((float)(x * pnet_stride) / scale,
		(float)(y * pnet_stride) / scale,
		(float)(x * pnet_stride + pnet_cell_slen - 1.f) / scale,
		(float)(y * pnet_stride + pnet_cell_slen - 1.f) / scale,
		score);
	float check = bbox.xmin;
}

FaceCdt::FaceCdt(int x, int y, float scale, float score, const float *reg_data, int i,int sptcl_size) {
	this->bbox.setFaceBox((float)(x * pnet_stride) / scale,
		(float)(y * pnet_stride) / scale,
		(float)(x * pnet_stride + pnet_cell_slen - 1.f) / scale,
		(float)(y * pnet_stride + pnet_cell_slen - 1.f) / scale,
		score);
	this->bbox_reg[0] =reg_data[i];
	this->bbox_reg[1] = reg_data[i + sptcl_size];
	this->bbox_reg[2] = reg_data[i + 2 * sptcl_size];
	this->bbox_reg[3] = reg_data[i + 3 * sptcl_size];
}
void FaceCdt::regBbox()
{
	float w = bbox.width();
	float h = bbox.height();
	bbox.addLen(bbox_reg[0] * w, bbox_reg[1] * h, bbox_reg[2] * w, bbox_reg[3] * h);
}
bool FaceCdt::operator==(const FaceCdt & x) const
{
	return this->bbox.score==x.bbox.score;
}
//µ¹ÐòÊä³ö
bool FaceCdt::operator<(const FaceCdt & x) const
{
	return this->bbox.score>x.bbox.score;
}

bool FaceCdt::isMerge(FaceCdt& x,char methodType = 'u', float thresh = 0.7f)const
{
	float area_com = bbox.commonArea(x.bbox);
	float area_this = bbox.area();
	float area_x = x.bbox.area();

	if (area_com == 0){
		return false;
	}

	switch (methodType)
	{case 'u':
		
		if (static_cast<float>(area_com) / (area_this + area_x - area_com) > thresh)
			return true; break;
	case 'm':
		std::cout << "area_com:" << area_com << ", area_this:" << area_this << ",area_x<<" << area_x << std::endl;
		std::cout << "u_IoU:" << static_cast<float>(area_com) / (area_this + area_x - area_com) << std::endl;
		std::cout << "m_IoU:" << static_cast<float>(area_com) / std::min<float>(area_this, area_x) << std::endl;
		if (static_cast<float>((area_com) / std::min<float>(area_this, area_x)) > thresh) {
			return true; break;
		}
	default:
		break;
	}
	return false;
}

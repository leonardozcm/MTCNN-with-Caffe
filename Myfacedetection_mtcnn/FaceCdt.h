#pragma once
#include "FaceBox.h"

const float pnet_stride = 2;
const float pnet_cell_slen = 12;

//储存预选人脸框信息
class FaceCdt 
{
public:
	float bbox_reg[4];
	float landmark[10];
	FaceBox bbox;
	FaceCdt();
	~FaceCdt();
	//void updateBbox(int x, int y,float scale);
	FaceCdt(FaceBox x,float score);
	FaceCdt(int x, int y, float scale,float score);
	FaceCdt(int x, int y, float scale, float score,const float *reg_data,int i,int sptcl_size);

	//regression回归操作Bbox
	void regBbox();

	bool operator ==(const FaceCdt& x)const;
	bool operator < (const FaceCdt& x)const;
	//判断是否可以合并两个区域
	bool isMerge(FaceCdt& x, char methodType, float thresh)const;
};


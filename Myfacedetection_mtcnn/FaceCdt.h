#pragma once
#include "FaceBox.h"

const float pnet_stride = 2;
const float pnet_cell_slen = 12;

//����Ԥѡ��������Ϣ
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

	//regression�ع����Bbox
	void regBbox();

	bool operator ==(const FaceCdt& x)const;
	bool operator < (const FaceCdt& x)const;
	//�ж��Ƿ���Ժϲ���������
	bool isMerge(FaceCdt& x, char methodType, float thresh)const;
};


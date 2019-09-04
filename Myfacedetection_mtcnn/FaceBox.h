#pragma once
#include <algorithm>
class FaceBox
{
public:
	float xmin;
	float ymin;
	float xmax;
	float ymax;
	float score;
	FaceBox();
	//FaceBox(float xmin, float yxim, float xmax, float ymax, float score);
	~FaceBox();

	float width()const;
	float height()const;

	void setFaceBox(float xmin, float yxim, float xmax, float ymax, float score);
	void addLen(float deltax1, float deltay1, float deltax2, float deltay2);

	//return ѡ������
	float area()const;
	//return �ཻ��������IoU
	float commonArea(FaceBox& x)const;

	//��ֹ���򳬳�ͼƬ��Χ
	void makePad(int width, int height);
	//ѡ�������λ���ֹͼ�����
	void makePadSquare(int width, int height);
};


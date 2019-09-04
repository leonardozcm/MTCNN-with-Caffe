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

	//return 选框的面积
	float area()const;
	//return 相交区域的面积IoU
	float commonArea(FaceBox& x)const;

	//防止区域超出图片范围
	void makePad(int width, int height);
	//选框正方形化防止图像变形
	void makePadSquare(int width, int height);
};


#include "FaceBox.h"


FaceBox::FaceBox()
{
}


FaceBox::~FaceBox()
{
}

float FaceBox::width()const
{
	return xmax-xmin+1.f;
}

float FaceBox::height()const
{
	return ymax-ymin+1.f;
}
/**
FaceBox::FaceBox(float xmin, float ymin, float xmax, float ymax, float score) {
	this->xmin = xmin;
	this->ymin = ymin;
	this->xmax = xmax;
	this->ymax = ymax;
	this->score = score;
}
**/
void FaceBox::setFaceBox(float xmin, float ymin, float xmax, float ymax, float score)
{
	this->xmin = xmin;
	this->ymin = ymin;
	this->xmax = xmax;
	this->ymax = ymax;
	this->score = score;
}

void FaceBox::addLen(float deltax1, float deltay1, float deltax2, float deltay2) {
	this->xmin += deltax1;
	this->ymin += deltay1;
	this->xmax += deltax2;
	this->ymax += deltay2;
}

float FaceBox::area()const
{
	return static_cast<float>(width()*height());
}

float FaceBox::commonArea(FaceBox & x)const
{
	float xmin_com = std::max<float>(static_cast<float>(xmin), static_cast<float>(x.xmin));
	float ymin_com = std::max<float>(static_cast<float>(ymin), static_cast<float>(x.ymin));
	float xmax_com = std::min<float>(static_cast<float>(xmax), static_cast<float>(x.xmax));
	float ymax_com = std::min<float>(static_cast<float>(ymax), static_cast<float>(x.ymax));
	float w = xmax_com - xmin_com + 1;
	float h = ymax_com - ymin_com + 1;
	if (w <= 0 || h <= 0)
		return 0.f;
	return static_cast<float>(w * h);
}

void FaceBox::makePad(int width, int height)
{
	xmin = round(std::max<float>(xmin, 0.f));
	ymin = round(std::max<float>(ymin, 0.f));
	xmax = round(std::min<float>(xmax, width - 1.f));
	ymax = round(std::min<float>(ymax, height - 1.f));
}

void FaceBox::makePadSquare(int width, int height)
{
	float w = this->width();
	float h = this->height();
	float std_len = h > w ? h : w;
	xmin = round(std::max<float>(xmin + (w - std_len)*0.5f, 0.f));
	ymin = round(std::max<float>(ymin + (h - std_len)*0.5f, 0.f));
	xmax = round(std::min<float>(xmin + std_len - 1, width - 1.f));
	ymax = round(std::min<float>(ymin + std_len - 1, height - 1.f));

}
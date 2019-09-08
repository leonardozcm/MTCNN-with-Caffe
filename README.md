# MTCNN-with-Caffe-in-C++
A implement to MTCNN.
Joint Face Detection and Alignment using Multi-task Cascaded Convolutional Neural Networks
paper url: [spl.pdf](https://github.com/happynear/MTCNN_face_detection_alignment/blob/master/paper/spl.pdf)

## Requirement
1. Caffe:https://github.com/BVLC/caffe, 中文版目前坑最少的编译环境配置帮助文档：https://blog.csdn.net/fengtaoO08/article/details/83023428 
    有问题欢迎提在issues里。
2. VS2017：https://visualstudio.microsoft.com/zh-hans/thank-you-downloading-visual-studio/?sku=Community&rel=15
3. library maybe inneed：[opencv3.1](https://github.com/opencv/opencv/archive/3.1.0.zip),[OpenBLAS-0.3.6](http://www.openblas.net/)
4. dataset:[widerface](http://shuoyang1213.me/WIDERFACE/),[celebA](http://mmlab.ie.cuhk.edu.hk/projects/CelebA.html)

## Result 
![MTCNN-facedetection](https://github.com/leonardozcm/MTCNN-with-Caffe/blob/master/Myfacedetection_mtcnn/image/0.jpg)

## 配置vs2017
需要配置一些依赖项

### 属性页
1. 常规-》***平台工具集（v141）***
![常规](https://github.com/leonardozcm/MTCNN-with-Caffe/blob/master/Myfacedetection_mtcnn/image/vscompile01.jpg)
2. 调试-》***包含目录、库目录***
![调试](https://github.com/leonardozcm/MTCNN-with-Caffe/blob/master/Myfacedetection_mtcnn/image/vscompile02.jpg)
3. C/C++-》***常规***
![C/C++](https://github.com/leonardozcm/MTCNN-with-Caffe/blob/master/Myfacedetection_mtcnn/image/vscompile03.jpg)
4. 链接器-》***输入***，配置静态库
![链接器](https://github.com/leonardozcm/MTCNN-with-Caffe/blob/master/Myfacedetection_mtcnn/image/vscompile04.jpg)

## 训练
将wilderFace数据集解压放到RaiseData文件夹下，UNcomment掉12的netsize，运行生产12x12的样本图片，然后24、48.
值得注意的是，48net的FP率会偏高，需要生产hardexample来强化学习，生成这个样板的脚本是gen_hard_example.py.
最后生成imdb格式数据集就可以训练了。
我的训练网络提供在RaiseData文件夹下，需要用得话记得修改路径。

## 运行
运行Myfacedetection_mtcnn.cpp,选择图片输入，进过卷积网络后得到输出。
!(https://github.com/leonardozcm/MTCNN-with-Caffe/blob/master/Myfacedetection_mtcnn/image/屏幕截图(43).jpg)
!(https://github.com/leonardozcm/MTCNN-with-Caffe/blob/master/Myfacedetection_mtcnn/image/屏幕截图(44).jpg)

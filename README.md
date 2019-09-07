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
属性页
常规-》***平台工具集（v141）***
调试-》***包含目录、库目录***
C/C++-》***常规***
链接器-》***输入***，配置静态库

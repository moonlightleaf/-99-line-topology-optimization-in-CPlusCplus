//����Ԫ����ӳ���
//����ֵΪȫ�ֽڵ�λ��
#pragma once
#include"lk.h"
#include<vector>
#include<list>
#include<opencv.hpp>
cv::Mat* FE(const int nelx, const int nely, const cv::Mat& x, const float penal, cv::Mat* ptrU);
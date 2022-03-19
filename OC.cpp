#include"OC.h"
cv::Mat OC(const int nelx, const int nely, const cv::Mat& x, const float volfrac, const cv::Mat& dc) {
	//����һ�����䣬ʹ�ö��ַ�ȷ���������Լ����������������
	float l1 = 0.f, l2 = 100000.f;
	//�������仯
	float move = 0.2f;
	//���ַ��õ�lambda
	float lmid;
	cv::Mat xnew(x.rows, x.cols, CV_32FC1);
	cv::Mat temp(x.rows, x.cols, CV_32FC1);
	
	while (l2 - l1 > 1e-4f) {
		//���ַ���ȡ�����м��
		lmid = (l2 + l1) / 2;
		//����4�ж�Ӧ��matlab����Ϊxnew = max(0.001, max(x - move, min(1., min(x + move, x.*sqrt(-dc. / lmid)))));
		cv::Mat localdc = -1.0 / lmid * dc;
		cv::sqrt(localdc, temp);
		temp = temp.mul(x);
		temp = cv::min(x + move, temp);
		temp = cv::min(1.0f, temp);
		temp = cv::max(x - move, temp);
		xnew = cv::max(0.001f, temp);
		
		if (cv::sum(xnew)[0] - volfrac * float(nelx)*float(nely) > 0)
			l1 = lmid;
		else l2 = lmid;
	}
	return xnew;
}
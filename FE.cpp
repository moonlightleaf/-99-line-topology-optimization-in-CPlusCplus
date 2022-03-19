#include"FE.h"
cv::Mat* FE(const int nelx, const int nely, const cv::Mat& x, const float penal, cv::Mat* ptrU) {
	//���㵥Ԫ�նȾ���
	cv::Mat *ptrKE = new cv::Mat(lk());
	//����նȾ���
	cv::Mat *ptrK = new cv::Mat(cv::Mat::zeros(cv::Size(2 * (nelx + 1)*(nely + 1), 2 * (nelx + 1)*(nely + 1)), CV_32FC1));
	//������
	cv::Mat *ptrF = new cv::Mat(cv::Mat::zeros(cv::Size(1, 2 * (nelx + 1)*(nely + 1)), CV_32FC1));
	//ȫ�ֽڵ�λ�ƾ���
	*ptrU = cv::Mat::zeros(cv::Size(1, 2 * (nelx + 1)*(nely + 1)), CV_32FC1);
	for (int elx = 0; elx < nelx; ++elx) {
		for (int ely = 0; ely < nely; ++ely) {
			//�������Ͻ�n1�����Ͻ�n2��Ԫ�ڵ���,��Ԫ�ı�Ź����Ǵ�0��ʼ�������е���������cpp����ϰ�ߣ�����matlab��1����
			int n1 = (nely + 1)*elx + ely;
			int n2 = (nely + 1)*(elx + 1) + ely;
			//��װ����նȾ���ʱ��Ҫ�����λ������
			std::vector<int> edof = { 2 * n1, 2 * n1 + 1, 2 * n2,2 * n2 + 1, 2 * n2 + 2, 2 * n2 + 3, 2 * n1 + 2, 2 * n1 + 3 };
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					(*ptrK).at<float>(edof[i], edof[j]) += pow(x.at<float>(ely, elx), penal)*(*ptrKE).at<float>(i, j);
				}
			}
		}
	}
	//ʩ���غ�,����Ӧ����һ�����ϽǵĴ�ֱ��Ԫ��
	(*ptrF).at<float>(1, 0) = -1;
	//ʩ��Լ��,��ߵ�һ�к����½�Լ��,�Ѿ��̶������ɶ���ȫ��λ�ƾ����е������±꣬Լ�����x�����½ǵ�y�������ɶ�
	std::vector<int> fixeddofs;
	for (int i = 0; i < nely; ++i) {
		fixeddofs.push_back(i * 2);
	}
	fixeddofs.push_back(2 * (nelx + 1)*(nely + 1) - 1);
	//ʣ�µ���Լ�����ɶ���ȫ��λ�ƾ����е��±�����
	std::list<int> prefreedofs;
	for (int i = 0; i < 2 * (nelx + 1)*(nely + 1); ++i) {
		prefreedofs.push_back(i);
	}
	for (const auto& i : fixeddofs) {
		prefreedofs.remove(i);
	}
	std::vector<int> freedofs(prefreedofs.begin(), prefreedofs.end());//���vector��������������������
	//������Է����飬�õ����ڵ�x��y�����λ��ֵ������U��
	cv::Mat *ptrcalculatingK = new cv::Mat(cv::Mat::zeros(cv::Size(freedofs.size(), freedofs.size()), CV_32FC1));
	//cv::Mat calculatingK = cv::Mat::zeros(cv::Size(freedofs.size(), freedofs.size()), CV_32FC1);
	for (int i = 0; i < freedofs.size(); ++i) {
		for (int j = 0; j < freedofs.size(); ++j) {
			(*ptrcalculatingK).at<float>(i, j) = (*ptrK).at<float>(freedofs[i], freedofs[j]);
		}
	}
	cv::Mat *ptrcalculatingF = new cv::Mat(cv::Mat::zeros(cv::Size(1, freedofs.size()), CV_32FC1));
	//cv::Mat calculatingF = cv::Mat::zeros(cv::Size(1, freedofs.size()), CV_32FC1);
	for (int i = 0; i < freedofs.size(); ++i) {
		(*ptrcalculatingF).at<float>(i, 0) = (*ptrF).at<float>(freedofs[i], 0);
	}
	cv::Mat *ptrcalculatingU = new cv::Mat(cv::Mat::zeros(cv::Size(1, freedofs.size()), CV_32FC1));
	//cv::Mat calculatingU = cv::Mat::zeros(cv::Size(1, freedofs.size()), CV_32FC1);
	*ptrcalculatingU = (*ptrcalculatingK).inv(cv::DECOMP_SVD)*(*ptrcalculatingF);
	
	//�õ����ڼ����calculatingU�󣬽��䰴λ�����ԭ����ȫ��U
	for (int i = 0; i < freedofs.size(); ++i) {
		(*ptrU).at<float>(freedofs[i], 0) = (*ptrcalculatingU).at<float>(i, 0);
	}
	delete ptrKE;
	delete ptrK;
	delete ptrF;
	
	delete ptrcalculatingF;
	delete ptrcalculatingK;
	delete ptrcalculatingU;
	/*std::cout << "U:" << std::endl;
	std::cout << (*ptrU).t() << std::endl;*/
	return ptrU;
}

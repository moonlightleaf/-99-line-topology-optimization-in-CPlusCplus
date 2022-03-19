#include"top.h"
void top(const int nelx, const int nely, const float volfrac, const float penal, const float rmin) {
	//��ʼ����Ʊ���xΪ�������
	cv::Mat x = cv::Mat::ones(cv::Size(nelx, nely), CV_32FC1);
	x = x * volfrac;
	//����ѭ������
	int loop = 0;
	//����ÿ�ε�������Ʊ����ı仯���������ж��Ƿ�����
	float change = 1.f;
	//���ڱ�����һ�ε��������Ʊ���
	cv::Mat xold(nely, nelx, CV_32FC1);
	//��Ԫ�նȾ���
	const cv::Mat KE = lk().clone();
	/*****************************************************************************************************************************/
	//һЩ������ǰ����һ���ڴ棬��ֹ�ڴ治���õ��¼���������
	
	//��Ʊ�����x��xold   ��û����
	cv::Mat *ptrx = new cv::Mat(cv::Mat::ones(cv::Size(nelx, nely), CV_32FC1));
	*ptrx = *ptrx * volfrac;
	cv::Mat *ptrxold = new cv::Mat(*ptrx);
	//ȫ�ֽڵ�λ�ƾ���
	cv::Mat *ptrU = new cv::Mat(cv::Mat::zeros(cv::Size(1, 2 * (nelx + 1)*(nely + 1)), CV_32FC1));
	/*****************************************************************************************************************************/

	//ѭ������
	while (change > 0.01) {
		++loop;

		//�ȱ�����һ�ε���Ʊ���
		xold = x.clone();
		//ÿ�ε���������һ������Ԫ������������λ�ƣ���������ȫ��λ������U��
		ptrU = FE(nelx, nely, x, penal, ptrU);
		//���ڱ���Ŀ�꺯���ı���
		float c = 0.f;
		//���ڱ�������
		cv::Mat dc(nely, nelx, CV_32FC1);
		dc = 0.f*dc;

		//������������Ԫ�أ������Ͻǿ�ʼ��һ��һ��
		for (int ely = 0; ely < nely; ++ely) {
			for (int elx = 0; elx < nelx; ++elx) {
				//�������Ͻ�n1�����Ͻ�n2��Ԫ�ڵ���,��Ԫ�ı�Ź����Ǵ�0��ʼ�������е���������cpp����ϰ�ߣ�����matlab��1����
				int n1 = (nely + 1)*elx + ely;
				int n2 = (nely + 1)*(elx + 1) + ely;
				//�õ���ǰ��Ԫ��λ��������edof�ǵ�ǰ��Ԫ���ɶ���ȫ��λ�ƾ����е��±�����
				std::vector<int> edof = { 2 * n1, 2 * n1 + 1, 2 * n2,2 * n2 + 1, 2 * n2 + 2, 2 * n2 + 3, 2 * n1 + 2, 2 * n1 + 3 };
				cv::Mat Ue(8, 1, CV_32FC1);//�����洢��Ԫλ������
				for (int i = 0; i < 8; ++i) {
					Ue.at<float>(i, 0) = (*ptrU).at<float>(edof[i], 0);
				}
				
				//��������ṹ���ֵ
				cv::Mat temp = Ue.t()*KE*Ue;
				c += pow(x.at<float>(ely, elx), penal)*temp.at<float>(0, 0);
				//����ÿ������ֵ����dc��Ӧλ��
				dc.at<float>(ely, elx) = -1 * penal*pow(x.at<float>(ely, elx), penal - 1)*temp.at<float>(0, 0);
			}
		}
		//�޹��������ȹ���
		dc = check(nelx, nely, rmin, x, dc).clone();
		//����OC�����Ʊ���
		x = OC(nelx, nely, x, volfrac, dc);
		//�仯��������Ʊ�����changeֵ�������и����ֱ��������С��ȡ�����о���ֵ������Ϊchangeֵ
		cv::Mat xchange = x - xold;
		double changeMin, changeMax;
		cv::minMaxLoc(xchange, &changeMin, &changeMax);
		change = abs(changeMax) > abs(changeMin) ? abs(changeMax) : abs(changeMin);

		//��ӡ��Ϣ
		std::cout << "ѭ�������� " << loop << " Ŀ�꺯���� " << c 
			<< " ��������� " << cv::sum(x)[0] / (nelx*nely) << " ��Ʊ����仯�� " << change << std::endl;
	}
	std::cout << x << std::endl;
}
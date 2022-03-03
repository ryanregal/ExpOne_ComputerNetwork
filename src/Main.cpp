#include<iostream>
#include<opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>
#include<math.h>
#include<vector>
#include<strstream>
#include<fstream>
#include <windows.h>
#include <cstdlib>
#include <algorithm>
using namespace std;
using namespace cv;
void QR_Draw(string code, int flag);

int binary_digit[8] = { 0 };//�洢�ַ��Ķ����Ʊ���
vector<Mat>img;//����ͼƬ
string verifyname;//CRC��������֤���ַ���

/*---------------------------------------------���벿��---------------=---------------------------------------*/

/*-------CRCУ��λ�ļ���-------*/

//CRCΪ������ԭʼ����dataΪ��������ģ������
string CalculateCRC(string data, string& CRC) {
	data.append(string(CRC.size(), '0'));//��ԭʼ����data�������У����λ����k����0
	//ģ2����
	for (int i = 0; i < data.size() - CRC.size(); i++) {
		//�����0����������0�������������������
		if (data[i] == '1') {
			for (int j = 0; j < CRC.size(); j++) {
				//������㣬��ͬ0����ͬ1
				(data[i + j] == CRC[j]) ? data[i + j] = '0' : data[i + j] = '1';
			}
		}
	}
	return data.substr(data.size() - CRC.size(), CRC.size());//��������(��kλ)
}

//��ȡ�����ļ��ַ���������crcУ���룬����У�����ݴ���
void Encode_Crc(string code, int flag) {
	string CRC = "10000010011000001000111011011011";
	code.append(CalculateCRC(code, CRC));//��code�м������õ�������
	QR_Draw(code, flag);//���ƶ�ά��
}


/*-------��ͼ����-------*/

//Position Detection Pattern����λ���Σ����ƣ��úڰ׺�3�����ο����
void Draw_Block(Mat img, Point tr) {
	rectangle(img, Point(0, 0) + tr, Point(105, 105) + tr, Scalar(0, 0, 0), -1);//������ľ��ο�RGB����ɫ��
	rectangle(img, Point(15, 15) + tr, Point(90, 90) + tr, Scalar(255, 255, 255), -1);//�м�ľ��ο�,RGB����ɫ��
	rectangle(img, Point(30, 30) + tr, Point(75, 75) + tr, Scalar(0, 0, 0), -1);//������ľ��ο�,RGB����ɫ��
}

//ȷ��������ɫ
void Help_Draw(string code, int& R, int& G, int& B, int& key) {
	if (code[key] == '0' && code[key + 1] == '0') { R = 255; G = 255; B = 0; }//��ɫ:00
	if (code[key] == '0' && code[key + 1] == '1') { R = 255; G = 0; B = 0; }//��ɫ:01
	if (code[key] == '1' && code[key + 1] == '0') { R = 0; G = 255; B = 0; }//��ɫ:10
	if (code[key] == '1' && code[key + 1] == '1') { R = 0; G = 0; B = 255; }//��ɫ:11
	key = key + 2;//���������أ��������¶�ȡcode����
}

//QR��ά��ͼ����
void QR_Draw(string code, int flag) {
	int key = 0;//code����ı������
	int R = 255, G = 255, B = 255;//�����ͨ����ֵ
	Mat src(780, 780, CV_8UC3, Scalar(255, 255, 255));//���ص����src���������ͣ���ɫ��
	Point recSize(10, 10);//���ش�С

	//Position Detection Pattern����λ���Σ�����
	Draw_Block(src, Point(0 + 30, 0 + 30));//����
	Draw_Block(src, Point(615 + 30, 0 + 30));//����
	Draw_Block(src, Point(0 + 30, 615 + 30));//����
	Draw_Block(src, Point(615 + 30, 615 + 30));//����

	//�м䲿�֣�һ��һ��Ϳ
	for (int col = 0; col < 72; col++) {//�м�һ��72��
		for (int row = 0; row < 48; row++) {//�м�һ��48��
			Point pos(row * 10 + 120 + 30, col * 10 + 30);//ȷ������λ��,120����Ϊ��λ���Σ�30�Ƕ�ά�����沿��
			Help_Draw(code, R, G, B, key);//����������ɫ
			rectangle(src, pos, pos + recSize, Scalar(B, G, R), -1);//��������
			//��ͼ�������һ��ʱ����src����img��
			if (flag == 1 && key >= code.size()) {
				//��ͼ�������һ��ʱ������Last_Draw��
				if (flag == 1 && key >= code.size())  goto Last_Draw;
			}
		}
	}

	//������Ĳ��֣�һ��һ��Ϳ
	for (int col = 0; col < 48; col++) {//���˶�λ����һ��48��
		for (int row = 48; row < 60; row++) {//һ��12��
			Point pos(row * 10 + 120 + 30, col * 10 + 120 + 30);//ֻ�ܴ�120��ʼ����ǰ���Ƕ�λ�ķ���
			Help_Draw(code, R, G, B, key);//����������ɫ
			rectangle(src, pos, pos + recSize, Scalar(B, G, R), -1);//��������
			//��ͼ�������һ��ʱ����src����img��
			if (flag == 1 && key >= code.size()) {
				//��ͼ�������һ��ʱ������Last_Draw��
				if (flag == 1 && key >= code.size())  goto Last_Draw;
			}
		}
	}

	//������Ĳ��֣�һ��һ��Ϳ
	for (int col = 0; col < 48; col++) {
		for (int row = 0; row < 12; row++) {
			Point pos(row * 10 + 30, col * 10 + 120 + 30);//ֻ�ܴ�120��ʼ����ǰ���Ƕ�λ�ķ���
			Help_Draw(code, R, G, B, key);//����������ɫ
			rectangle(src, pos, pos + recSize, Scalar(B, G, R), -1);//��������
			//��ͼ�������һ��ʱ����src����img��
			if (flag == 1 && key >= code.size()) {
				//��ͼ�������һ��ʱ������Last_Draw��
				if (flag == 1 && key >= code.size())  goto Last_Draw;
			}
		}
	}
Last_Draw:
	img.push_back(src);
}

/*-------��������-------*/

//������Ƶ
void Get_Video() {
	char videoname[20];
	cout << "Please input the encoded video file name:��e.g: 1.mp4��" << endl;
	cin >> videoname;
	int frame_rate = 10;//��Ƶ֡��Ĭ��Ϊ10
	cout << "Video frame rate defaults to 10" << endl;
	VideoWriter video(videoname, CAP_ANY, frame_rate, Size(780, 780));//����Ƶ�����videoname
	for (size_t i = 0; i < img.size(); i++)//i:long long unsigned int
	{
		Mat image = img[i].clone();
		video << image;// ������������ͼƬ������Ƶ
	}
}

//���뺯����ASCIIת2���ƣ��õ���2�����ַ��洢��binary_digit������
void Binary_Code(unsigned char a) {
	for (int j = 7; j >= 0; j--) {
		binary_digit[j] = a % 2;
		a = a / 2;
	}
}


/*---------------------------------------------���벿��---------------=---------------------------------------*/

//��ȡ��Ƶ
void Read_Video(string videopath, vector<Mat>& srcImages) {
	VideoCapture capture(videopath);//ʹ��cv2�⺯����ȡ��Ƶ,������srcImages��
	Mat frame;

	while (1) {//��ȡ��Ƶ֡
		capture >> frame;
		if (frame.empty()) break;//ֹͣ��ȡ
		else {
			resize(frame, frame, Size(720, 720), 0, 0, INTER_NEAREST);//������С��ʹ������ڲ�ֵ��
			srcImages.push_back(frame.clone());//����֡��¡��srcImages��
			frame.release();//�ͷ���Ƶ��
		}
	}
}

/*----------------ͼ����--------------------*/

//��ͼ������������ȥ����
Mat Handle_Img(Mat& srcImg) {
	Mat midImg = srcImg.clone();
	cvtColor(midImg, midImg, CV_RGB2GRAY);//��ɫת�Ҷ�
	for (int i = 0; i < 5; i++) {
		GaussianBlur(midImg, midImg, Size(3, 3), 0);//��˹�˲���ƽ��ͼ�񣬼���
	}
	threshold(midImg, midImg, 100, 255, THRESH_BINARY); //��ֵ������ֵ��
	return midImg;
}

//���ĸ���λ���ΰ�˳������
//ȱ�㣺�����ֻ�ֻ������������Ƶ
//�Ľ�������������λ�㣬���������ν��ж�λ��ͬʱ�����жϷ���
Point2f* Sort_Corner(vector<Point2f>& mc) {
	Point2f* p = new Point2f[4];//�洢����õľ������ĵ�
	float sum[4];
	for (int i = 0; i < mc.size(); i++) {
		sum[i] = mc[i].x + mc[i].y;//sum���鴢��x+y
	}
	int bottomright = max_element(sum, sum + 4) - sum;//������󣬼�sum�õ�����
	int topleft = min_element(sum, sum + 4) - sum;//������С����sum�õ�����
	int topright = 0, bottomleft = 0;
	//ȷ�����º�����
	for (int i = 0; i < mc.size(); i++) {
		if (i == bottomright || i == topleft)  continue;//�Ѿ�ȷ���ˣ���������
		if (mc[i].x > mc[i].y)
			topright = i;//���ϵ�x>y
		if (mc[i].x < mc[i].y)
			bottomleft = i;//���µ�y>x
	}
	p[0] = mc[topleft];//����
	p[1] = mc[topright];//����
	p[2] = mc[bottomleft];//����
	p[3] = mc[bottomright];//����
	return p;
}

//������ε�����
vector<Point2f> Get_Center(vector<vector<Point>>& corner) {
	vector<Moments> mu(corner.size());//����һ��vector��Ԫ�ظ���Ϊcorner.size()��
	//����ÿ�����������о�
	for (int i = 0; i < corner.size(); i++) {
		mu[i] = moments(corner[i], false);//opencv���������������������ߴ��������о�
		//Ĭ�Ͼ���false��true��Ҫ�ٴζ�ֵ��
	}
	//��������������
	vector<Point2f> mc(corner.size());//�洢���ĵľ���
	for (int i = 0; i < corner.size(); i++) {
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		// ���ĵ� X,Y ���꣺(m10/m00, m01/m00),m10 etc��moments�ڲ���Ա
	}
	return mc;//��������
}

//�ҵ���λ���Σ�������������
vector<vector<Point>> Find_Corner(Mat& midImg) {
	vector<vector<Point>> contours, corner;//contours��������������corner���涨λ����
	vector<Vec4i> hierarchy;
	//opencv�������洢���еľ��Σ��㼶���洢��Ĭ�ϴ���С��⿪ʼ
	findContours(midImg, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

	int index = -1, mark = 0;
	for (int i = 0; i < contours.size(); i++)//�ҵ��ĸ���λ���ε������Ž�corner
	{
		if (contourArea(contours[i]) > 50000) continue;//���������µ����������50000���ǣ��˳�
		//-1��ʾû�ж�Ӧ�ĸ��������������������
		if (hierarchy[i][2] != -1 && mark == 0) {//��һ���ҵ���������
			index = i;//��λ
			mark++;
		}
		else if (hierarchy[i][2] != -1) {//�ҵ���������
			mark++;
		}
		else if (hierarchy[i][2] == -1) {//�ҵ���������
			mark = 0;//mark��0
			index = -1;//index����
		}
		//���ҵ�2����λ����Ϣ����ʱ��λ����ȷ��
		if (mark >= 2) {
			corner.push_back(contours[index]);//����λ���δ���corner��
			mark = 0;//ic��0
			index = -1;//index����
		}
	}
	return corner;
}

//��ȡͼƬ����
void Get_ROI(Mat& midImg, Mat& srcImg, Mat& dstImg) {
	vector<vector<Point>> corner = Find_Corner(midImg); //�ҵ���ά���ĸ���λ
	if (corner.size() == NULL || int(corner.size()) != 4)  return; //corner��ȡ�����򷵻�
	vector<Point2f> centerpoint(corner.size()); //�����ĸ���������ģ�2f�����ά����
	centerpoint = Get_Center(corner); //�õ����ε�����
	Point2f* P1;
	Point2f P2[4];//����͸�ӱ任��ĵ�λ
	P1 = Sort_Corner(centerpoint); //�ĸ����ΰ������ϣ����ϣ����£���������
	P2[0] = Point2f(52, 52); //��Ӧ͸�ӱ任��ĵ�λ
	P2[1] = Point2f(668, 52);
	P2[2] = Point2f(52, 668);
	P2[3] = Point2f(668, 668);
	//����任�������͸�ӱ任
	Mat matrix = getPerspectiveTransform(P1, P2);
	//��͸�ӱ任���ͼƬ������dstImg��
	warpPerspective(srcImg, dstImg, matrix, srcImg.size(), 1, 0, Scalar(255, 255, 255));
}


/*-----------------ͼƬ����------------------*/

//����Decode��������������ɫ��Ϣ��ش���
//color��{B,G,R}
void Help_Decode(Scalar& color, string& code) {
	if (color[1] > 100 && color[2]>100 && color[0]<1.5*color[1]&& color[0] < 1.5 * color[2]) {
		code += "00";//��ɫ	
	}
	else if (color[0] < color[2] / 1.5 && color[1] < color[2] / 1.5) code += "01";//��ɫ
	else if (color[0] < color[1] / 1.5 && color[2] < color[1] / 1.5) code += "10";//��ɫ
	else if (color[1] < color[0] / 1.5 && color[2] < color[0] / 1.5) code += "11";//��ɫ
}

//�������Ҫ����
string Decode(Mat& img, ofstream& verify) {
	string code;
	string CRC = "10000010011000001000111011011011";
	int endFlag = 0;//��ʾ�ļ��Ƿ����

	//����
	//�м�Ĳ���
	for (int col = 0; col < 72; col++) {
		for (int row = 0; row < 48; row++) {
			Point pos(row * 10, col * 10), center(5, 5), tr(120, 0);
			Scalar color = img.at<Vec3b>(tr + pos + center);//atȡ����(i,j)λ�õ���ͨ������ֵ
			if (color[0] > 180 && color[1] > 180 && color[2] > 180) {//�����˰�ɫ�����أ���ʾ���ܽ�����
				endFlag = 1;
				goto endjudge;//����У��
			}
			Help_Decode(color, code);
		}
	}
	//�����沿��
	for (int col = 0; col < 48; col++) {
		for (int row = 48; row < 60; row++) {
			Point pos(row * 10, col * 10), center(5, 5), tr(120, 120);
			Scalar color = img.at<Vec3b>(tr + pos + center);//atȡ����(i,j)λ�õ���ͨ������ֵ
			if (color[0] > 180 && color[1] > 180 && color[2] > 180) {//�����˰�ɫ�����أ���ʾ���ܽ�����
				endFlag = 1;
				goto endjudge;//����У��
			}
			Help_Decode(color, code);
		}
	}
	//�����沿��
	for (int col = 0; col < 48; col++)
		for (int row = 0; row < 12; row++) {
			Point pos(row * 10, col * 10), center(5, 5), tr(0, 120);
			Scalar color = img.at<Vec3b>(tr + pos + center);//atȡ����(i,j)λ�õ���ͨ������ֵ
			if (color[0] > 180 && color[1] > 180 && color[2] > 180) {//�����˰�ɫ�����أ���ʾ���ܽ�����
				endFlag = 1;
				goto endjudge;//����У��
			}
			Help_Decode(color, code);
		}
endjudge:
	//��codeΪ��ʱ��ֱ�ӷ���
	if (code.empty())  return code;

	//CRCУ�鷢�ִ���ʱ��������Ϊ0��
	else if (CalculateCRC(code, CRC) != string(CRC.size(), '0')) {
		for (int i = 0; i < (code.size() - CRC.size()) / 8; i++) {
			verify << (unsigned char)0;//��verify��0
		}
		code = code.substr(0, code.size() - CRC.size());
		if (endFlag == true)  code.append(string(32, '0'));//������ʱ��code��󲹼�32��0
		return code;
	}
	//CRCУ����ȷʱ
	else {
		code = code.substr(0, code.size() - CRC.size());
		for (int i = 0; i < code.size() / 8; i++) {
			verify << (unsigned char)255;//��verify��1
		}
		if (endFlag == true) code.append(string(32, '0'));//������ʱ��code��󲹼�32��0
		return code;
	}
}

//��srcImageͼƬת��Ϊ����
string Code_Translate(Mat& srcImage, ofstream& verify) {
	Mat midImage, dstImage;//�м�����ͼ������ͼ��
	string code = "";

	//����ԭʼͼƬ
	midImage = Handle_Img(srcImage);//�򵥳�������ȥ��
	Get_ROI(midImage, srcImage, dstImage);//��ȡͼƬ�еĶ�ά�����dstImage��
	code = Decode(dstImage, verify);//��ͼƬ����
	return code;//����ͼƬ����
}

//������ת��Ϊ���ָ�ʽ
string Code_Text(string codes) {
	string text = "";
	auto it = codes.begin();//it++���Լ������¶�ȡcodes���൱��charָ��
	int num = 0, zero_times = 0;
	while (true) {
		num = 0;
		for (int i = 0; i < 8; i++, it++) {
			num += (*it - '0') * pow(2, 7 - i);//����ʮ������ֵ
		}
		if (num == 0) 	zero_times++; //��¼0���ֵĴ���
		else zero_times = 0;
		if (zero_times == 4) break;//������0000������ѭ��
		text.push_back(unsigned char(num));//��numת��Ϊchar�ַ�������text��
	}
	for (int i = 0; i < 3; i++)  text.pop_back();//��textĩβԪ��ɾ��*3
	return text;
}


/*---------------------------------------------����������---------------=---------------------------------------*/


int main()
{
	//�û�ѡ��
	int choice = 1;
	cout << "Which function do you want to use��Encode��input 1��   Decode��input 2��" << endl;
	cin >> choice;
	while (choice != 1 && choice != 2) {
		cout << "Please input correctly��" << endl;
		cout << "Which function do you want to use��Encode��input 1��   Decode��input 2��" << endl;
		cin >> choice;
	}
	cout << endl;

	//��Ϣ����
	if (choice == 1) {
		char name1[20];
		cout << "Please input your binary code filename (e.g:��1.bin��):" << endl;
		cin >> name1;
		cout << endl;

		//��ȡ�ļ�
		vector<int>out;
		string input_string;
		ifstream fs(name1, ios::binary);//��ȡ��ʵ���࣬�Զ����ƴ��ļ�
		stringstream ss;
		ss << fs.rdbuf();//fs������ָ���������ss�����������
		input_string = ss.str();//������ʱ��string�����inString
		int length_char, length_bit;
		length_char = input_string.size();//�ļ��ַ�������

		length_bit = length_char * 8;//ÿ���ַ���Ӧ8 bits

		//�����Ʊ��벿��
		int img_count;
		int LEN = 8;
		img_count = length_bit / 9184;
		//һ�Ŷ�ά����72^2=5184������4*12*12�񱻶�λ����ռ�ݣ�ʣ��4608��һ����ɫ���ɴ���������
		//һ����4608*2=9216 bits������32 bits�ǲ��ӵ�0����һ�Ŷ�ά����Դ���9184 bits����Ϣ��
		if (length_bit % 9184 != 0) img_count++;//�жϼ���ͼ


		//����ļ������򱨴�
		int qr_max;
		cout << "Please limit the number of characters in the file: (e.g:92000)" << endl;
		cin >> qr_max;
		if (length_char > 92000 || length_char > qr_max) {
			cout << "Your File is too large" << endl;
			return 0;
		}
		cout << endl;

		string data;//�洢�����Ʊ���
		input_string += "\0";//�ַ���������

		for (int i = 0; i < length_char; i++) {
			Binary_Code((unsigned char)input_string[i]);//���ַ�ת��Ϊ������
			for (int j = 0; j < LEN; j++) {//������data��
				if (binary_digit[j] == 1) data.append(1, '1');
				else if (binary_digit[j] == 0) data.append(1, '0');
			}
		}
		data += "\0";//������
		cout << "Finish encode" << endl;
		cout << "Data length= " << data.length() << endl;
		cout << "Char length= " << data.length() * 8 << endl;

		//CRC����+��ά�����
		for (int i = 0; i < img_count - 1; i++) {
			Encode_Crc(data.substr(i * 1148 * 4 * 2, 1148 * 4 * 2), 0);//��һ��������code���Ե�i�ν���CRC����
		}
		Encode_Crc(data.substr((img_count - 1) * 1148 * 4 * 2), 1);//�ڶ�������Ϊ1ʱ��ʾ�Ѿ�����
		cout << "Finish CRC" << endl;
		cout << endl;
		Get_Video();//ͼƬת��Ƶ
		cout << endl << "OK!��" << endl;
		system("pause");//��ͣ����ִ��
	}

	//��Ϣ����
	else {
		vector<Mat> srcImages;//�洢����Ƶ�ж�ȡ��ͼƬ
		char videoname[20], outname[20];
		string codes, text;
		vector<string> code_array;//�ַ������飬�洢����ı���

		//�û���������ļ�·��
		cout << "Please input the name of the video to be decoded (e.g: 1.mp4)" << endl;
		cin >> videoname;
		cout << endl << "Please input the file name where the decoded information will be stored (e.g: 1.bin)" << endl;
		cin >> outname;
		cout << endl << "Please input the name of the file where the inspection information is stored (e.g: v1.bin)" << endl;
		cin >> verifyname;

		string videopath(videoname);//�ַ���videopathȡֵvideoname
		Read_Video(videopath, srcImages);//��ȡ��Ƶ����ͼƬ������srcImages������

		string outfile(outname);//�ַ���outfileȡֵoutname
		ofstream out(outfile, ios::binary);//�����ƴ�outfileд��out
		ofstream verify(verifyname, ios::binary);//�����ƴ�verifynameд��verify

		int i = 0;
		string temp;
		for (i = 0; i < srcImages.size(); i++) {
			temp = Code_Translate(srcImages[i], verify);//��ͼƬת��Ϊ����
			if (temp.empty())  continue;//��һ�ζ�ʱ�����������ͼƬ���գ���ȡ��һ��
			else {
				code_array.push_back(temp);//�������ı���������������
				break;//����ѭ��
			}
		}
		i = i + 3;//��������ͼ
		for (; i < srcImages.size(); i += 3) {
			temp = Code_Translate(srcImages[i], verify);//��ͼƬת��Ϊ����
			if (temp.empty())  continue;
			else code_array.push_back(temp);
		}

		//���������鴢��Ϊ�ַ���
		for (int i = 0; i < code_array.size(); i++) {
			codes.append(code_array[i]);
		}

		codes.append(string(32, '0'));//��codes�󲹼�32��0
		text = Code_Text(codes);//������ת��Ϊ�ı�
		out << text;//����out�ļ���
		cout << endl << "OK!��" << endl << endl;//�������
		system("pause");//��ͣ����ִ��
		return 0;
	}
}
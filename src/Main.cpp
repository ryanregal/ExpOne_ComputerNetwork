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

int binary_digit[8] = { 0 };//存储字符的二进制编码
vector<Mat>img;//储存图片
string verifyname;//CRC中用于验证的字符串

/*---------------------------------------------编码部分---------------=---------------------------------------*/

/*-------CRC校验位的计算-------*/

//CRC为除数，原始数据data为被除数，模二除法
string CalculateCRC(string data, string& CRC) {
	data.append(string(CRC.size(), '0'));//在原始数据data后面加上校验码位数（k）个0
	//模2除法
	for (int i = 0; i < data.size() - CRC.size(); i++) {
		//如果是0可以跳过，0与其他数异或还是其他数
		if (data[i] == '1') {
			for (int j = 0; j < CRC.size(); j++) {
				//异或运算，相同0，不同1
				(data[i + j] == CRC[j]) ? data[i + j] = '0' : data[i + j] = '1';
			}
		}
	}
	return data.substr(data.size() - CRC.size(), CRC.size());//返回余数(后k位)
}

//读取待测文件字符串，加入crc校验码，用来校验数据传输
void Encode_Crc(string code, int flag) {
	string CRC = "10000010011000001000111011011011";
	code.append(CalculateCRC(code, CRC));//向code中加入计算得到的余数
	QR_Draw(code, flag);//绘制二维码
}


/*-------绘图部分-------*/

//Position Detection Pattern（定位矩形）绘制，用黑白黑3个矩形框叠加
void Draw_Block(Mat img, Point tr) {
	rectangle(img, Point(0, 0) + tr, Point(105, 105) + tr, Scalar(0, 0, 0), -1);//最外面的矩形框，RGB（黑色）
	rectangle(img, Point(15, 15) + tr, Point(90, 90) + tr, Scalar(255, 255, 255), -1);//中间的矩形框,RGB（白色）
	rectangle(img, Point(30, 30) + tr, Point(75, 75) + tr, Scalar(0, 0, 0), -1);//最里面的矩形框,RGB（黑色）
}

//确定像素颜色
void Help_Draw(string code, int& R, int& G, int& B, int& key) {
	if (code[key] == '0' && code[key + 1] == '0') { R = 255; G = 255; B = 0; }//黄色:00
	if (code[key] == '0' && code[key + 1] == '1') { R = 255; G = 0; B = 0; }//红色:01
	if (code[key] == '1' && code[key + 1] == '0') { R = 0; G = 255; B = 0; }//绿色:10
	if (code[key] == '1' && code[key + 1] == '1') { R = 0; G = 0; B = 255; }//蓝色:11
	key = key + 2;//跳两个比特，继续往下读取code数组
}

//QR二维码图绘制
void QR_Draw(string code, int flag) {
	int key = 0;//code数组的编码序号
	int R = 255, G = 255, B = 255;//存放三通道的值
	Mat src(780, 780, CV_8UC3, Scalar(255, 255, 255));//像素点矩阵src（长宽，类型，颜色）
	Point recSize(10, 10);//像素大小

	//Position Detection Pattern（定位矩形）绘制
	Draw_Block(src, Point(0 + 30, 0 + 30));//左上
	Draw_Block(src, Point(615 + 30, 0 + 30));//右上
	Draw_Block(src, Point(0 + 30, 615 + 30));//左下
	Draw_Block(src, Point(615 + 30, 615 + 30));//右下

	//中间部分，一列一列涂
	for (int col = 0; col < 72; col++) {//中间一共72列
		for (int row = 0; row < 48; row++) {//中间一共48行
			Point pos(row * 10 + 120 + 30, col * 10 + 30);//确定像素位置,120是因为定位矩形，30是二维码外面部分
			Help_Draw(code, R, G, B, key);//计算像素颜色
			rectangle(src, pos, pos + recSize, Scalar(B, G, R), -1);//绘制像素
			//当图像是最后一张时，将src存入img中
			if (flag == 1 && key >= code.size()) {
				//当图像是最后一张时，跳到Last_Draw处
				if (flag == 1 && key >= code.size())  goto Last_Draw;
			}
		}
	}

	//最下面的部分，一列一列涂
	for (int col = 0; col < 48; col++) {//除了定位矩形一共48列
		for (int row = 48; row < 60; row++) {//一共12行
			Point pos(row * 10 + 120 + 30, col * 10 + 120 + 30);//只能从120开始画，前面是定位的方格
			Help_Draw(code, R, G, B, key);//计算像素颜色
			rectangle(src, pos, pos + recSize, Scalar(B, G, R), -1);//绘制像素
			//当图像是最后一张时，将src存入img中
			if (flag == 1 && key >= code.size()) {
				//当图像是最后一张时，跳到Last_Draw处
				if (flag == 1 && key >= code.size())  goto Last_Draw;
			}
		}
	}

	//最上面的部分，一列一列涂
	for (int col = 0; col < 48; col++) {
		for (int row = 0; row < 12; row++) {
			Point pos(row * 10 + 30, col * 10 + 120 + 30);//只能从120开始画，前面是定位的方格
			Help_Draw(code, R, G, B, key);//计算像素颜色
			rectangle(src, pos, pos + recSize, Scalar(B, G, R), -1);//绘制像素
			//当图像是最后一张时，将src存入img中
			if (flag == 1 && key >= code.size()) {
				//当图像是最后一张时，跳到Last_Draw处
				if (flag == 1 && key >= code.size())  goto Last_Draw;
			}
		}
	}
Last_Draw:
	img.push_back(src);
}

/*-------其他部分-------*/

//生成视频
void Get_Video() {
	char videoname[20];
	cout << "Please input the encoded video file name:（e.g: 1.mp4）" << endl;
	cin >> videoname;
	int frame_rate = 10;//视频帧率默认为10
	cout << "Video frame rate defaults to 10" << endl;
	VideoWriter video(videoname, CAP_ANY, frame_rate, Size(780, 780));//将视频保存成videoname
	for (size_t i = 0; i < img.size(); i++)//i:long long unsigned int
	{
		Mat image = img[i].clone();
		video << image;// 流操作符，把图片传入视频
	}
}

//编码函数，ASCII转2进制，得到的2进制字符存储在binary_digit数组中
void Binary_Code(unsigned char a) {
	for (int j = 7; j >= 0; j--) {
		binary_digit[j] = a % 2;
		a = a / 2;
	}
}


/*---------------------------------------------解码部分---------------=---------------------------------------*/

//读取视频
void Read_Video(string videopath, vector<Mat>& srcImages) {
	VideoCapture capture(videopath);//使用cv2库函数读取视频,储存在srcImages中
	Mat frame;

	while (1) {//读取视频帧
		capture >> frame;
		if (frame.empty()) break;//停止读取
		else {
			resize(frame, frame, Size(720, 720), 0, 0, INTER_NEAREST);//调整大小（使用最近邻插值）
			srcImages.push_back(frame.clone());//将该帧克隆到srcImages中
			frame.release();//释放视频流
		}
	}
}

/*----------------图像处理--------------------*/

//简单图像处理（初步噪音去除）
Mat Handle_Img(Mat& srcImg) {
	Mat midImg = srcImg.clone();
	cvtColor(midImg, midImg, CV_RGB2GRAY);//彩色转灰度
	for (int i = 0; i < 5; i++) {
		GaussianBlur(midImg, midImg, Size(3, 3), 0);//高斯滤波，平滑图像，减噪
	}
	threshold(midImg, midImg, 100, 255, THRESH_BINARY); //二值化（阈值）
	return midImg;
}

//将四个定位矩形按顺序排列
//缺点：限制手机只能正着拍摄视频
//改进：设置三个定位点，利用三角形进行定位，同时可以判断方向
Point2f* Sort_Corner(vector<Point2f>& mc) {
	Point2f* p = new Point2f[4];//存储排序好的矩形中心点
	float sum[4];
	for (int i = 0; i < mc.size(); i++) {
		sum[i] = mc[i].x + mc[i].y;//sum数组储存x+y
	}
	int bottomright = max_element(sum, sum + 4) - sum;//右下最大，减sum得到编码
	int topleft = min_element(sum, sum + 4) - sum;//左上最小，减sum得到编码
	int topright = 0, bottomleft = 0;
	//确定左下和右上
	for (int i = 0; i < mc.size(); i++) {
		if (i == bottomright || i == topleft)  continue;//已经确定了，可以跳过
		if (mc[i].x > mc[i].y)
			topright = i;//右上的x>y
		if (mc[i].x < mc[i].y)
			bottomleft = i;//左下的y>x
	}
	p[0] = mc[topleft];//左上
	p[1] = mc[topright];//右上
	p[2] = mc[bottomleft];//左下
	p[3] = mc[bottomright];//右下
	return p;
}

//计算矩形的质心
vector<Point2f> Get_Center(vector<vector<Point>>& corner) {
	vector<Moments> mu(corner.size());//建立一个vector，元素个数为corner.size()个
	//计算每个轮廓的所有矩
	for (int i = 0; i < corner.size(); i++) {
		mu[i] = moments(corner[i], false);//opencv函数，获得轮廓的所有最高达三阶所有矩
		//默认就是false，true是要再次二值化
	}
	//计算轮廓的质心
	vector<Point2f> mc(corner.size());//存储质心的矩阵
	for (int i = 0; i < corner.size(); i++) {
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		// 质心的 X,Y 坐标：(m10/m00, m01/m00),m10 etc是moments内部成员
	}
	return mc;//返回质心
}

//找到定位矩形（有三个轮廓）
vector<vector<Point>> Find_Corner(Mat& midImg) {
	vector<vector<Point>> contours, corner;//contours储存所有轮廓，corner储存定位轮廓
	vector<Vec4i> hierarchy;
	//opencv函数，存储所有的矩形，层级树存储，默认从最小检测开始
	findContours(midImg, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

	int index = -1, mark = 0;
	for (int i = 0; i < contours.size(); i++)//找到四个定位矩形的轮廓放进corner
	{
		if (contourArea(contours[i]) > 50000) continue;//计算轮廓下的面积，大于50000则不是，退出
		//-1表示没有对应的父轮廓，即是外面的轮廓
		if (hierarchy[i][2] != -1 && mark == 0) {//第一次找到最里轮廓
			index = i;//定位
			mark++;
		}
		else if (hierarchy[i][2] != -1) {//找到次里轮廓
			mark++;
		}
		else if (hierarchy[i][2] == -1) {//找到外面轮廓
			mark = 0;//mark清0
			index = -1;//index回置
		}
		//当找到2个定位点信息，此时定位矩阵确定
		if (mark >= 2) {
			corner.push_back(contours[index]);//将定位矩形存入corner中
			mark = 0;//ic清0
			index = -1;//index回置
		}
	}
	return corner;
}

//提取图片区域
void Get_ROI(Mat& midImg, Mat& srcImg, Mat& dstImg) {
	vector<vector<Point>> corner = Find_Corner(midImg); //找到二维码四个定位
	if (corner.size() == NULL || int(corner.size()) != 4)  return; //corner提取出错，则返回
	vector<Point2f> centerpoint(corner.size()); //储存四个矩阵的质心，2f代表二维浮点
	centerpoint = Get_Center(corner); //得到矩形的质心
	Point2f* P1;
	Point2f P2[4];//储存透视变换后的点位
	P1 = Sort_Corner(centerpoint); //四个矩形按照左上，右上，左下，右下排序
	P2[0] = Point2f(52, 52); //对应透视变换后的点位
	P2[1] = Point2f(668, 52);
	P2[2] = Point2f(52, 668);
	P2[3] = Point2f(668, 668);
	//计算变换矩阵进行透视变换
	Mat matrix = getPerspectiveTransform(P1, P2);
	//将透视变换后的图片储存在dstImg中
	warpPerspective(srcImg, dstImg, matrix, srcImg.size(), 1, 0, Scalar(255, 255, 255));
}


/*-----------------图片翻译------------------*/

//辅助Decode（），将像素颜色信息译回代码
//color：{B,G,R}
void Help_Decode(Scalar& color, string& code) {
	if (color[1] > 100 && color[2]>100 && color[0]<1.5*color[1]&& color[0] < 1.5 * color[2]) {
		code += "00";//黄色	
	}
	else if (color[0] < color[2] / 1.5 && color[1] < color[2] / 1.5) code += "01";//红色
	else if (color[0] < color[1] / 1.5 && color[2] < color[1] / 1.5) code += "10";//绿色
	else if (color[1] < color[0] / 1.5 && color[2] < color[0] / 1.5) code += "11";//蓝色
}

//解码的主要函数
string Decode(Mat& img, ofstream& verify) {
	string code;
	string CRC = "10000010011000001000111011011011";
	int endFlag = 0;//表示文件是否结束

	//解码
	//中间的部分
	for (int col = 0; col < 72; col++) {
		for (int row = 0; row < 48; row++) {
			Point pos(row * 10, col * 10), center(5, 5), tr(120, 0);
			Scalar color = img.at<Vec3b>(tr + pos + center);//at取出在(i,j)位置的三通道像素值
			if (color[0] > 180 && color[1] > 180 && color[2] > 180) {//遇到了白色的像素，表示可能结束了
				endFlag = 1;
				goto endjudge;//进行校验
			}
			Help_Decode(color, code);
		}
	}
	//最下面部分
	for (int col = 0; col < 48; col++) {
		for (int row = 48; row < 60; row++) {
			Point pos(row * 10, col * 10), center(5, 5), tr(120, 120);
			Scalar color = img.at<Vec3b>(tr + pos + center);//at取出在(i,j)位置的三通道像素值
			if (color[0] > 180 && color[1] > 180 && color[2] > 180) {//遇到了白色的像素，表示可能结束了
				endFlag = 1;
				goto endjudge;//进行校验
			}
			Help_Decode(color, code);
		}
	}
	//最上面部分
	for (int col = 0; col < 48; col++)
		for (int row = 0; row < 12; row++) {
			Point pos(row * 10, col * 10), center(5, 5), tr(0, 120);
			Scalar color = img.at<Vec3b>(tr + pos + center);//at取出在(i,j)位置的三通道像素值
			if (color[0] > 180 && color[1] > 180 && color[2] > 180) {//遇到了白色的像素，表示可能结束了
				endFlag = 1;
				goto endjudge;//进行校验
			}
			Help_Decode(color, code);
		}
endjudge:
	//当code为空时，直接返回
	if (code.empty())  return code;

	//CRC校验发现错误时（余数不为0）
	else if (CalculateCRC(code, CRC) != string(CRC.size(), '0')) {
		for (int i = 0; i < (code.size() - CRC.size()) / 8; i++) {
			verify << (unsigned char)0;//向verify补0
		}
		code = code.substr(0, code.size() - CRC.size());
		if (endFlag == true)  code.append(string(32, '0'));//当结束时，code最后补加32个0
		return code;
	}
	//CRC校验正确时
	else {
		code = code.substr(0, code.size() - CRC.size());
		for (int i = 0; i < code.size() / 8; i++) {
			verify << (unsigned char)255;//向verify补1
		}
		if (endFlag == true) code.append(string(32, '0'));//当结束时，code最后补加32个0
		return code;
	}
}

//将srcImage图片转换为编码
string Code_Translate(Mat& srcImage, ofstream& verify) {
	Mat midImage, dstImage;//中间生成图像，最终图像
	string code = "";

	//处理原始图片
	midImage = Handle_Img(srcImage);//简单初步处理，去噪
	Get_ROI(midImage, srcImage, dstImage);//提取图片中的二维码存入dstImage中
	code = Decode(dstImage, verify);//将图片解码
	return code;//返回图片编码
}

//将编码转换为文字格式
string Code_Text(string codes) {
	string text = "";
	auto it = codes.begin();//it++可以继续往下读取codes，相当于char指针
	int num = 0, zero_times = 0;
	while (true) {
		num = 0;
		for (int i = 0; i < 8; i++, it++) {
			num += (*it - '0') * pow(2, 7 - i);//计算十进制数值
		}
		if (num == 0) 	zero_times++; //记录0出现的次数
		else zero_times = 0;
		if (zero_times == 4) break;//结束码0000，跳出循环
		text.push_back(unsigned char(num));//将num转换为char字符，存入text中
	}
	for (int i = 0; i < 3; i++)  text.pop_back();//将text末尾元素删除*3
	return text;
}


/*---------------------------------------------主函数部分---------------=---------------------------------------*/


int main()
{
	//用户选择
	int choice = 1;
	cout << "Which function do you want to use：Encode（input 1）   Decode（input 2）" << endl;
	cin >> choice;
	while (choice != 1 && choice != 2) {
		cout << "Please input correctly！" << endl;
		cout << "Which function do you want to use：Encode（input 1）   Decode（input 2）" << endl;
		cin >> choice;
	}
	cout << endl;

	//信息编码
	if (choice == 1) {
		char name1[20];
		cout << "Please input your binary code filename (e.g:“1.bin”):" << endl;
		cin >> name1;
		cout << endl;

		//读取文件
		vector<int>out;
		string input_string;
		ifstream fs(name1, ios::binary);//读取的实现类，以二进制打开文件
		stringstream ss;
		ss << fs.rdbuf();//fs流对象指向的内容用ss流对象来输出
		input_string = ss.str();//返回临时的string对象给inString
		int length_char, length_bit;
		length_char = input_string.size();//文件字符串长度

		length_bit = length_char * 8;//每个字符对应8 bits

		//二进制编码部分
		int img_count;
		int LEN = 8;
		img_count = length_bit / 9184;
		//一张二维码是72^2=5184格，其中4*12*12格被定位矩形占据，剩下4608格，一格（颜色）可传递两个码
		//一共是4608*2=9216 bits，其中32 bits是补加的0，故一张二维码可以传递9184 bits的信息。
		if (length_bit % 9184 != 0) img_count++;//判断几张图


		//如果文件过大，则报错。
		int qr_max;
		cout << "Please limit the number of characters in the file: (e.g:92000)" << endl;
		cin >> qr_max;
		if (length_char > 92000 || length_char > qr_max) {
			cout << "Your File is too large" << endl;
			return 0;
		}
		cout << endl;

		string data;//存储二进制编码
		input_string += "\0";//字符串结束符

		for (int i = 0; i < length_char; i++) {
			Binary_Code((unsigned char)input_string[i]);//将字符转换为二进制
			for (int j = 0; j < LEN; j++) {//储存在data中
				if (binary_digit[j] == 1) data.append(1, '1');
				else if (binary_digit[j] == 0) data.append(1, '0');
			}
		}
		data += "\0";//结束符
		cout << "Finish encode" << endl;
		cout << "Data length= " << data.length() << endl;
		cout << "Char length= " << data.length() * 8 << endl;

		//CRC编码+二维码绘制
		for (int i = 0; i < img_count - 1; i++) {
			Encode_Crc(data.substr(i * 1148 * 4 * 2, 1148 * 4 * 2), 0);//第一个参数是code，对第i段进行CRC编码
		}
		Encode_Crc(data.substr((img_count - 1) * 1148 * 4 * 2), 1);//第二个参数为1时表示已经读完
		cout << "Finish CRC" << endl;
		cout << endl;
		Get_Video();//图片转视频
		cout << endl << "OK!！" << endl;
		system("pause");//暂停程序执行
	}

	//信息解码
	else {
		vector<Mat> srcImages;//存储从视频中读取的图片
		char videoname[20], outname[20];
		string codes, text;
		vector<string> code_array;//字符串数组，存储译出的编码

		//用户输入各个文件路径
		cout << "Please input the name of the video to be decoded (e.g: 1.mp4)" << endl;
		cin >> videoname;
		cout << endl << "Please input the file name where the decoded information will be stored (e.g: 1.bin)" << endl;
		cin >> outname;
		cout << endl << "Please input the name of the file where the inspection information is stored (e.g: v1.bin)" << endl;
		cin >> verifyname;

		string videopath(videoname);//字符串videopath取值videoname
		Read_Video(videopath, srcImages);//读取视频，将图片储存在srcImages数组中

		string outfile(outname);//字符串outfile取值outname
		ofstream out(outfile, ios::binary);//二进制打开outfile写入out
		ofstream verify(verifyname, ios::binary);//二进制打开verifyname写入verify

		int i = 0;
		string temp;
		for (i = 0; i < srcImages.size(); i++) {
			temp = Code_Translate(srcImages[i], verify);//将图片转换为代码
			if (temp.empty())  continue;//第一次读时，如果当该张图片读空，读取下一张
			else {
				code_array.push_back(temp);//将读到的编码存入编码数组中
				break;//跳出循环
			}
		}
		i = i + 3;//往后三张图
		for (; i < srcImages.size(); i += 3) {
			temp = Code_Translate(srcImages[i], verify);//将图片转换为代码
			if (temp.empty())  continue;
			else code_array.push_back(temp);
		}

		//将编码数组储存为字符串
		for (int i = 0; i < code_array.size(); i++) {
			codes.append(code_array[i]);
		}

		codes.append(string(32, '0'));//在codes后补加32个0
		text = Code_Text(codes);//将编码转化为文本
		out << text;//存入out文件中
		cout << endl << "OK!！" << endl << endl;//解码完毕
		system("pause");//暂停程序执行
		return 0;
	}
}
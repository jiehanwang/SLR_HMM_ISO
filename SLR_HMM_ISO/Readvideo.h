//#include "PostureRecognition.h"
#include <time.h>
#include <opencv2\opencv.hpp>
#include<iostream>
#include<fstream>
#include "globalDefine.h"

using namespace std;
using namespace cv;

//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct _Vector2i
/// @brief skeleton data transformed data
//////////////////////////////////////////////////////////////////////////
// struct _Vector2i
// {
// 	int x;
// 	int y;
// };

//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct SLR_ST_Skeleton
/// @brief skeleton data  real data
//////////////////////////////////////////////////////////////////////////
// struct _Vector4f
// {
// 	float x;
// 	float y;
// 	float z;
// 	float w;
// };

// struct SLR_ST_Skeleton
// {
// 	_Vector4f _3dPoint[20];    ///< real point
// 	_Vector2i _2dPoint[20];    ///< pix in color image
// }; 

class Readvideo
{
public:
	vector<LONGLONG> vDepthFrame;
	vector<LONGLONG> vColorFrame;
	vector<LONGLONG> vSkeletonFrame;
	vector<SLR_ST_Skeleton> vSkeletonData;		//一个手语词汇的骨架点集合
	vector<Mat> vDepthData;
	vector<IplImage*> vColorData;

	//////////////////////////////////////////////////////////////////////////
	//ushort *depthData;
public:
	void readvideo(string filePath);
	bool readColorFrame(string filename);
	bool readDepthFrame(string filename);
	bool readSkeletonFrame(string filename);
	Mat retrieveColorDepth(Mat depthMat);
	Mat retrieveGrayDepth(Mat depthMat);
	Readvideo(void);
	~Readvideo(void);
	void release(void);
};


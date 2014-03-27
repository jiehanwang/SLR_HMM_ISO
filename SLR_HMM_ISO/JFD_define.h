#ifndef _JFD_DEFINE_H_
#define _JFD_DEFINE_H_

//输入图像格式结构定义
//图象颜色深度，支持位真彩色和位灰度图象
enum BitsPerPixel
{
	Gray8 = 8,			// 8位灰度
	Color24 = 24		//24位真彩
};

/* 输入源参数设置*/
// 输入源参数结构体定义
typedef struct{
	int nInputWidth; // 输入视频的分辨率
	int nInputHeight; 
	BitsPerPixel nBPP;//输入图像的格式
}IMAGEINFORPARAM;


//检测参数结构定义
// 设置深度旋转(Pan)的检测范围，缺省Level_1
// 3 levels
enum PanLevel
{
	PL_1_30_degree = 1,		// Level_1 +/-30 degree
	PL_2_60_degree = 2,		// Level_2 +/-60 degree
	PL_3_90_degree = 3,		// Level_3 +/-90 degree
};

// 设置平面内旋转(Roll)的检测范围，缺省Level_1
// 2 levels
enum RollLevel
{
	RL_1_20_degree = 1,		// Level_1 +/-20 degree
	RL_2_45_degree = 2,		// Level_2 +/-45 degree
};

// 设置俯仰旋转(Tilt)的检测范围，缺省Level_1
// 2 levels
enum TiltLevel
{
	TL_1_20_degree = 1,		// Level_1 +/-20 degree
	TL_2_40_degree = 2,		// Level_2 +/-40 degree
};

/* 检测参数设置*/
// 检测参数结构体定义
typedef struct{
    double nMinFaceScale; // 最小脸人脸比例
	int nMinFaceLength; // 最小检测人脸的宽和高的大小（宽等于高）
	PanLevel nPanLevel; // 3种姿态检测范围
	RollLevel nRollLevel;
	TiltLevel nTiltLevel;
	int nSpeedAccuracyMode; // 检测（速度\精度）模式

	double nTailAccuracy;	//Tail分类器的精度控制
	int nCondNumAccuracy;	//候选框精度控制参数
	double nHeadThresHold;	//Head分类器的阈值
	int nXPace;				//X方向检测步长
	int nYPace;				//Y方向检测步长
}FDPARAM;

//分类器范围
typedef struct {
     PanLevel nPanClassifierRange;	//Pan方向旋转角度
     RollLevel nRollClassifierRange;//Roll方向旋转角度
     TiltLevel nTiltClassifierRange;//Tilt方向旋转角度
}CLASSIFIERRANGEPARAM;


//检测函数输出人脸结构
// 深度旋转(Pan)范围标志
// 5 levels
enum PanRange
{
	PR_LeftFullProfile = 1,
	PR_LeftHalfProfile = 2,
	PR_Frontal = 3,
	PR_RightFullProfile = 4,
	PR_RightHalfProfile = 5,
};

// 平面内旋转(Roll)范围标志
// 3 levels
enum RollRange
{
	RR_Left = 11,
	RR_Frontal = 12,
	RR_Right = 1,
};

// 俯仰旋转(Tilt)范围标志
// 3 levels
enum TiltRange
{
	TR_Up = 1,
	TR_Forward = 2,
	TR_Down = 3,
};

//人脸信息
typedef struct
{
	int nLeft;		// 人脸矩形框左上点X坐标
	int nTop;		// 人脸矩形框左上点Y坐标
	int nWidth;		// 人脸矩形框宽度
	int nHeight;	// 人脸矩形框高度

	PanRange nPan;	//Pan旋转角度
	RollRange nRoll;//Roll旋转角度
	TiltRange nTilt;//Tilt旋转角度

	double lfConfidence;        // 可信度，范围在到+1.0之间
}FACEINFO;

#endif //_JFD_DEFINE_H_

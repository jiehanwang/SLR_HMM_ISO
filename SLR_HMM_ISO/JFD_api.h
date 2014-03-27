#ifndef _JFD_API_H_
#define _JFD_API_H_
#include "JFD_define.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FACEDECTALL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
//  functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
// #define  __declspec(dllexport)
// #ifdef MAKE_API
//  #define  /*extern "C"_declspec(dllexport)*/
// #else
//  #define  /*extern "C"_declspec(dllimport)*/
// #endif

#define MAXFACENUM 250
#define CLASSIFFIERNUM 15

//人脸基本信息
typedef struct{
	int UpX;
	int UpY;
	int DownX;
	int DownY;
	int POSE;
	double Mean;
	double Variance;
}FACEINFO_YSY;//36个字节

class _declspec(dllexport) JFDetector
{
public:
	// 初始化FD资源
	int FDInitialize();
	int SetClassifiersPath( const char *sPath = NULL );//直接输入文件目录，比如C盘根目录输入“c:”
	int GetClassifiersPath(char *sPath);//获取分类器文件目录,如果第一个字符输出' '，表示为当前目录，其它则是实际设置目录
	int SetPanClassifier(PanLevel nPL = PL_1_30_degree ); 
	int GetPanClassifier(PanLevel *pPL);                 
	int SetRollClassifier(RollLevel nRL = RL_1_20_degree ); 
	int GetRollClassifier(RollLevel *pRL);               
	int SetTiltClassifier(TiltLevel nTL = TL_2_40_degree ); 
	int GetTiltClassifier(TiltLevel *pTL);                  

	int SetInputResolusion(int nInputWidth = 640, int nInputHeight = 480);
	int GetInputResolusion(int *pInputWidth , int *pInputHeight);

	int SetInputBPP(BitsPerPixel nBPP = Color24);
	int GetInputBPP(BitsPerPixel *pBPP );

	int SetMinDetectFaceScale(double nMDFScale = 0.10);                       
	int GetMinDetectFaceScale(double *pMDFScale);                            
	int SetMinDetectFaceSize(int nMDFSize = 35);
	int GetMinDetectFaceSize(int *pMDFSize);

	int SetPanDetectDegree(PanLevel nPL = PL_1_30_degree);
	int GetPanDetectDegree(PanLevel *pPL);

	int SetRollDetectDegree(RollLevel nRL = RL_1_20_degree);
	int GetRollDetectDegree(RollLevel *pRL);

	int SetTiltDetectDegree(TiltLevel nTL = TL_2_40_degree);
	int GetTiltDetectDegree(TiltLevel *pTL );

	int SetSpeedAccuracyMode(int nSAM = 4);//参数有效取值范围1-14
	int GetSpeedAccuracyMode(int *pSAM);

	//分类器结构：由Head分类器和Tail分类器构成。Head分类器先运行，Tail分类器随后运行，Head分类器对速度有很大的影响
	int SetAccuracybyTail(double nAT);//通过Tail分类器运行的程度设置精度，取值范围为0到1，1为全部运行
	int GetAccuracybyTail(double *pAT);

	int SetAccuracybyCondNum(int nACN = 1);//通过分类器运行后的候选框数目情况设置精度，取值任意，取值越大，要求候选框越多才能判断为人脸
	int GetAccuracybyCondNum(int *pACN);

	int SetSpeedbyHead(double nSH = 0);//参数取值范围随意，如果取值大，则检测率下降，误检率下降；反之，检测率增加，误检率增加
	int GetSpeedbyHead(double *pSH);

	int SetSpeedbyXPace(int nSXP);//参数取值范围为任意，在图像上X方向每个子窗口进行分类器时的运行步长
	int GetSpeedbyXPace(int *pSXP);

	int SetSpeedbyYPace(int nSYP);//参数取值范围为任意，在图像上Y方向每个子窗口进行分类器时的运行步长
	int GetSpeedbyYPace(int *pSYP);

	int SetInputParam(IMAGEINFORPARAM ImageInfor, FDPARAM FDParam, CLASSIFIERRANGEPARAM ClassifierRange );
	int GetInputParam(IMAGEINFORPARAM *pImageInfor, FDPARAM *pFDParam, CLASSIFIERRANGEPARAM *pClassifierRange);

	int FDFindFaces(const unsigned char * lpImageData, int *pFace);
	int FDGetFacesInfo  (FACEINFO * lpFaces, int nFace);
	int bInit;
	JFDetector();
	~JFDetector();
private:
	// 释放FD资源
	int FDClose ();

	bool beInSetInputResolution;

	//原fdapi.cpp内全局变量
	CLASSIFIERRANGEPARAM CurClassParam;
	FACEINFO Faces[MAXFACENUM];

	IMAGEINFORPARAM CurInputParam;
	FDPARAM CurFDTParam;

	double fImageRatio;

	unsigned char * pGreyData;

	//int 最小尺寸系统内部是24，输出是20
	double m_dnMinFaceLength;

	int LoadClassifier(PanLevel nPL, RollLevel nRL, TiltLevel nTL);
	BOOL FDSetSpeed(const int lfSpeed);
	void SortFaceByConf(FACEINFO * lpFaces, int nFace);

//原来DetectInfor-YSY.CPP全局变量
	int m_nFaceNum;							//输出人脸个数
	FACEINFO_YSY face[MAXFACENUM];					//输出人脸矩形空间

	BOOL bNeedRun[CLASSIFFIERNUM];			//每个pose分类器运行的开关
	float LUBThresH;						//LAB预分类器的阈值设置
	int CasNum[CLASSIFFIERNUM];				//每个pose cascaded分类器的运行深入程度

	double m_fScaleCoeffient;				//图像金字塔缩放系数
	int m_iMIN_VARIANCE;					//候选脸的最小均方差
	float m_fMINFACEOVERLAPNUM;				//去重框时重框最小个数
	double	m_fROI_MARGIN_RADIO;			//去重框时核心区域的范围设定

	#define MAXRUNTEMPNUM 9
	float m_fAccuracy;						//分类器的精确度
	int m_iXPace;							//检测时X方向的步长
	bool bXPaceHalfPixel;
	bool bXRunTemplate[MAXRUNTEMPNUM];
	int m_iXCurTempNum;
	int m_iYPace;							//检测时Y方向的步长
	bool bYPaceHalfPixel;
	bool bYRunTemplate[MAXRUNTEMPNUM];
	int m_iYCurTempNum;

	//检测时缓冲空间
	int DetectedFaceNumG[CLASSIFFIERNUM];
	FACEINFO_YSY * FaceG[CLASSIFFIERNUM];
	unsigned char * pImageG;
	unsigned char * pImageGShrink;
	long *** InteG;
	DWORD *** InteSqurG;
	BYTE *** LBPDataG;
	long *** RectSumG;
	int tempSer[2000];
	int FaceGUnitNum;//每个姿态人脸分配空间的大小

	int m_iTotalImageNumG;
	int m_iWidthOfScaledImage[100];
	int m_iHeightOfScaledImage[100];

	FACEINFO_YSY CulFace[MAXFACENUM];



//原来DetectInfor-YSY.h全局函数声明
	int FDAllocateBuffer(int nInputWidth, int nInputHeight);
	void FDFreeBuffer();
	int GetScaleNum(int iWidth);

	void ConvertID2Pose(int nID, int& nPoseROP, int& nPoseRIP);
	void ConvertPose2ID(int nPoseROP, int nPoseRIP, int& nID);
	void ShrinkImage_YSY(const unsigned char *scrImage, BYTE* destImage, WORD scrW, WORD scrH,WORD destW, WORD destH);
	void ShrinkRGBImage(BYTE *scrImage, BYTE* destImage, WORD scrW, WORD scrH,WORD destW, WORD destH);
	void SetPose(PanLevel nP = PL_2_60_degree, RollLevel nR = RL_1_20_degree, TiltLevel nT = TL_2_40_degree);
	void SetRunLayerNum(double iPara = 0.7);
	void SetXPace(double iPara = 2);
	void SetYPace(double iPara = 2);
	void SetLABLevel(double iPara = 0);
	void SetMinOverlaps(double iPara = 1.9);

	void GetRunLayerNum(double &iPara);
	void GetXPace(double &iPara);
	void GetYPace(double &iPara);
	void GetLABLevel(double &iPara);
	void GetMinOverlaps(double & iPara);

	//********************CAdaboostFaceDetect.h类中出现的函数*****************/
	void FreeClassifierID(int nID);
	int LoadClassifierID(int nID);
	void FDInitialize_YSY();
	void FDClose_YSY();
	void LBP_(long ** inInte, int width, int height, int unitw, int unith, BYTE ** outLBP, int iScale);
	BOOL DetectROPAll(const unsigned char * ima, int w, int h);
	BOOL DetectShinkedImage(const unsigned char * ima, int w, int h, double dShrinkRadio = 1);
	double CaculateVarianceOptimize(long **Integral,DWORD **IntegralSqur,int Measure,int MeasureH,int SqurMeasure, int PosX, int PosY, int Width, int Height,double * Mean);
	void IntegralE(unsigned char * Image, int width, int height, long ** IntegralImage);
	void IntegralSqurE(unsigned char * Image, int width, int height, DWORD ** IntegralImage);


	// 20070202 function to load classifiers from bin-file
	int CreateAcWFS_Gen_BinFile(int nPoseROP, int nPoseRIP);
	//********************CAdaboostFaceDetect.h类中出现的函数*****************/
	int ReadFeatureNum(int nPoseROP, int nPoseRIP, int * FeaNumOfLayer);

	void RemoveOverlap_ROI_Single(FACEINFO_YSY * face, int & FaceNum, int mode = 1);
	void RemoveOverlap_MultiPose_PRI(FACEINFO_YSY * face, int & FaceNum);
	BYTE LargestPRI(FACEINFO_YSY * face1, FACEINFO_YSY * face2);
};

#endif


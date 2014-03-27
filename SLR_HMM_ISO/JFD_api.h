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

//����������Ϣ
typedef struct{
	int UpX;
	int UpY;
	int DownX;
	int DownY;
	int POSE;
	double Mean;
	double Variance;
}FACEINFO_YSY;//36���ֽ�

class _declspec(dllexport) JFDetector
{
public:
	// ��ʼ��FD��Դ
	int FDInitialize();
	int SetClassifiersPath( const char *sPath = NULL );//ֱ�������ļ�Ŀ¼������C�̸�Ŀ¼���롰c:��
	int GetClassifiersPath(char *sPath);//��ȡ�������ļ�Ŀ¼,�����һ���ַ����' '����ʾΪ��ǰĿ¼����������ʵ������Ŀ¼
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

	int SetSpeedAccuracyMode(int nSAM = 4);//������Чȡֵ��Χ1-14
	int GetSpeedAccuracyMode(int *pSAM);

	//�������ṹ����Head��������Tail���������ɡ�Head�����������У�Tail������������У�Head���������ٶ��кܴ��Ӱ��
	int SetAccuracybyTail(double nAT);//ͨ��Tail���������еĳ̶����þ��ȣ�ȡֵ��ΧΪ0��1��1Ϊȫ������
	int GetAccuracybyTail(double *pAT);

	int SetAccuracybyCondNum(int nACN = 1);//ͨ�����������к�ĺ�ѡ����Ŀ������þ��ȣ�ȡֵ���⣬ȡֵԽ��Ҫ���ѡ��Խ������ж�Ϊ����
	int GetAccuracybyCondNum(int *pACN);

	int SetSpeedbyHead(double nSH = 0);//����ȡֵ��Χ���⣬���ȡֵ���������½���������½�����֮����������ӣ����������
	int GetSpeedbyHead(double *pSH);

	int SetSpeedbyXPace(int nSXP);//����ȡֵ��ΧΪ���⣬��ͼ����X����ÿ���Ӵ��ڽ��з�����ʱ�����в���
	int GetSpeedbyXPace(int *pSXP);

	int SetSpeedbyYPace(int nSYP);//����ȡֵ��ΧΪ���⣬��ͼ����Y����ÿ���Ӵ��ڽ��з�����ʱ�����в���
	int GetSpeedbyYPace(int *pSYP);

	int SetInputParam(IMAGEINFORPARAM ImageInfor, FDPARAM FDParam, CLASSIFIERRANGEPARAM ClassifierRange );
	int GetInputParam(IMAGEINFORPARAM *pImageInfor, FDPARAM *pFDParam, CLASSIFIERRANGEPARAM *pClassifierRange);

	int FDFindFaces(const unsigned char * lpImageData, int *pFace);
	int FDGetFacesInfo  (FACEINFO * lpFaces, int nFace);
	int bInit;
	JFDetector();
	~JFDetector();
private:
	// �ͷ�FD��Դ
	int FDClose ();

	bool beInSetInputResolution;

	//ԭfdapi.cpp��ȫ�ֱ���
	CLASSIFIERRANGEPARAM CurClassParam;
	FACEINFO Faces[MAXFACENUM];

	IMAGEINFORPARAM CurInputParam;
	FDPARAM CurFDTParam;

	double fImageRatio;

	unsigned char * pGreyData;

	//int ��С�ߴ�ϵͳ�ڲ���24�������20
	double m_dnMinFaceLength;

	int LoadClassifier(PanLevel nPL, RollLevel nRL, TiltLevel nTL);
	BOOL FDSetSpeed(const int lfSpeed);
	void SortFaceByConf(FACEINFO * lpFaces, int nFace);

//ԭ��DetectInfor-YSY.CPPȫ�ֱ���
	int m_nFaceNum;							//�����������
	FACEINFO_YSY face[MAXFACENUM];					//����������οռ�

	BOOL bNeedRun[CLASSIFFIERNUM];			//ÿ��pose���������еĿ���
	float LUBThresH;						//LABԤ����������ֵ����
	int CasNum[CLASSIFFIERNUM];				//ÿ��pose cascaded����������������̶�

	double m_fScaleCoeffient;				//ͼ�����������ϵ��
	int m_iMIN_VARIANCE;					//��ѡ������С������
	float m_fMINFACEOVERLAPNUM;				//ȥ�ؿ�ʱ�ؿ���С����
	double	m_fROI_MARGIN_RADIO;			//ȥ�ؿ�ʱ��������ķ�Χ�趨

	#define MAXRUNTEMPNUM 9
	float m_fAccuracy;						//�������ľ�ȷ��
	int m_iXPace;							//���ʱX����Ĳ���
	bool bXPaceHalfPixel;
	bool bXRunTemplate[MAXRUNTEMPNUM];
	int m_iXCurTempNum;
	int m_iYPace;							//���ʱY����Ĳ���
	bool bYPaceHalfPixel;
	bool bYRunTemplate[MAXRUNTEMPNUM];
	int m_iYCurTempNum;

	//���ʱ����ռ�
	int DetectedFaceNumG[CLASSIFFIERNUM];
	FACEINFO_YSY * FaceG[CLASSIFFIERNUM];
	unsigned char * pImageG;
	unsigned char * pImageGShrink;
	long *** InteG;
	DWORD *** InteSqurG;
	BYTE *** LBPDataG;
	long *** RectSumG;
	int tempSer[2000];
	int FaceGUnitNum;//ÿ����̬��������ռ�Ĵ�С

	int m_iTotalImageNumG;
	int m_iWidthOfScaledImage[100];
	int m_iHeightOfScaledImage[100];

	FACEINFO_YSY CulFace[MAXFACENUM];



//ԭ��DetectInfor-YSY.hȫ�ֺ�������
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

	//********************CAdaboostFaceDetect.h���г��ֵĺ���*****************/
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
	//********************CAdaboostFaceDetect.h���г��ֵĺ���*****************/
	int ReadFeatureNum(int nPoseROP, int nPoseRIP, int * FeaNumOfLayer);

	void RemoveOverlap_ROI_Single(FACEINFO_YSY * face, int & FaceNum, int mode = 1);
	void RemoveOverlap_MultiPose_PRI(FACEINFO_YSY * face, int & FaceNum);
	BYTE LargestPRI(FACEINFO_YSY * face1, FACEINFO_YSY * face2);
};

#endif


#pragma once
#include "Hmm.h"
#include "Recognition.h"
#include <opencv2\opencv.hpp>
#include "S_FeaExtraction.h"
#include "globalDefine.h"
using namespace std;
using namespace cv;

class IsolateHMM
{
public:
	IsolateHMM(void);
	~IsolateHMM(void);

public:
	CHMM* m_pDhmm_test;
	CRecognition *m_pRecog;
	S_CFeaExtraction myFeaExtraction;
	CHandSegment_HMM handSegmentVideo;

	CString modelPath;

	CvPoint headPoint, lPoint2, rPoint2;
	vector<Posture> vPosture;
	vector<SLR_ST_Skeleton> vSkeleton;
	vector<int> frameSelect;
	bool frameSelected;
	int usefulFrameSize;
	int heightLimit;

	void loadModel(CString path);
	void run(double **feature, int frameNum, int rank[], double score[]);
	void frameSelect_inMatch(int heightLimit, int leftY, int rightY);
	void readIndata(SLR_ST_Skeleton skeletonCurrent, Mat depthCurrent, IplImage* frameCurrent,int framID);
	void recognize(int rank[], double score[]);
	void patchRun(vector<SLR_ST_Skeleton> skeleton, vector<Mat> depth, vector<IplImage*> color, int rank[], double score[]);
	void ReadGallery(CString path);
	static bool comp2(scoreAndIndex dis_1, scoreAndIndex dis_2);
};


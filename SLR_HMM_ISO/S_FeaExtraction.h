#pragma once
#include "HandSegment.h"
#include "globalDefine.h"

#define spDim 10

class S_CFeaExtraction
{
public:
	S_CFeaExtraction(void);
	~S_CFeaExtraction(void);
public:
	//CHandSegment M_handSegmentVideo;
	double **hog_color;//[frameNum][DES_FEA_NUM];
	double **sp_skeleon;//[frameNum][spDim];
	double **feature;//[frameNum][DES_FEA_NUM + spDim];
	int frameNum;
public:
	void postureFeature(vector<Posture> vPosture, CHandSegment handSegmentVideo);
	void release(void);
	void SPFeature(vector<SLR_ST_Skeleton> vSkeleton);
	void PostureSP(void);
};


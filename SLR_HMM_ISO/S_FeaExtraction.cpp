#include "StdAfx.h"
#include "S_FeaExtraction.h"


S_CFeaExtraction::S_CFeaExtraction(void)
{
	frameNum = 0;
}


S_CFeaExtraction::~S_CFeaExtraction(void)
{
}


void S_CFeaExtraction::postureFeature(vector<Posture> vPosture, CHandSegment_HMM handSegmentVideo)
{
	frameNum = vPosture.size();
	hog_color = new double *[frameNum];
	for (int i=0; i<frameNum; i++)
	{
		hog_color[i] = new double [DES_FEA_NUM];
	}

	for(int i=0; i<vPosture.size(); i++)
	{
		Posture posture = vPosture[i];
		if(posture.leftHandImg == NULL || posture.rightHandImg == NULL)
		{
			for(int j=i-1; j>=0; j--)
			{
				Posture tmpPosture = vPosture[j];
				if(posture.leftHandImg == NULL && tmpPosture.leftHandImg != NULL)
				{
					posture.leftHandImg = cvCreateImage(cvGetSize(tmpPosture.leftHandImg),tmpPosture.leftHandImg->depth,tmpPosture.leftHandImg->nChannels);
					cvCopy(tmpPosture.leftHandImg,posture.leftHandImg);
				}
				if(posture.rightHandImg == NULL && tmpPosture.rightHandImg != NULL)
				{
					posture.rightHandImg = cvCreateImage(cvGetSize(tmpPosture.rightHandImg),tmpPosture.rightHandImg->depth,tmpPosture.rightHandImg->nChannels);
					cvCopy(tmpPosture.rightHandImg,posture.rightHandImg);
				}
				if(posture.rightHandImg != NULL && posture.leftHandImg != NULL)
				{
					break;
				}
			}
		}

		
		handSegmentVideo.getHogFeature(posture.leftHandImg, posture.rightHandImg,hog_color[i]);

	}
}


void S_CFeaExtraction::release(void)
{
	for (int i=0; i<frameNum; i++)
	{
		delete[] hog_color[i];
	}
	delete[] hog_color;

	double **hog_color;

	for (int i=0; i<frameNum; i++)
	{
		delete[] sp_skeleon[i];
	}
	delete[] sp_skeleon;

	double **sp_skeleon;

	for (int i=0; i<frameNum; i++)
	{
		delete[] feature[i];
	}
	delete[] feature;
	double **feature;
}


void S_CFeaExtraction::SPFeature(vector<SLR_ST_Skeleton> vSkeleton)
{
	vector<float> Tdata[15];
	int allFrame = vSkeleton.size();

	sp_skeleon = new double *[allFrame];
	for (int i=0; i<allFrame; i++)
	{
		sp_skeleon[i] = new double [spDim];
	}

	for (int i=0; i<allFrame; i++)
	{
		Tdata[0].push_back(vSkeleton[i]._3dPoint[3].x);
		Tdata[1].push_back(vSkeleton[i]._3dPoint[3].y);
		Tdata[2].push_back(vSkeleton[i]._3dPoint[3].z);
		Tdata[3].push_back(vSkeleton[i]._3dPoint[5].x);
		Tdata[4].push_back(vSkeleton[i]._3dPoint[5].y);
		Tdata[5].push_back(vSkeleton[i]._3dPoint[5].z);
		Tdata[6].push_back(vSkeleton[i]._3dPoint[7].x);
		Tdata[7].push_back(vSkeleton[i]._3dPoint[7].y);
		Tdata[8].push_back(vSkeleton[i]._3dPoint[7].z);
		Tdata[9].push_back(vSkeleton[i]._3dPoint[9].x);
		Tdata[10].push_back(vSkeleton[i]._3dPoint[9].y);
		Tdata[11].push_back(vSkeleton[i]._3dPoint[9].z);
		Tdata[12].push_back(vSkeleton[i]._3dPoint[11].x);
		Tdata[13].push_back(vSkeleton[i]._3dPoint[11].y);
		Tdata[14].push_back(vSkeleton[i]._3dPoint[11].z);
	}

	for (int i=0; i<allFrame; i++)
	{
		Vector<float> skP;
		for (int sk=0; sk<5; sk++)
		{
			for (int sk2=sk+1; sk2<5; sk2++)
			{
				float temp = pow((Tdata[sk*3+0][i]-Tdata[sk2*3+0][i]),2) 
					+ pow((Tdata[sk*3+1][i]-Tdata[sk2*3+1][i]),2)
					+ pow((Tdata[sk*3+2][i]-Tdata[sk2*3+2][i]),2);
				skP.push_back(temp);
			}
		}
		float maxSKP = 0.0;
		for (int k=0; k<skP.size(); k++)
		{
			if (maxSKP < skP[k])
			{
				maxSKP = skP[k];
			}
		}
		for (int k=0; k<skP.size(); k++)
		{
			skP[k] /=maxSKP;
			sp_skeleon[i][k] = skP[k];
		}
	}



}


void S_CFeaExtraction::PostureSP(void)
{
	feature = new double *[frameNum];
	for (int i=0; i<frameNum; i++)
	{
		feature[i] = new double [DES_FEA_NUM+spDim];
	}

	for (int i=0; i<frameNum; i++)
	{
		for (int j=0; j<spDim; j++)
		{
			feature[i][j] = sp_skeleon[i][j];
		}
		for (int j=spDim; j<DES_FEA_NUM+spDim; j++)
		{
			feature[i][j] = hog_color[i][j-spDim];
		}
	}
}

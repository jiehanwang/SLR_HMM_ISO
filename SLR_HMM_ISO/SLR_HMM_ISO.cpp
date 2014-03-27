// SLR_HMM_ISO.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "S_Matching.h"
#include "Readvideo.h"
#include "HandSegment.h"
#include "S_FeaExtraction.h"
#include <fstream>
using namespace std;
using namespace cv;

S_CMatching myMatching;
S_CFeaExtraction myFeaExtraction;
ofstream outputFile;

int _tmain(int argc, _TCHAR* argv[])
{
	CString videoFileName;

	for (int signID=0; signID<100; signID++)
	{
		Readvideo myReadVideo;
		int frameSize;
		if (signID < 10)
			videoFileName.Format("D:\\iData\\isolatedWord\\P50\\P50_000%d_1_0_20121002.oni",signID);
		else if (signID < 100)
			videoFileName.Format("D:\\iData\\isolatedWord\\P50\\P50_00%d_1_0_20121002.oni",signID);
		else if (signID < 1000)
			videoFileName.Format("D:\\iData\\isolatedWord\\P50\\P50_0%d_1_0_20121002.oni",signID);
		
		
		string  s   =   (LPCTSTR)videoFileName;
		myReadVideo.readvideo(s);
		frameSize = myReadVideo.vColorData.size();

		CHandSegment handSegmentVideo;
		handSegmentVideo.init();

		CvPoint headPoint, lPoint2, rPoint2;
		vector<Posture> vPosture;
		vector<SLR_ST_Skeleton> vSkeleton;

		vector<int> frameSelect;
		int heightLimit = min(myReadVideo.vSkeletonData[0]._2dPoint[7].y,
			myReadVideo.vSkeletonData[0]._2dPoint[11].y) - 20;
		int usefulFrameSize = 0;
		for (int i=0; i<frameSize; i++)
		{
			int heightThisLimit = min(myReadVideo.vSkeletonData[i]._2dPoint[7].y,
				myReadVideo.vSkeletonData[i]._2dPoint[11].y);
			if (heightThisLimit < heightLimit)
			{
				frameSelect.push_back(1);
				usefulFrameSize++;
			}
			else
			{
				frameSelect.push_back(0);
			}
		}


		for (int i=0; i<frameSize; i++)
		{
			if (i == 0)
			{
				headPoint.x = myReadVideo.vSkeletonData[i]._2dPoint[3].x;
				headPoint.y = myReadVideo.vSkeletonData[i]._2dPoint[3].y;
				bool bHeadFound = handSegmentVideo.headDetection(
					myReadVideo.vColorData[i],
					myReadVideo.vDepthData[i],
					headPoint);

				if(bHeadFound)
				{
					handSegmentVideo.colorClusterCv(handSegmentVideo.m_pHeadImage,3);
					handSegmentVideo.getFaceNeckRegion(myReadVideo.vColorData[i],myReadVideo.vDepthData[i]);
					handSegmentVideo.copyDepthMat(myReadVideo.vDepthData[i].clone());
				}
			}

			if (frameSelect[i] == 1)
			{
				vSkeleton.push_back(myReadVideo.vSkeletonData[i]);


				Posture posture;

				lPoint2.x = myReadVideo.vSkeletonData[i]._2dPoint[7].x;
				lPoint2.y = myReadVideo.vSkeletonData[i]._2dPoint[7].y;

				rPoint2.x = myReadVideo.vSkeletonData[i]._2dPoint[11].x;
				rPoint2.y = myReadVideo.vSkeletonData[i]._2dPoint[11].y;

				CvRect leftHand;
				CvRect rightHand;

				handSegmentVideo.kickHandsAll(myReadVideo.vColorData[i],myReadVideo.vDepthData[i]
				,lPoint2,rPoint2,posture,leftHand,rightHand);

				vPosture.push_back(posture);
			}
		}


		myFeaExtraction.postureFeature(vPosture,handSegmentVideo);
		myFeaExtraction.SPFeature(vSkeleton);
		myFeaExtraction.PostureSP();


		CString modelPath = "..\\model\\HmmData_50.dat";
		myMatching.loadModel(modelPath);
		myMatching.run(myFeaExtraction.feature, myFeaExtraction.frameNum);


		//Release resource.
		myFeaExtraction.release();
		for (int i=0; i<vPosture.size(); i++)
		{
			cvReleaseImage(&vPosture[i].leftHandImg);
			cvReleaseImage(&vPosture[i].rightHandImg);
		}
		vPosture.clear();
		vSkeleton.clear();
		frameSelect.clear();
	}
	

	cout<<"Done!"<<endl;
	getchar();
	return 0;
}


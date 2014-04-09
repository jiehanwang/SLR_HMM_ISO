// SLR_HMM_ISO.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "S_Matching.h"
#include "Readvideo.h"
#include "HandSegment.h"
#include <fstream>
using namespace std;
using namespace cv;

S_CMatching myMatching;
ofstream outputFile;

int _tmain(int argc, _TCHAR* argv[])
{
	CString         videoFileName;
	SLR_ST_Skeleton skeletonCurrent;    //The 3 current data.
	Mat             depthCurrent;
	IplImage        *frameCurrent;
	CvPoint3D32f    headPoint;

		//Read gallery.
	CString modelPath = "..\\model\\HmmData_50.dat";
	myMatching.ReadGallery(modelPath);

	for (int signID=0; signID<100; signID++)
	{
		int rank[5];
		double score[5];
			//Read video.
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

			//Recognize
		myMatching.patchRun(myReadVideo.vSkeletonData, 
			myReadVideo.vDepthData,
			myReadVideo.vColorData,
			rank, score);

			//Show the result
		for (int i=0; i<5; i++)
		{
			cout<<"Top "<<i<<": "<<rank[i]<<" "<<score[i]<<endl;
		}
	}
	cout<<"Done!"<<endl;
	getchar();
	return 0;
}


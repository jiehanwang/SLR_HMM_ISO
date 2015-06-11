// SLR_HMM_ISO.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IsolateHMM.h"
#include "Readvideo.h"
using namespace std;
using namespace cv;

int _tmain(int argc, _TCHAR* argv[])
{
	IsolateHMM myMatching;

	//Read videos.
	bool fileFindFlag;
	CFileFind fileFind;
	CString normFileName;
	normFileName.Format("E:\\isolatedDemoSign4test\\P50\\*.oni");
	fileFindFlag = true;
	fileFindFlag = fileFind.FindFile(normFileName);

	//The loop of testing
	int *rankIndex;
	rankIndex = new int[5];
	double *rankScore;
	rankScore = new double[5];

	while (fileFindFlag)
	{
		fileFindFlag = fileFind.FindNextFile();
		CString videoFilePath = fileFind.GetFilePath();
		CString videoFileName = fileFind.GetFileName();
		CString videoFileClass = videoFileName.Mid(4,4);
		int classNo = _ttoi(videoFileClass);
		cout<<classNo<<endl;

		Readvideo myReadVideo;
		string  s   =   (LPCTSTR)videoFilePath;
		myReadVideo.readvideo(s);
		int frameSize = myReadVideo.vColorData.size();
		cout<<"Total frameSize "<<frameSize<<endl;

			//Recognize
		myMatching.patchRun(myReadVideo.vSkeletonData, 
			myReadVideo.vDepthData,
			myReadVideo.vColorData,
			rankIndex, rankScore);

			//Show the result
		for (int i=0; i<5; i++)
		{
			cout<<"Top "<<i<<": "<<rankIndex[i]<<" "<<rankScore[i]<<endl;
		}
	}

	delete []rankScore;
	delete []rankIndex;
	cout<<"Done!"<<endl;
	getchar();
	return 0;
}


#pragma once
#include "Hmm.h"
#include "Recognition.h"
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

class S_CMatching
{
public:
	S_CMatching(void);
	~S_CMatching(void);

public:
	CHMM* m_pDhmm_test;
	CRecognition *m_pRecog;
	


	void loadModel(CString path);
	void run(double **feature, int frameNum);
};


#include "StdAfx.h"
#include "S_Matching.h"


S_CMatching::S_CMatching(void)
{
	m_pDhmm_test = new CHMM;
	m_pRecog = new CRecognition;

}


S_CMatching::~S_CMatching(void)
{
}


void S_CMatching::loadModel(CString path)
{
	m_pDhmm_test->Init(path);
}


void S_CMatching::run(double **feature, int frameNum)
{
	m_pRecog->GetHmmModel(m_pDhmm_test);
	double* pCadidateProb = new double [m_pRecog->m_pDhmm->m_nTotalHmmWord];
	int nWordNum, nRecogNum;
	m_pRecog->TestOneWordFromMemory(feature,frameNum,pCadidateProb,nWordNum,nRecogNum);


	cout<<m_pRecog->m_pDhmm->m_pHmmWordIndex[nRecogNum]->Word<<endl;
}

// Recog.h: interface for the CRecog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _RECOGNITION_H
#define _RECOGNITION_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//#include "list.h"
#include "Hmm.h"

typedef struct forSort
{
	int index;
	float value;
}forSort;

class CRecognition  
{
public:
	CRecognition();
	virtual ~CRecognition();
	CHMM* m_pDhmm;

public:
	void ContinueTestOneSen(char * FileName, BOOL bNeedCalibrate, char* result);
	//����ʶ��һ������

public:
	void  BatchTestPlus(CString TestFile);

	int  StaticTest(CString Testfile, BOOL bSpeed = TRUE);
	void  BatchTest(CString TestFile, CString dir, int testID);

	void Decode(char *result,int *StateSize, int T, Linklists *head);

    Linklists *ContinuostRecog(CString TestFile,int *T, BOOL bNeedCalibrate);


	void  ReleaseMemory(Linklists *head);
	void EvaluateSentence(CString strOrigSen, CString strRecSen, int& Delete, int& Insert,
										int& Substitue, int& Correct);

public:
	void  TestWithCadidateSet(CString strWord, double** data, int nFrames, short* pCandidateLoc, int nCandidateNum, int bFirst);
	//������һ�������ʻ㼯�Ĵ�
	void TestOneWord(CString filename, char* word, double* CandidateProb, 
		int& nWordNum, int& nRecogNum,  BOOL bPrune = TRUE);
	void TestOneWordFromMemory(double **feature, int frameNum, double* CandidateProb,
		int& nWordNum, int& nRecogNum);
	//����һ���������е�ģ���е�ʶ��
	void GetWord(CString strName, char* word);

	void CalculateFirstState(double* pData, double *p, int nWordID, int nDimension);

	//���ļ��еõ��ô�
	Linklists *InitialList(int *StateSize,int MaxStateSize);
	

	void PreSlectCandidate(Linklists *head,double *data,int *IndexU,
							int *Ivc, double* NewFirstProb,
						  int t,int *ActiveWordList,int *ActiveWordNum,int *StateSize);

	Linklists *CreateLists(Linklists *head, int *IndexU,int Vct,
					   int *ActiveWordList,int ActiveWordNum,
					   int *StateSize,int MaxStateSize,int t,
					   double *x,double* NewFirstProb,int WordSize,int p);

	double StateProb(double *O,double *Cpai,double **Mixture,double **Cov,
		double *cons, int M,int p);
	double N_probility(double *X,double *mu,double *xigamer,double cons,int p);
	void GetTransProb(int row,int col,double& dwProb);


	void GetHmmModel(CHMM* hmm);

	static bool comparison(forSort a,forSort b);
};

#endif 
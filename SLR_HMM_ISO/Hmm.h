// CHMM.h: interface for the CHMM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined _HMM_H_
#define _HMM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HmmStruct.h"
#define qConst 1.0/sqrt(2.0*3.1415926)

class CHMM  
{
public:
	CHMM();
	virtual ~CHMM();

	void TrainPlus(int WordFlag, HMMSeq* Sequence, int* T, int L, int M, int N, int p,int Tmax, char* FileOut, int NodeIdx );
	void DHMMPlus(char* FileIn,char* FileOut,int L,int MaxN, int M);
	void InitAllPlus(int nWord,double*** Mixture,double*** Cov,double *Pi,double **A,double **APre,double **Cpai,int M,int N,int p);
	void CalculateBPlus(double **B, double **Cpai,double **O,double **dwWeight,
					  double ***Mixture,double ***Cov,double ***N_prob,
					  int M,int N,int p,int T);

	double** GetData(FILE *fp,int Tmax,int *tl,BOOL bNeedCalibrate=TRUE);
	double** GetDataFromMemory(double **feature, int frameNum);


public:
	CHMMStruct** m_pHmmWordIndex;
	double** m_ppTransProb;

	BOOL m_bFlagTrain;
	int m_nTotalHmmWord;
	int m_nMaxStateSize;
	int m_nDimension;
	int m_nMixS;
	double m_MinVa[DIMENSION];
	double m_MaxVa[DIMENSION];

	char* m_pMem;
	LONG m_lBuffSize;
	HGLOBAL m_hGlobal;

public:
	double E(int j,int i,double **data,int v,int *Maping,double ***BSW,
				double *Pailog,double **Alog);
	double InWordViterbi(double *Pilog,double **Alog,double **Blog,int T,int N);
	void SegmentIntoWordUnit(int *Maping,double **data,int TFrames,int WordSize,int *T_Seg);



	double CalCons(double *xigamer,int p);
	void Init(CString fileName);

	void ReadEmbedHmmFile(CHMMStruct**& pHmmIndex,CString fileName);
	void ReadHmmFile(CHMMStruct**& pHmmIndex,CString fileName);
	//void GetMinMaxData();
	void ReadTransProb();
	
	int MapEmbedTable(int nNum);
	
	void WriteHmmFile(double* Pai,double** A,double** CPai,double*** Mixture, double*** Cov,int M, int N, int Dimension,CString WaitWord);
	void Train( HMMSeq* Sequence, int* T, int L, int M, int N, int p,int Tmax, char* FileOut, int NodeIdx );
	void  DHMM(char *BaseFile,char *FileOut,int L,int MaxN,int M,BOOL bFlag=TRUE);
	void  GetT(FILE *fp,int *T,int *p);


    void  FileOutput(char *FileName,double *Pi,double **A,double **Cpai,double ***mixture,
					double ***Cov,int M,int N,int p);


    double Caculate(double **A,double **APre,int N);
	void  CaculateA(double **A,double **B,double **af,double **back,double *C,double **A_up,double *temp,int N,int T);
	void  CaculateMX(double **O,double ***Gamer,double ***Mixture,double ***Cov,
					  double ***Tempmean,double ***Tempcov,double **Eita,
					  int M,int N,int T,int p);
	void  CaculateC(double **Cpaitemp,double *Gamatemp,double ***Gamer,
					int M,int N,int T);
	void  semi_con_probility(double **af,double **back,double *C,double **Cpai,
						   double ***Gamer,double ***N_Prob,double** B,int T,int N,int M);
	void  forward_backward(double *Pi,double **A,double **B,int N,int T,
	                   double *C,double **af,double **back); 
	void  CalculateB(double **B, double **Cpai,double **O,
					  double ***Mixture,double ***Cov,double ***N_prob,int M,int N,
					  int p,int T);

	double N_probility(double *X,double *mu,double *xigamer,int p);
	void  Restamate(double **Cpaitemp,double *Gamatemp,double **Atemp,
					 double *Abtemp,double ***Tempmean,double ***Tempcov,
					 double **Eita,double **Cpai,double **A,double ***Mixture, 
					 double ***Cov,int M,int N,int p);
	void  InitZero(double **C_mid,double *sump,double **A_up,double *temp,double **Eita,
					double ***Tempmean,double ***Tempcov,int M,int N,int L,int p);
    void  InitN(HMMSeq *Sequnce, int *T, int L, int MaxN, int *N, int p);
    void  InitMixture(HMMSeq *Sequnce,double  ***Mixture,double ***Cov,int *T,int N,
					int M,int L,int p);
	void  InitAll(double *Pi,double **A,double **APre,double **Cpai,int M,
					int N);
	double cal_prob(double *O,double **Mixture,double **Cov,double *Cpai, double *N_Pro,int M,int p);
 
	void  LBG_Train(double **mu,double **Ov, int  M,int numdata,int p,int iterations,double Up);
	int   winning_cells(double **input_matrix,double **vect,double **diffvect, 
		                int I,int numcv,int ip_dimension);
	int   Centroids(double *centvec,double **data, int *labelvec,
	           int numdata, int dimension,int label);
    double Viterbi(double *Pi,double **A,double **B,int T,int N);//, int*, bool);
	
	
	double SViterbi(double *Pi,double **A,double **B,int T,int N, int* StateSeq, bool btr);
    double Psud_probility(double *X,double *mu,double *xigamer,int p);
	double forward_backwards(double *Pi,double **A,double **B,int N,int T,
	                   double **af); 
    double ViterbiSeg(double *Pi,double **A,double **B,int T,int N, int* T_Seg);
    void  wAutoSegmentBasicGesture(double **data,int TFrames,int Dimen,int K_Seg,int *Seg);
	double Diavation(int j,int i,double **data,int p) ;

	void ConvertSourceHMMToBinary(CString strPath, CString strOutFile, CString signsList);

};

#endif // !defined(AFX_DHMM_H__3A982CC1_AFB1_11D3_A5AE_0060083481CE__INCLUDED_)

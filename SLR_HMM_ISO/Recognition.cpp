// Recog.cpp: implementation of the CRecognition class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "CGRS.h"
#include "MyInclude.h"
#include "Recognition.h"
#include <algorithm>
using namespace std;

//#include "ProgressDialog.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char m_BaseDir[100];

//extern CProgressDialog* m_pDlgRun;

//CHMM* m_pDhmm;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRecognition::CRecognition()
{

}

CRecognition::~CRecognition()
{
}

void CRecognition::GetWord(CString strName, char* word)
{//从文件名中得到一个词
	int nLoc = strName.ReverseFind('\\');
	CString strTemp = strName.Mid(nLoc+1);
	strTemp = strTemp.Left(strTemp.Find('.'));
	strcpy(word, strTemp);
}

int CRecognition::StaticTest(CString TestFile, BOOL bSpeed)
{
	char word[256];
	int nWordNum, nRecogNum;
	
	double* pCadidateProb = new double [m_pDhmm->m_nTotalHmmWord];
	
	GetWord(TestFile, word);

	TestOneWord(TestFile, word, pCadidateProb, nWordNum, nRecogNum, bSpeed);

	delete []pCadidateProb;
	return nRecogNum;
}

void  CRecognition::BatchTestPlus(CString TestFile)
{
	/*CHMMStruct** pHmmOld=new CHMMStruct*[MAXWORDNUM];
	CString fileName="Hmmdat";
	m_pDhmm->ReadHmmFile(pHmmOld,fileName);
	*/

	double ***B;
	double ***StateProb;
	double **data;
	double ***dwWeight;
	FILE *fp1, *fp2, *fp3;
	int nFrames, nDimension, count;
	char word[50], filename[100], buf[10];
	int* StateSeq;
	double *prob;
	int idx,errnum;
	char str[50];

	if (m_pDhmm->m_nTotalHmmWord == 0) {
		AfxMessageBox("Not training yet, return");
		return;
	}
	if ((fp1 = fopen(TestFile, "r")) == NULL) {
		AfxMessageBox("Invalid index test_filename, return");
		return;
	}
	count = 0;
	while (fscanf(fp1, "%s", buf) != EOF)
		count++;
	fseek(fp1, 0L, SEEK_SET);
	sprintf(filename, "%s%s", m_BaseDir, "\\database\\result.txt");  
	if ((fp3 = fopen(filename, "w")) == NULL) {
		AfxMessageBox("Can not create result file, return.");
		fclose(fp1);
		return;
	}
	fprintf(fp3, "%-15s", "EXPECTED");
	fprintf(fp3, "%s\n", "WRONGRESULT");
	errnum = 0;
   	//m_pDlgRun->Create();
	//m_pDlgRun->m_cProgress.SetRange( 0, count );
	//m_pDlgRun->m_cProgress.SetStep( 1 );
	for (int j=0; j<count; j++)
	{
		fscanf(fp1, "%s", word);
		sprintf(filename, "%s%s%s%s", m_BaseDir, "\\database\\test\\", word, ".txt");
		if ((fp2 = fopen(filename, "r")) == NULL) {
            sprintf(str,"%s%s", "Invalid source test_filename, return.",word); 
			AfxMessageBox(str);
			fclose(fp1);
			return;
		}

		fscanf(fp2, "%d%d", &nFrames, &nDimension);
		if (fseek(fp2, 0L, SEEK_SET)) {
			AfxMessageBox("file operation error, return");
			fclose(fp2);
			return;
		}

		data = m_pDhmm->GetData(fp2, nFrames, &nFrames);
		dwWeight=(double***)Alloc3d(MAXWORDNUM,nFrames,DIMENSION,sizeof(double));

		
		fclose(fp2);
        char buffer[128];
		sprintf(buffer,"%d",j);
		if(nFrames==0)
		{
			AfxMessageBox(buffer);
			fclose(fp1);
			fclose(fp3);
			return;
		}
  
		StateProb = (double ***)Alloc3d(nFrames, m_pDhmm->m_nMaxStateSize,m_pDhmm->m_nMixS, sizeof(double));
		B = (double ***)Alloc3d(MAXWORDNUM,m_pDhmm->m_nMaxStateSize, nFrames, sizeof(double));
		
		for(int jj=0; jj<m_pDhmm->m_nMaxStateSize; jj++)  
		{
		   for(int tt=0; tt<nFrames; tt++)  
		   {
				double sum=0.0;
		
				for (int i=0; i<MAXWORDNUM; i++) 
				{
				  /*B[i][jj][tt] = m_pDhmm->cal_prob(data[tt],pHmmOld[i]->Mix[jj],
					  pHmmOld[i]->Cov[jj],pHmmOld[i]->C[jj],
					  StateProb[tt][jj],pHmmOld[i]->MixS,48);*/
				  B[i][jj][tt] = m_pDhmm->cal_prob(data[tt],m_pDhmm->m_pHmmWordIndex[i]->Mix[jj],
					  m_pDhmm->m_pHmmWordIndex[i]->Cov[jj],m_pDhmm->m_pHmmWordIndex[i]->C[jj],
					  StateProb[tt][jj],m_pDhmm->m_pHmmWordIndex[i]->MixS,DIMENSION);
					sum+=B[i][jj][tt];
					dwWeight[i][jj][tt]=B[i][jj][tt];
				}
				for(int i=0;i<MAXWORDNUM;i++)
				{
					dwWeight[i][jj][tt]=MAXWORDNUM*dwWeight[i][jj][tt]/sum;
				}
	
		   }
		}
		
		prob = new double [m_pDhmm->m_nTotalHmmWord];
		StateSeq = new int [nFrames];
		for (int i=0; i<m_pDhmm->m_nTotalHmmWord; i++) 
		{
			/*m_pDhmm->CalculateBPlus(B, m_pDhmm->m_pHmmWordIndex[i]->C, data,dwWeight[i],
								m_pDhmm->m_pHmmWordIndex[i]->Mix,
								m_pDhmm->m_pHmmWordIndex[i]->Cov,
								StateProb,m_pDhmm->m_pHmmWordIndex[i]->MixS,
								m_pDhmm->m_pHmmWordIndex[i]->MixT,
								nDimension, nFrames);
								*/
			for(int j=0;j<m_pDhmm->m_pHmmWordIndex[i]->MixT;j++)
				for(int k=0;k<nFrames;k++)
				{
					B[i][j][k]=B[i][j][k]*dwWeight[i][j][k];
				}
			prob[i] = m_pDhmm->Viterbi(m_pDhmm->m_pHmmWordIndex[i]->Pai, 
									    m_pDhmm->m_pHmmWordIndex[i]->A, B[i],
										nFrames, m_pDhmm->m_pHmmWordIndex[i]->MixT);//,
										//StateSeq, false); 
		}

		double max = (double)-1.e30;
		int CurNo;
		idx=-1;
		for(int i=0; i<m_pDhmm->m_nTotalHmmWord; i++) {
			if (prob[i] > max) {
				max = prob[i];
				idx = i;
			}
			if (strcmp(m_pDhmm->m_pHmmWordIndex[i]->Word,word)==0) {
                CurNo=i;
			}
		}
			
		if (idx==-1||strcmp(m_pDhmm->m_pHmmWordIndex[idx]->Word,word)) {
			errnum ++;
			fprintf(fp3, "%-15s", word);
			fprintf(fp3,"%f",prob[CurNo]);
			fprintf(fp3, "%s", m_pDhmm->m_pHmmWordIndex[idx]->Word);
			fprintf(fp3,"%f\n",max);
		}
	   
		delete []prob;
		//m_pDhmm->FreeData(data,nFrames);
		Free2d((char**)data);

		Free3d((char ***)StateProb);
    	Free3d((char ***)B);
		delete [] StateSeq;
   		//m_pDlgRun->m_cProgress.StepIt();
		Free3d((char***)dwWeight);
		///////////////////////////////
	}
	/////////////////////////
   	//m_pDlgRun->DestroyWindow();
	fprintf(fp3,"%d",errnum);
	fclose(fp1);
	fclose(fp3);

}

void  CRecognition::TestWithCadidateSet(CString strWord, double** data, int nFrames, short* pCandidateLoc, int nCandidateNum, int bFirst)
{
	double **B;
	double ***StateProb;
	FILE *fp1;
	int nDimension;
	char word[50];
	int* StateSeq;
	double *prob;
	int idx,errnum=0;
	nDimension=DIMENSION;

	strcpy(word, strWord);

	if(bFirst==0)
	{
		fp1=Myfopen("database\\result.txt", "w");
		fprintf(fp1, "%-15s", "EXPECTED");
		fprintf(fp1, "%s\n", "WRONGRESULT");

	}
	else
		fp1=Myfopen("database\\result.txt", "a");


	prob = new double [nCandidateNum];
	StateProb = (double ***)Alloc3d(nFrames, m_pDhmm->m_nMaxStateSize,m_pDhmm->m_nMixS, sizeof(double));
	B = (double **)Alloc2d(m_pDhmm->m_nMaxStateSize, nFrames, sizeof(double));
	StateSeq = new int [nFrames];
	double max = (double)-1.e30;
	int CurNo = -1;
	idx=-1;
	for (int i=0; i<nCandidateNum; i++) 
	{
		int Map= pCandidateLoc[i];
		m_pDhmm->CalculateB(B, m_pDhmm->m_pHmmWordIndex[Map]->C, data,
			m_pDhmm->m_pHmmWordIndex[Map]->Mix,
			m_pDhmm->m_pHmmWordIndex[Map]->Cov,
			StateProb,m_pDhmm->m_pHmmWordIndex[Map]->MixS,
			m_pDhmm->m_pHmmWordIndex[Map]->MixT,
			nDimension, nFrames);
		prob[i] = m_pDhmm->Viterbi(m_pDhmm->m_pHmmWordIndex[Map]->Pai, 
			m_pDhmm->m_pHmmWordIndex[Map]->A, B,
			nFrames, m_pDhmm->m_pHmmWordIndex[Map]->MixT);//,
		//StateSeq, false); 
		if (prob[i] > max) {
			max = prob[i];
			idx = i;
		}
		if (strcmp(m_pDhmm->m_pHmmWordIndex[Map]->Word,word)==0) {
			CurNo=i;
		}
	}
	
	
	if (idx==-1||strcmp(m_pDhmm->m_pHmmWordIndex[pCandidateLoc[idx]]->Word,word)) {
		errnum ++;
		fprintf(fp1, "%-15s", word);
		if( CurNo == -1)
			fprintf(fp1, "0.0");
		else
			fprintf(fp1,"%f",prob[CurNo]);

		fprintf(fp1, "%s", m_pDhmm->m_pHmmWordIndex[pCandidateLoc[idx]]->Word);
		fprintf(fp1,"%f\n",max);
	}
	   
	delete []prob;
	//m_pDhmm->FreeData(data,nFrames);
	Free2d((char**)data);
	
	Free3d((char ***)StateProb);
	Free2d((char **)B);
	delete [] StateSeq;
	fclose(fp1);
   		
	/////////////////////////
}
#define XISHU 0 //-0.1 -- 1 //0.1 - 0.25 
void CRecognition::TestOneWord(CString filename, char* word, double* CandidateProb,
							   int& nWordNum, int& nRecogNum, BOOL bPrune)
{
	//nWordNum目前正确的词号
	//nRecogNum 识别的词号
	//CandidateProb保存候选的概率值
	FILE* fp;
	int nFrames;
	int nDimension;
	double** data;

	double **B;
	double ***StateProb;

	int* StateSeq;
	double *prob;
	int idx;
	char str[100];

	if ((fp = fopen(filename, "r")) == NULL)
	{
		sprintf(str,"%s%s", "Invalid source test_filename, return.", word); 
		AfxMessageBox(str);
		fclose(fp);
		return;
	}
	
	fscanf(fp, "%d%d", &nFrames, &nDimension);
	if (fseek(fp, 0L, SEEK_SET)) {
		AfxMessageBox("file operation error, return");
		fclose(fp);
		return;
	}
	data = m_pDhmm->GetData(fp, nFrames, &nFrames,FALSE);
	//data = m_pDhmm->GetData(fp, nFrames, &nFrames,TRUE);
	//FALSE 已经归一了
	fclose(fp);

	if(nFrames==0)
	{
		AfxMessageBox(filename);
		fclose(fp);
		return;
	}
	
	prob = new double [m_pDhmm->m_nTotalHmmWord];
	StateProb = (double ***)Alloc3d(nFrames, m_pDhmm->m_nMaxStateSize,m_pDhmm->m_nMixS, sizeof(double));
	B = (double **)Alloc2d(m_pDhmm->m_nMaxStateSize, nFrames, sizeof(double));
	StateSeq = new int [nFrames];
	double max = (double)-1.e30;
	int CurNo=-1;
	idx=-1;
	///////////////////////////
	//for the prune word
	int* iprob=new int[m_pDhmm->m_nTotalHmmWord];
	memset(iprob, 1, sizeof(int)*m_pDhmm->m_nTotalHmmWord);
	double sumprob=0;
	double maxf=-1e30;
/*
	if( bPrune )
	{
		for (int ii=0; ii<m_pDhmm->m_nTotalHmmWord; ii++) 
		{ 
			CalculateFirstState(data[0], &prob[ii], ii, nDimension); //ii word index, 
			if(prob[ii]>maxf)
				maxf=prob[ii];
			sumprob +=prob[ii];
		}
		sumprob /= m_pDhmm->m_nTotalHmmWord;
		sumprob =sumprob+XISHU*(maxf-sumprob);
		for(int ii=0; ii<m_pDhmm->m_nTotalHmmWord; ii++)
		{ 
			if(prob[ii]>sumprob)
				iprob[ii]=1;
			else
				iprob[ii]=0;
		}
	}
//*/
	///////////////////////////

	memset(CandidateProb, 0, m_pDhmm->m_nTotalHmmWord*sizeof(double));
	for (int i=0; i<m_pDhmm->m_nTotalHmmWord; i++) 
	{
		if (iprob[i])
		{
			m_pDhmm->CalculateB(B, m_pDhmm->m_pHmmWordIndex[i]->C, data,
				m_pDhmm->m_pHmmWordIndex[i]->Mix,
				m_pDhmm->m_pHmmWordIndex[i]->Cov,
				StateProb,m_pDhmm->m_pHmmWordIndex[i]->MixS,
				m_pDhmm->m_pHmmWordIndex[i]->MixT,
				nDimension, nFrames);
			CandidateProb[i] = m_pDhmm->Viterbi(m_pDhmm->m_pHmmWordIndex[i]->Pai, 
				m_pDhmm->m_pHmmWordIndex[i]->A, B,
				nFrames, m_pDhmm->m_pHmmWordIndex[i]->MixT);//,
			
			if (CandidateProb[i] > max) 
			{
				max = CandidateProb[i];
				idx = i;
			}
		
		}

		if (strcmp(m_pDhmm->m_pHmmWordIndex[i]->Word,word)==0) 
		{
			CurNo=i;
		}
	}
	if(CurNo == -1)
	{
		//AfxMessageBox("There is no right CurrNo");  //Modified by Hanjie Wang
		return;
	}
	nWordNum = CurNo;

	nRecogNum = idx;

	delete []prob;
	delete []iprob;

	Free2d((char**)data);
	
	Free3d((char ***)StateProb);
	Free2d((char **)B);
	delete [] StateSeq;
}

void CRecognition::TestOneWordFromMemory(double **feature, int frameNum, double* CandidateProb,
							   int& nWordNum, int& nRecogNum)
{

	int nFrames = frameNum;
	int nDimension = 61;
	//double** data;

	double **B;
	double ***StateProb;

	int* StateSeq;
	double *prob;
	int idx;
	char str[100];

	//data = m_pDhmm->GetDataFromMemory(feature,frameNum);
	//data = m_pDhmm->GetData(fp, nFrames, &nFrames,TRUE);
	//FALSE 已经归一了
	
	prob = new double [m_pDhmm->m_nTotalHmmWord];
	StateProb = (double ***)Alloc3d(nFrames, m_pDhmm->m_nMaxStateSize,m_pDhmm->m_nMixS, sizeof(double));
	B = (double **)Alloc2d(m_pDhmm->m_nMaxStateSize, nFrames, sizeof(double));
	StateSeq = new int [nFrames];
	double max = (double)-1.e30;
	int CurNo=-1;
	idx=-1;
	///////////////////////////
	//for the prune word
	int* iprob=new int[m_pDhmm->m_nTotalHmmWord];
	memset(iprob, 1, sizeof(int)*m_pDhmm->m_nTotalHmmWord);
	double sumprob=0;
	double maxf=-1e30;

	memset(CandidateProb, 0, m_pDhmm->m_nTotalHmmWord*sizeof(double));
	for (int i=0; i<m_pDhmm->m_nTotalHmmWord; i++) 
	{
		if (iprob[i])
		{
			m_pDhmm->CalculateB(B, m_pDhmm->m_pHmmWordIndex[i]->C, feature,  //data, whj
				m_pDhmm->m_pHmmWordIndex[i]->Mix,
				m_pDhmm->m_pHmmWordIndex[i]->Cov,
				StateProb,m_pDhmm->m_pHmmWordIndex[i]->MixS,
				m_pDhmm->m_pHmmWordIndex[i]->MixT,
				nDimension, nFrames);
			CandidateProb[i] = m_pDhmm->Viterbi(m_pDhmm->m_pHmmWordIndex[i]->Pai, 
				m_pDhmm->m_pHmmWordIndex[i]->A, B,
				nFrames, m_pDhmm->m_pHmmWordIndex[i]->MixT);//,
			
			if (CandidateProb[i] > max) 
			{
				max = CandidateProb[i];
				idx = i;
			}
		
		}

	}

	nWordNum = CurNo;

	nRecogNum = idx;

	delete []prob;
	delete []iprob;

	//Free2d((char**)data);
	
	Free3d((char ***)StateProb);
	Free2d((char **)B);
	delete [] StateSeq;
}

void CRecognition::CalculateFirstState(double* pData, double *p, int nWordID, int nDimension)
{
	
//
	int l=0;
	int iw=nWordID;
	*p =  StateProb(pData, m_pDhmm->m_pHmmWordIndex[iw]->C[l],
							 m_pDhmm->m_pHmmWordIndex[iw]->Mix[l],
							 m_pDhmm->m_pHmmWordIndex[iw]->Cov[l],
							 m_pDhmm->m_pHmmWordIndex[iw]->ConS[l],
							 m_pDhmm->m_nMixS,nDimension);

//
/*
    int j=0;

	double* N_prob = new double[m_pDhmm->m_pHmmWordIndex[nWordID]->MixS];

	*p = m_pDhmm->cal_prob(pData,m_pDhmm->m_pHmmWordIndex[nWordID]->Mix[j],
		m_pDhmm->m_pHmmWordIndex[nWordID]->Cov[j],m_pDhmm->m_pHmmWordIndex[nWordID]->C[j],
		N_prob,m_pDhmm->m_pHmmWordIndex[nWordID]->MixS,nDimension);
	delete []N_prob;*/
	
}

void  CRecognition::BatchTest(CString TestFile, CString dir, int testID)
{

	FILE *fp1, *fp3;
	FILE *fp_whj;
	FILE *fp_detail;
	int count;
	char word[50], filename[100];

	int errnum;
	
	if (m_pDhmm->m_nTotalHmmWord == 0)
	{
		AfxMessageBox("Not training yet, return");
		return;
	}
	if ((fp1 = fopen(TestFile, "r")) == NULL) 
	{
		AfxMessageBox("Invalid index test_filename, return");
		return;
	}
	count = 0;
	fscanf(fp1,"%d\n",&count);

	sprintf(filename, "%s%s%d%s", dir, "result_", testID ,".txt");
	if ((fp3 = fopen(filename, "w")) == NULL) {
		AfxMessageBox("Can not create result file, return.");
		fclose(fp1);
		return;
	}
	fprintf(fp3, "%-15s", "EXPECTED");
	fprintf(fp3, "%s\n", "WRONGRESULT");

	sprintf(filename, "%s%s%d%s", dir, "result_detail_", testID ,".txt");
	if ((fp_detail = fopen(filename, "w")) == NULL) {
		AfxMessageBox("Can not create result file, return.");
		fclose(fp_detail);
		return;
	}

	errnum = 0;
   	//m_pDlgRun->Create();
	//m_pDlgRun->m_cProgress.SetRange( 0, count );
	//m_pDlgRun->m_cProgress.SetStep( 1 );

	double* pCadidateProb = new double [m_pDhmm->m_nTotalHmmWord];
	float rank1 = 0.0;
	float rank5 = 0.0;
	float rank10 = 0.0;
	for (int j=0; j<count; j++)
	{
		cout<<"P"<<testID<<" sign ID: "<<j<<endl;
		fscanf(fp1, "%s", word);
		sprintf(filename, "%s%s%d%s%s%s", dir, "test_", testID ,"\\", word, ".txt");
		int nWordNum, nRecogNum;

		TestOneWord(filename, word, pCadidateProb, nWordNum, nRecogNum);

		//fprintf(fp_detail, "%s\t", word);
		for (int k=0; k<m_pDhmm->m_nTotalHmmWord; k++)
		{
			fprintf(fp_detail, "%f\t", pCadidateProb[k]);
		}
		fprintf(fp_detail, "\n");
	
		if(nWordNum != nRecogNum)   //Output the wrong result. 
		{
			errnum ++;
			fprintf(fp3, "%-15s", word);
			fprintf(fp3,"%f",pCadidateProb[nWordNum]);
			fprintf(fp3, " %s ", m_pDhmm->m_pHmmWordIndex[nRecogNum]->Word);
			fprintf(fp3,"%f\n",pCadidateProb[nRecogNum]);

// 			sprintf(filename, "%s%s", dir, "\\database\\temp.csv");
// 			fp_whj = fopen(filename, "w");
// 			for (int i=0; i<m_pDhmm->m_nTotalHmmWord; i++)
// 			{
// 				fprintf(fp_whj,"%d,%f\n",i,pCadidateProb[i]);
// 			}
// 			fclose(fp_whj);
			
		}
	   
   		//m_pDlgRun->m_cProgress.StepIt();
		//////////////////////////////////////////////////////////////////////////
		//Obtain the rank x candidates. 
		forSort *myForSort;
		myForSort = new forSort[m_pDhmm->m_nTotalHmmWord];
		for (int i=0; i<m_pDhmm->m_nTotalHmmWord; i++)
		{
			myForSort[i].index = i;
			myForSort[i].value = pCadidateProb[i];
		}
		sort(myForSort, myForSort+m_pDhmm->m_nTotalHmmWord, comparison);
		for (int i=0; i<10; i++)
		{
			if (myForSort[i].index == nWordNum)
			{
				if (i==0)
				{
					rank1+=1;
				}
				if (i<5)
				{
					rank5 += 1;
				}
				if (i<10)
				{
					rank10 += 1;
				}
			}
		}

		// 		sprintf(filename, "%s%s", m_BaseDir, "\\database\\temp.csv");
		// 		fp_whj = fopen(filename, "w");
		// 		for (int i=0; i<m_pDhmm->m_nTotalHmmWord; i++)
		// 		{
		// 			fprintf(fp_whj,"%d,%f\n",myForSort[i].index, myForSort[i].value);
		// 		}
		// 		fclose(fp_whj);
		delete []myForSort;
	}
	/////////////////////////
	delete []pCadidateProb;
   	//m_pDlgRun->DestroyWindow();
	fprintf(fp3,"%d\n",errnum);
	fprintf(fp3,"Rank 1: %f\n",rank1/count);
	fprintf(fp3,"Rank 5: %f\n",rank5/count);
	fprintf(fp3,"Rank 10: %f\n",rank10/count);

	sprintf(filename, "%s%s", dir, "allResult.txt");
	if ((fp_whj = fopen(filename, "at")) == NULL)
	{
		fp_whj = fopen(filename, "w");
	}
	
	fprintf(fp_whj,"%f\t%f\t%f\n",rank1/count, rank5/count, rank10/count);
	fclose(fp_whj);


	fclose(fp1);
	fclose(fp3);
	fclose(fp_detail);
}

bool CRecognition::comparison(forSort a,forSort b)
{
	return a.value > b.value;
}
//连续手语识别部分
//////////////////////////////////////////////////////////

void CRecognition::ContinueTestOneSen(char * FileName, BOOL bNeedCalibrate, char* result)
{//连续识别一个句子
//输入文件名
//输入文件是否归一化, TRUE 为 需要归一，
//输出结果

	Linklists *head;
	int nFrames=0;
	int *StateSize;

	head = ContinuostRecog(FileName,&nFrames, bNeedCalibrate);

	StateSize=new int[m_pDhmm->m_nTotalHmmWord];

	for(int i=0; i<m_pDhmm->m_nTotalHmmWord; i++)
		StateSize[i] = m_pDhmm->m_pHmmWordIndex[i]->MixT;

    Decode(result, StateSize, nFrames, head);
//	AfxMessageBox("fgl");
 ////////////////////////////////////////////////////////

//从解码中得到的word 除非有错
	
/*	indexword=Word[1];
//	sprintf(buftemp,"%s",m_pDhmm->m_pHmmWordIndex[indexword]->Word);
//	string_cp(bufchar1,buftemp);
	strcpy(bufchar1, m_pDhmm->m_pHmmWordIndex[indexword]->Word);
    //strcpy(strSenBuffer,bufchar1);    
	strSenBuffer = bufchar1;
////////////////////////////////////////////////////////////////////////////

	for(int t=2; t<nFrames; t++) 
	{
		//sprintf(buftemp,"%s",m_pDhmm->m_pHmmWordIndex[Word[t]]->Word);
		//string_cp(bufchar2,buftemp);
		strcpy(bufchar2, m_pDhmm->m_pHmmWordIndex[Word[t]]->Word);
		ish=strcmp(bufchar2,bufchar1);
		if(Word[t] !=indexword && ish)  
		{
			//strcpy(buffer,bufchar1);
			//strcpy(Buffer,bufchar2);
			indexword=Word[t];
			//sprintf(buftemp,"%s",m_pDhmm->m_pHmmWordIndex[indexword]->Word);
			//string_cp(bufchar1,buftemp);
            //strcat(SenBuffer,Buffer); 
			strcpy(bufchar1, m_pDhmm->m_pHmmWordIndex[indexword]->Word);
			strSenBuffer += bufchar2;
		}
	}
    strcpy(result, strSenBuffer);
*/
	delete []StateSize;
	ReleaseMemory(head);
}



Linklists *CRecognition::InitialList(int *StateSize,int MaxStateSize)
{
	int v,WordSize,l;
	Linklists *head,*s1;
	head=NULL;
	s1=new Linklists;
	WordSize=m_pDhmm->m_nTotalHmmWord;
	s1->nWordNum = WordSize;
	s1->Score=new double* [WordSize];
	s1->Psi  =  new int* [WordSize];
	s1->Fi   =  new int* [WordSize];


	for (v=0; v<WordSize; v++) 
	{
		s1->Score[v]= new double [MaxStateSize];
   		s1->Psi[v]  = new int [MaxStateSize];
		s1->Fi[v]   = new int [MaxStateSize];
	}

	for(v=0; v<WordSize; v++) 
	{
		s1->Score[v][0]=0;
		s1->Psi[v][0]=-1;
		s1->Fi[v][0]=-1;
		for(int l=1;l<StateSize[v]; l++) 
		{
		 	 s1->Score[v][l]=(double)-1.e20;
		     s1->Psi[v][l]=-1; 
		     s1->Fi[v][l]=-1;
		}
	}
	s1->BeamThreshold=-1.e30;
	s1->BeamEndThreshold=-1.e30;
	
	s1->next=NULL;
	head=s1;
	return head;
}

Linklists *CRecognition::CreateLists(Linklists *head, int *IndexU,int Vct,
					   int *ActiveWordList,int ActiveWordNum,
					   int *StateSize,int MaxStateSize,
					   int t,  double *x, double* NewFirstProb,int WordSize,int p)
{
//IndexU, Vct分别是前一帧的候选词列表和数目。
//ActiveWordList,int *ActiveWordNum,分别是当前帧的候选词列表和数目
//候选词数

//ActiveWordList是当前帧激活的词集,保存的是词的ID号

	Linklists *s;
	double ScoreIntra,ScoreInter,temp,value,Av,Bv,Bv0,logGram;
	int iv,v,uindex,i,iindex,l,j,iu,icv;
	int iw, iuWord;
	int iPreWordLoc;

	s=new Linklists;
	s->Score=new double* [ActiveWordNum];
	s->Psi  =new int* [ActiveWordNum];
	s->Fi   =new int* [ActiveWordNum];
	for (v=0; v<ActiveWordNum; v++) 
	{
		s->Score[v]= new double [StateSize[v]];
		s->Psi[v]  = new int [StateSize[v]];
		s->Fi[v]   = new int [StateSize[v]];
		for(int l=0; l<StateSize[v]; l++) 
		{
			s->Fi[v][l]   =-1;
			s->Psi[v][l]  =-1;
			s->Score[v][l]=-1.e30;
		}
	}
	s->BeamMax=-1.e30;
	s->BeamEndThreshold=-1.e30;
	s->BeamThreshold=-1.e30;
    s->BeamEndMax=-1.e30;

	s->nWordNum = ActiveWordNum;
	s->pWordList = new int[ActiveWordNum];
	memcpy(s->pWordList, ActiveWordList, sizeof(int)*ActiveWordNum);

	if (t == 0)//first frame
	{
		for( iv =0;iv<ActiveWordNum;iv++)
		{
			iw = ActiveWordList[iv];
			//这个位置映射的词
			Bv=StateProb(x,m_pDhmm->m_pHmmWordIndex[iw]->C[0],
						 m_pDhmm->m_pHmmWordIndex[iw]->Mix[0],
						 m_pDhmm->m_pHmmWordIndex[iw]->Cov[0],
						 m_pDhmm->m_pHmmWordIndex[iw]->ConS[0],
						 m_pDhmm->m_nMixS,p);
			if(Bv<1.e-30)
				Bv=1.e-30;
			
			Bv=log(Bv);

			s->Score[iv][0]=Bv;
			for (j=0;j<StateSize[iw];j++)
			{
    			s->Psi[iv][j]=iv;
	    		s->Fi[iv][j]=0;
			}
		}
		s->next=head;
		head=s;
		return(head);
	}

	for(iv=0; iv<ActiveWordNum; iv++) 
	{//从第0个词开始，到ActiveWordNum结束
		temp=(double)-1.e30;
		iw = ActiveWordList[iv];

		iPreWordLoc = -1;

		for(int i=0; i< head->nWordNum; i++)
		{
			if( head->pWordList[i] == iw)
			{
				iPreWordLoc = i;
				break;
			}
		}
		//可能前面没有这个词 iPreWordLoc = -1

		for(int l=0;l<StateSize[iw]; l++)
		{//每个词的每个状态	
			if(iv < ActiveWordNum - Vct)//这个是新加的词,直接用表查得第一个状态，其它状态赋最小值
			{
				if( l==0 )
					Bv = NewFirstProb[iv];
				else
					Bv = 1.e-30;
			}
			else
			{
				Bv =  StateProb(x, m_pDhmm->m_pHmmWordIndex[iw]->C[l],
							 m_pDhmm->m_pHmmWordIndex[iw]->Mix[l],
							 m_pDhmm->m_pHmmWordIndex[iw]->Cov[l],
							 m_pDhmm->m_pHmmWordIndex[iw]->ConS[l],
							 m_pDhmm->m_nMixS,p);
			}
			//
			if(Bv<1.e-30)
				Bv=1.e-30;
			Bv=log(Bv);

			if(l==0)
				Bv0=Bv;

			//状态内部跳转
			temp=(double)-1.e40;
			if(iPreWordLoc != -1)
			{//前面没有对应的这个词
				//for(int i=l-1;i<l+1; i++)	//l-1,l //if leap
				for(int i=0;i<l+1;i++)
				{
					if(i>=0 && t>=l) 
					{
						if(head->Score[iPreWordLoc][i]>head->BeamThreshold) 
						{
							if(m_pDhmm->m_pHmmWordIndex[iw]->A[i][l]<1.e-30)
								m_pDhmm->m_pHmmWordIndex[iw]->A[i][l]=1.e-30;
							
							Av=log(m_pDhmm->m_pHmmWordIndex[iw]->A[i][l]);
								
							//if (Bv>-20)//
								value=head->Score[iPreWordLoc][i]+Av+Bv;
							//else
							//	value=head->Score[iPreWordLoc][i]-(double)100;
							if(value>temp)  //max
							{
								 temp=value;
								 iindex=i;
							}
						}
					}
				}
			}
			ScoreIntra = temp;
			
			
			/////////////////////
			//词之间的跳转
			temp=(double)-1.e30;
			
			if ((l==0)&&(Vct!=0))//Vct is effcient value number
			{//第一个状态
				/*Av=m_pDhmm->m_pHmmWordIndex[iw]->A[0][l];
				if(Av<1.e-30)
					Av=1.e-30;
				Av=log(Av);*/

				for(icv=0;icv<Vct; icv++) //前面的有效值
				{//从前面跳转过来
					iu = IndexU[icv];//store efficient value
					iuWord = head->pWordList[iu];//前一个词

					if(iuWord!=iw) 
					{
					   	if(head->Score[iu][StateSize[iuWord]-1]>(head->BeamThreshold)
						 &&head->Score[iu][StateSize[iuWord]-1]>-1.e10)
						{
							//if(Bv0 > -40) 	// -20
							//{
								GetTransProb(iu,iv,logGram);
								value = Bv0+logGram+head->Score[iu][StateSize[iuWord]-1];// Av+Bv0//??u
							//}
							//else
							 // value=head->Score[iu][StateSize[iuWord]-1]-(double)100.0;
							
							if(value>temp) 
							{
								temp=value;
								uindex=iu;//前一个词的位置
							}//if 更大
						} // if 符合跳转条件
					}// 不是同一词
				}//所有可能
				ScoreInter=temp; //ScoreInter=temp-30;//词跳转惩罚
			}
			else
				ScoreInter=(double)-1.e30;
		
			//判断是否是词内还是词间跳转

			//if((ScoreIntra>=ScoreInter)&&(ScoreIntra>-1.e10)) 
			if(ScoreIntra>=ScoreInter)
			{//内部跳转
				if(s->BeamMax<ScoreIntra)
				   s->BeamMax=ScoreIntra;
				
				s->Fi[iv][l]   = iindex;//前一个状态
				s->Psi[iv][l]  = iPreWordLoc;//前一个词的索引号
				s->Score[iv][l]=ScoreIntra;
			}
			else if(ScoreInter>-1.e20 )
			{//词之间转移
				s->Fi[iv][l]   = StateSize[head->pWordList[uindex]]-1;//前一个状态
				s->Psi[iv][l]  = uindex; //前一个词的索引号
				s->Score[iv][l]= ScoreInter;
				
				if(s->BeamMax<ScoreInter)
				   s->BeamMax=ScoreInter;	
				
			}
		}//for l
	}//for iv 
	
	s->BeamThreshold = s->BeamMax-12;//30
	s->BeamEndThreshold = s->BeamEndMax-5;//15

	s->next=head;
	return(s);
} 

Linklists *CRecognition::ContinuostRecog(CString TestFile,int *T, BOOL bNeedCalibrate)
{
	double **data;
	FILE *fp;
	int nFrames, nDimension;
	int MaxState;

	if ((fp = fopen(TestFile, "r")) == NULL)
		AfxMessageBox("Invalid test file name, return");

	fscanf(fp, "%d%d", &nFrames, &nDimension);
	*T=nFrames;
	fseek(fp, 0L, SEEK_SET);
	data = m_pDhmm->GetData(fp, nFrames, &nFrames, bNeedCalibrate);
	fclose(fp);
	
	if (m_pDhmm->m_nTotalHmmWord == 0) 
	{
		AfxMessageBox("Not training yet, return");
		return(0);
	}

////////////////////////////////////////////////////////////

	int i;
	int *StateSize,t;
	Linklists *head;
	int *IndexU, *ActiveWordList;
	int ivt, ActiveWordNum;
	double * NewFirstProb;

	nDimension = m_pDhmm->m_nDimension;

	StateSize=new int[m_pDhmm->m_nTotalHmmWord];
	NewFirstProb  =new double [m_pDhmm->m_nTotalHmmWord];
	IndexU = new int [m_pDhmm->m_nTotalHmmWord];
	ActiveWordList = new int [m_pDhmm->m_nTotalHmmWord];

	for(int i=0;i<m_pDhmm->m_nTotalHmmWord;i++)
	{
		StateSize[i]=m_pDhmm->m_pHmmWordIndex[i]->MixT;
		IndexU[i] = i; //初始化
	}
	MaxState = m_pDhmm->m_nMaxStateSize;

	head = InitialList(StateSize,MaxState);

	int* temp = NULL;


	for(t=0; t<nFrames; t++)  
	{//第一次的head 
		PreSlectCandidate(head,data[t],IndexU,&ivt,NewFirstProb,t,
						ActiveWordList, &ActiveWordNum, 
						StateSize);//IndexU store previous value, ivt previous num
		

	    head=CreateLists(head, IndexU, ivt, ActiveWordList, ActiveWordNum, 
			           StateSize,MaxState, t, data[t], NewFirstProb,
						m_pDhmm->m_nTotalHmmWord, nDimension);


/*
	sprintf(bu,"%s%s_%d.txt",m_BaseDir,"\\database\\temp\\res",t);
	if ((fp = fopen(bu, "w")) == NULL)
	{
		AfxMessageBox("Invalid test file name, return");
	    return(head);
	}
	fprintf(fp,"%d\n",NumberActiveWord);
	for (int k=0;k<NumberActiveWord;k++)
	{
		fprintf(fp,"%d  %s\n",k,m_pDhmm->m_pHmmWordIndex[ActiveWordList[k]].Word);
		for (int j=0;j<StateSize[ActiveWordList[k]];j++)
		    fprintf(fp,"%f  %d  %d  ",head->Score[k][j],head->Psi[k][j],head->Fi[k][j]);
        fprintf(fp,"\n");
	}
fclose(fp);
*/
	
	}
//    m_pDhmm->FreeData(data,nFrames);
	Free2d((char**)data);

	delete []StateSize;
	delete []IndexU;
	delete []NewFirstProb;
	delete []ActiveWordList;

	return(head);
}

#define ACTIVE_VALUE 0//0.12
void CRecognition::PreSlectCandidate(Linklists *head,double *data,int *IndexU,
							int *Ivc, double* NewFirstProb,
						  int t,int *ActiveWordList,int *ActiveWordNum,int *StateSize)
{
//ActiveWordList是当前帧激活的词集,保存的是词的ID号

//NewFirstProb保存的是计算出来新加的词的初始状态概率值，个数为ActiveWordNum-Ivc.

//IndexU,保存的是head中词的位置,具有head的帧的候选词集,前一帧保留的。

//进入的原则是：初始状态的概率值大大于一定域值；淘汰的原则是：如果一个词的所有状态均小于域值，该词将从激活表中清除
//阈值的设定

	int v,i;
	int ict,Icvv;
	BOOL bActive[MAXWORDNUM];

	int nWordNum = head->nWordNum;
    ict=0; 
	int btemp[MAXWORDNUM];
	double btemp2[MAXWORDNUM];
	memset(btemp, 0, MAXWORDNUM*sizeof(int));

	memset(bActive, FALSE, sizeof(BOOL)*MAXWORDNUM);

	if (t == 0) //first frame 选候选词
	{
		*Ivc = ict;
		Icvv = 0;
		//计算第一个状态的概率,并保存在数组里

		double sumprob=0;
		double maxf=-1e30;
		for (int ii=0; ii<m_pDhmm->m_nTotalHmmWord; ii++) 
		{ 
			CalculateFirstState(data, &NewFirstProb[ii], ii, DIMENSION); //ii word index, 
			
			if(NewFirstProb[ii]>maxf)
				maxf = NewFirstProb[ii];
			sumprob += NewFirstProb[ii];
		}
		sumprob /= m_pDhmm->m_nTotalHmmWord;
		sumprob =sumprob+XISHU*(maxf-sumprob);
		for(int ii=0; ii<m_pDhmm->m_nTotalHmmWord; ii++)
		{ 
			if(NewFirstProb[ii]>sumprob)
				ActiveWordList[Icvv++] = ii;
		}
		memcpy(btemp, ActiveWordList, sizeof(int)*Icvv);

	}
	else
	{
		//淘汰
		for(v=0; v<nWordNum; v++)
		{
			int nDiscard = 0;
			
			for(int j =0; j< StateSize[head->pWordList[v]]; j++)
			{
				if(head->Score[v][j] < head->BeamThreshold) 
					nDiscard++;
			}
			
			if(nDiscard != StateSize[head->pWordList[v]])
			{
				IndexU[ict] = v;
				ict++;
				bActive[head->pWordList[v]] = TRUE;
			}
			else
			{
				int temp=v;
			}
		}
		*Ivc=ict;

		Icvv = 0;
		for(v=0; v<m_pDhmm->m_nTotalHmmWord; v++)
		{
			if (!bActive[v])//不是激活的词
			{
				CalculateFirstState(data, &NewFirstProb[Icvv], v, DIMENSION);
				//增加前300个
				if(NewFirstProb[Icvv] > ACTIVE_VALUE)//如何设置这个值
				{
					ActiveWordList[Icvv++] = v;
				}
			}

		}	

		memcpy(btemp2, NewFirstProb, sizeof(double)*Icvv);

		memcpy(btemp, ActiveWordList, sizeof(int)*Icvv);

		for(int i=0; i< ict; i++)
		{
			ActiveWordList[Icvv++] = head->pWordList[IndexU[i]];
		}

		memcpy(btemp, ActiveWordList, sizeof(int)*Icvv);

	}
	*ActiveWordNum = Icvv;

}


void CRecognition::Decode(char *result, int *StateSize, int T, Linklists *head)
{//head 是最后一帧，从后往前走
	int v,qt,t,fs;
	double Amax;
	int nWordLoc;
	
	int Word[MAXFRAMENUM];
	memset(Word, 0, sizeof(int)*MAXFRAMENUM);
	
	if(T >= MAXFRAMENUM)
	{
		AfxMessageBox("Max frame is set too small");
		return;
	}

	Linklists *s;

	Amax=-1.e80;
	
	fs=0;
	qt=0;

	if (head==NULL)
		return;

	for(v=0; v<head->nWordNum; v++)
	{
		if(head->Score[v][StateSize[head->pWordList[v]]-1] > Amax) 
		{
			Amax = head->Score[v][StateSize[head->pWordList[v]]-1];
		    qt = v;
			fs = StateSize[head->pWordList[v]]-1;
		}
	}//max

	if(fs==0) 
	{
	   AfxMessageBox("Decoding Error");
	 	 exit(0);
	}

	Word[T] = head->pWordList[qt];//最后一个是词qt
//	fs=StateSize[qt]-1;
	s=head;
	//s=head->next; 
	for(t=T-1; t>0; t--) 
	{
		nWordLoc = s->Psi[qt][fs];

		fs = s->Fi[qt][fs];
		qt = nWordLoc;

		s=s->next;
		Word[t] = s->pWordList[nWordLoc];

		/*	if(fs>2||fs<0)
		{
			CString str;
			str.Format("fs %d t=%d",fs,t);
			AfxMessageBox(str);
		}
		if(qt<0||qt>207)
		{
			CString str;
			str.Format("%d",qt);
			AfxMessageBox(str);
		}*/

	}

///////////////////////////////////////////////////////////////////////////
	CString strSenBuffer;
	int indexword=Word[1];

	strSenBuffer = m_pDhmm->m_pHmmWordIndex[indexword]->Word;
	strSenBuffer += '/';
	for(t=2; t< T; t++) 
	{
		if(Word[t] != indexword )  
		{
			indexword=Word[t];
			strSenBuffer += m_pDhmm->m_pHmmWordIndex[indexword]->Word;
			strSenBuffer += '/';
		}
	}

	CString strWord[75] = {"_1","借帮助","上海市","北京市", "变化2", "承办1", "独立1", "公式2", "合同1", "会议1", "具体1", "模仿1", "模仿2", "入迷1","广泛","力量","人九", "七他", "尽量请",	"主吃","模式一样","具体有","你问号","问号","科技术","要能力","给","交换互相","重要每", "重要各", "人具体","天天气","工作吗","上海市爱","上海市是","需要的","不培养","才大学","能一","吗学","请等","状况识别","请问去","成大家", "下雪吗","会每","人了的","人民才","问想","我们里","有饭","合工作","可以实","能有效","人类等才","错你","了读书","聋人工业","了学", "吗读书", "了大学","与问","和问","里很","你成","人类等了",	"聊天一","聊天每", "市每天","吗权利","能每网","成来","来每个",
	"在每读","在里读"}; 
	CString strCorrWord[75] = {"","应用", "上海","北京","化", "负担","独", "式", "合", "会", "具", "模", "拟", "入","广","努力","人机","其他","尽量使", "主食","模式化","具有","你呢","吗","科技","要努力","赋予", "交互", "重要意义", "重要意义", "人机", "天气","工作了吗","上海市怎么样","上海市现在是","需要社会的","不适应培养","大学","能上","大学","请稍等","表情识别","请你去","谢谢大家", "下雪了吗","会上","人才的","人才","问你想","我们这里","有米饭","合作","可实","可以有效","人类语言","希望你", "大学","哈尔滨工业","大学","大学","大学","和你","和你","我很","你好","人类语言","聊天","聊天","市天","哪些","能上网","好来","来一个",
	"在哪读","在哪读"}; 


	for(int i=0; i<75; i++)
	{

		strSenBuffer.Replace(strWord[i], strCorrWord[i]);
	}

	CString strNextWord[2] = {"位置", "和(与、同)"};
	CString strNextCorrWord[2]={"在","和"};
	for(int i=0; i<2; i++)
	{

		strSenBuffer.Replace(strNextWord[i], strNextCorrWord[i]);
	}

    strcpy(result, strSenBuffer);

}

double CRecognition::N_probility(double *X,double *mu,double *xigamer,double cons,int p)
{   
    int i;
    double temp,r,r0;
    temp=1.0;
	r=0;					
    for(int i=0; i<p; i++) {
	  r0 =(X[i]-mu[i]);
	  r +=r0*r0/xigamer[i];
    }
	temp = temp*cons;
	temp *=exp(-0.5*r);
    return((double)temp);
}

double CRecognition::StateProb(double *O,double *Cpai,double **Mixture,double **Cov,double *cons, int M,int p)
{
	int i;
	double temp,temp1;
	double sum;
	sum = 0.0;
	for(int i=0;i<M;i++)
	{   
	    temp1 = N_probility(O,Mixture[i],Cov[i],cons[i],p);
		temp = Cpai[i]*temp1;
		sum +=temp;
	}
    return(sum);
}

void CRecognition::GetTransProb(int row,int col,double& dwProb)
{
	row=m_pDhmm->MapEmbedTable(row);
	col=m_pDhmm->MapEmbedTable(col);

	dwProb=m_pDhmm->m_ppTransProb[row][col];
	if(dwProb==0.0)
		dwProb=-10;
	else
		dwProb=log(dwProb)-1;
	dwProb = -8;
}

void CRecognition::ReleaseMemory(Linklists *head)
{
	Linklists *s;

	while(head!=NULL)
	{
		s=head->next;
		delete head;
		head=s;
	}
}

void CRecognition::EvaluateSentence(CString strOrigSen, CString strRecSen, int& Delete, int& Insert,
									int& Substitue, int& Correct)
{//假设strOrigSen 和strRecSen均是经过分词的中间加/
	CString strOrigWord[100];
	CString strRecWord[100];
	int strOrigFlag[100];
	int strRecFlag[100];

	memset(strOrigFlag, 0, sizeof(int)*100); 
	memset(strRecFlag, 0, sizeof(int)*100); 

	int nLoc;
	int nNumOrig = 0;
	int nNumRec = 0;
	
	Delete =0;
	Insert = 0;
	Substitue = 0;
	Correct = 0;

	while( (nLoc = strOrigSen.Find('/')) != -1)
	{
		strOrigFlag[nNumOrig] = -1;
		strOrigWord[nNumOrig++] = strOrigSen.Left(nLoc);
		strOrigSen = strOrigSen.Mid(nLoc+1);
	}

	while( (nLoc = strRecSen.Find('/')) != -1)
	{
		strRecFlag[nNumRec] = -1;
		strRecWord[nNumRec++] = strRecSen.Left(nLoc);
		strRecSen = strRecSen.Mid(nLoc+1);
	}
	
	int LocNumOrig = 0;//数组的位置
	int LocNumRec = 0;//数组的位置

	while(LocNumOrig < nNumOrig || LocNumRec < nNumRec)
	{
		if(strOrigWord[LocNumOrig] == strRecWord[LocNumRec])
		{
			Correct++;
			LocNumOrig++;
			LocNumRec++;
		}
		else
		{//大多数不等
			int TmpOrigLoc = LocNumOrig;
			int TmpRecLoc = LocNumRec;
			//向后搜索相等的
			while(TmpRecLoc<nNumRec)
			{
				if(strOrigWord[LocNumOrig] != strRecWord[TmpRecLoc])
					TmpRecLoc++;
				else
				{//
					Correct++;
					Insert = Insert + TmpRecLoc - LocNumRec;
					LocNumOrig++;
					LocNumRec = TmpRecLoc+1;
					goto Loop;//应该是直接跳到下一个循环开始while(LocNumOrig <= nNumOrig || LocNumRec <= nNumRec)

				}
			}
			
			while(TmpOrigLoc<nNumOrig && TmpRecLoc == nNumRec)
			{
				TmpOrigLoc++;

				TmpRecLoc = LocNumRec;
				//向后搜索相等的
				while(TmpRecLoc<nNumRec)
				{
					if(strOrigWord[TmpOrigLoc] != strRecWord[TmpRecLoc])
						TmpRecLoc++;
					else
					{//
						Correct++;
						if(TmpRecLoc - LocNumRec > TmpOrigLoc - LocNumOrig)
						{
							Substitue = Substitue + TmpOrigLoc - LocNumOrig;
							Insert = Insert + TmpRecLoc - LocNumRec - (TmpOrigLoc - LocNumOrig);
						}
						else
						{
							Substitue = Substitue + TmpRecLoc - LocNumRec;
							Delete = Delete + TmpOrigLoc - LocNumOrig - (TmpRecLoc - LocNumRec);
						}
						LocNumOrig = TmpOrigLoc+1;
						LocNumRec = TmpRecLoc+1;
						goto Loop;//应该是直接跳到下一个循环开始while(LocNumOrig <= nNumOrig || LocNumRec <= nNumRec)
						
					}
				}

			} //while(TmpRecLoc == nNumRec)
			//所有的词全部比较完，没有相等的
			if(TmpOrigLoc == nNumOrig)
			{
				if(TmpRecLoc - LocNumRec > TmpOrigLoc - LocNumOrig)
				{
					Substitue = Substitue + TmpOrigLoc - LocNumOrig;
					Insert = Insert + TmpRecLoc - LocNumRec - (TmpOrigLoc - LocNumOrig);
				}
				else
				{
					Substitue = Substitue + TmpRecLoc - LocNumRec;
					Delete = Delete + TmpOrigLoc - LocNumOrig - (TmpRecLoc - LocNumRec);
				}
				LocNumOrig = TmpOrigLoc;
				LocNumRec = TmpRecLoc;
				goto Loop;
			}


		}
		Loop: LocNumOrig = LocNumOrig;
	}

//特殊情况处理,评价不是十分准确
	int nCorrectNum = 0 ;
	for(int i=0; i<nNumOrig; i++)
	{
		for(int j=0; j<nNumRec; j++)
		{
			if(strOrigFlag[i]==-1 && strRecFlag[j]==-1 && strOrigWord[i] == strRecWord[j])
			{
				strOrigFlag[i] = 0;
				strRecFlag[j] = 0;
				nCorrectNum++;
			}

		}
	}

	if(nCorrectNum > Correct)
	{
		Correct = nCorrectNum;
		if(nNumOrig > nNumRec)
		{
			Substitue = nNumRec - nCorrectNum;
			Delete = nNumOrig - nNumRec;
			Insert = 0;
		}
		else
		{
			Substitue = nNumOrig  - nCorrectNum;
			Insert = nNumRec - nNumOrig;
			Delete = 0;
		}
	}
}


void CRecognition::GetHmmModel(CHMM* hmm)
{
	m_pDhmm = hmm;
}

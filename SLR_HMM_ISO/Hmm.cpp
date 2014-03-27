// CHMM.cpp: implementation of the CHMM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "CGRS.h"
#include "MyInclude.h"

#include "HMM.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define rand_val()    ((double) rand())/((double) RAND_MAX)
extern char m_BaseDir[100];


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMM::CHMM()
{
	//m_bFlagTrain = true;   //For training
	m_bFlagTrain = false;    //For test
	//is train or not? manual set
	//正确  表示训练
	m_pHmmWordIndex = NULL;
	m_lBuffSize=0;
	m_pMem=NULL;
	m_ppTransProb=NULL;

	m_nTotalHmmWord = 0;
	m_nDimension=DIMENSION;
	m_nMaxStateSize=0;

// 	if(!m_bFlagTrain)
// 		Init();
}

CHMM::~CHMM()
{
	if(m_pHmmWordIndex!=NULL)
	{
		for(int i=0;i<MAXWORDNUM;i++)
			delete m_pHmmWordIndex[i];
		delete []m_pHmmWordIndex;
	}

	if(m_ppTransProb!=NULL)
		Free2d((char**)m_ppTransProb);

}
void CHMM::Init(CString fileName)
{
	m_pHmmWordIndex=new CHMMStruct*[MAXWORDNUM];
	m_ppTransProb=(double**)Alloc2d(MAXWORDNUM,MAXWORDNUM,sizeof(double));

    //GetMinMaxData();

	//CString fileName="..\\data\\HmmData.dat";
	ReadHmmFile(m_pHmmWordIndex,fileName);
	
	//fileName="database\\HmmDataEmbed.dat";
	//ReadEmbedHmmFile(m_pHmmWordIndex,fileName);

	//ReadTransProb();
}
void CHMM::ReadTransProb()
{
	CFile fileR;
	if(!fileR.Open("database\\transition.dat",CFile::modeRead|CFile::typeBinary))
	{ 
		AfxMessageBox("Cann't open file to read");
		return;
	}	
	int nNum=0;
	fileR.Read(&nNum,sizeof(int));
	int ni,nj;
	float prob;
	for(int i=0;i<nNum;i++)
	{
		fileR.Read(&ni,sizeof(int));
		fileR.Read(&nj,sizeof(int));
		fileR.Read(&prob,sizeof(float));
		m_ppTransProb[ni][nj]=prob;
	}

}

int CHMM::MapEmbedTable(int nNum)
{
	/*if(nNum<208)
		return nNum;
	//98
	//int Table[]={0,2,7,8,13,14,18,21,24,27,31,35,36,39,41,42,45,49,51,53,54,57,58,61,62,63,65,67,69,70,71,73,75,79,85,89,90,91,95,98,99,100,103,105,106,110,111,112,114,117,118,122,125,126,127,131,132,134,136,138,139,141,143,145,150,151,156,157,158,161,164,165,166,167,170,171,172,176,177,178,179,180,183,186,187,192,195,196,197,198,199,200,201,202,203,204,205,206};
	//99
	int Table[]={0,2,7,8,13,14,18,20,21,24,27,31,35,36,39,41,42,45,49,51,53,54,57,58,61,62,63,65,67,69,70,71,73,75,79,85,89,90,91,95,98,99,100,103,105,106,110,111,112,114,117,118,122,125,126,127,131,132,134,136,138,139,141,143,145,150,151,156,157,158,161,164,165,166,167,170,171,172,176,177,178,179,180,183,186,187,192,195,196,197,198,199,200,201,202,203,204,205,206};
	return Table[nNum-208];*/
	return nNum;

}
// void CHMM::GetMinMaxData()
// {
// 	char filename[100];
// 	int t;
// 	FILE *fp; 
// 	float buff;
// 	sprintf(filename, "%s%s",m_BaseDir,"\\database\\test\\MinMax.dat");
// 	if ((fp = fopen(filename, "r")) == NULL) 
// 	{
// 		AfxMessageBox("Need to estimate the range!");
// 		for(t=0;t<DIMENSION;t++)
// 		{
// 			m_MinVa[t] = 1000;
// 			m_MaxVa[t] = -1000;
// 		}
// 	}
// 	else
// 	{
// 		for(t=0;t<DIMENSION;t++)
// 		{
// 			fscanf(fp,"%f",&buff);
// 			m_MinVa[t]=buff;
// 		}
// 		for(t=0;t<DIMENSION;t++)
// 		{
// 			fscanf(fp,"%f",&buff);
// 			m_MaxVa[t]=buff;
// 		}
// 	}	
// 	fclose(fp);
// 
// }

void CHMM::DHMMPlus(char* FileIn,char* FileOut,int L,int MaxN, int M)
{
	int* T;
	int  N; 
	int Tmax, ST, p;
	Seq* Sequence;
	char buffer[128];
	FILE* fp;
 
	T = new int [L];		//Pointer to the time

	if (!(fp = fopen(FileIn, "r")))
	{
		  AfxMessageBox("Invalid training file.");
		  return;
	}
	CString fileName=FileIn;
	CString WaitWord=fileName.Mid(fileName.ReverseFind('\\')+1,fileName.ReverseFind('.')-fileName.ReverseFind('\\')-1);
	int WordFlag;

	for(int l=0; l<L; l++)  
	{
		GetT(fp,&T[l],&p);
		if(T[l]==0)
		{
		sprintf(buffer,"%d",m_nTotalHmmWord);
			AfxMessageBox(buffer);
		 return;
		}
	}
	fclose(fp);

	Tmax=0;
    ST=0;
    for(int l=0; l<L; l++) 
	{
           ST+=T[l];
           if(T[l]>Tmax) 
			   Tmax=T[l];
    }

    Sequence= (Seq *)malloc(L*sizeof(Seq));		//Pointer to the time
	if (!(fp = fopen(FileIn, "r")))
					 exit(1);

	for(int l=0; l<L; l++)  
	{
		Sequence[l].O=GetData(fp,Tmax,&T[l],FALSE);
		Sequence[l].dwWeight=(double**)Alloc2d(T[l],DIMENSION,sizeof(double));
	}
	fclose(fp);
    
	InitN( Sequence, T, L, MaxN, &N, p);

	for(int l=0;l<L;l++)
	{
		double*** StateProb = (double ***)Alloc3d(T[l], m_nMaxStateSize, m_nMixS, sizeof(double));
   
		int j,t;
		for(int j=0; j<3; j++)  
		{
		   for(t=0; t<T[l]; t++)  
		   {
				double sum=0.0;
				double tmp;
				double B;
				for (int i=0; i<MAXWORDNUM; i++) 
				{
				  B = cal_prob(Sequence[l].O[t],m_pHmmWordIndex[i]->Mix[j],
					  m_pHmmWordIndex[i]->Cov[j],m_pHmmWordIndex[i]->C[j],
					  StateProb[t][j],m_pHmmWordIndex[i]->MixS,p);
					if(strcmp(m_pHmmWordIndex[i]->Word,WaitWord.GetBuffer(30))==0)
					{
						tmp=B;
						WordFlag=i;
					}
					WaitWord.ReleaseBuffer();
					sum+=B;
				}
				Sequence[l].dwWeight[j][t]=MAXWORDNUM*tmp/sum;
	
		   }
		}
		Free2d((char**)StateProb);
	}
     
//	m_pTrain->AllocOneHmmModel(m_pTrain->m_nTotalHmmWord-1, M, N,p);

	TrainPlus(WordFlag, Sequence, T, L, M, N, p, Tmax, FileOut, -1 );

	for(int l=0; l<L; l++ )
	{
		Free2d((char**)Sequence[l].O);
		Free2d((char**)Sequence[l].dwWeight);
	}
	free(Sequence);
	delete []T;
}
void CHMM::TrainPlus(int WordFlag, Seq* Sequence, int* T, int L, int M, int N, int p,int Tmax, char* FileOut, int NodeIdx )
{
	int   l, ite;
	double ***Mixture,***Cov; 
	double ***Gamer,**gamer,***N_Prob,*Gamatemp;
	double **Cpai,**Cpaitemp;
    double *Abtemp,**Atemp,**Eita; 
    double **A,**B,*C,*Pi,*LgP,**af,**back;
    double ***Tempmean,***Tempcov,**APre,Eps=1.0;
    const int ITMAX = 20;

	Pi                      = new double [N];         //Initial Probibility  
    LgP                     = new double [L];		   // log P*
    Gamatemp                = new double [N];
    Abtemp                  = new double [N];
	C                       =new double [Tmax];
    
    A                     =(double **)Alloc2d (N, N,sizeof(double));	   //transion matrix
    APre                  =(double **)Alloc2d (N, N,sizeof(double));	   //transion matrix
	Atemp                 =(double **)Alloc2d (N, N,sizeof(double));  
    B                     =(double **)Alloc2d (N,Tmax,sizeof(double));	  
    gamer                 =(double **)Alloc2d (Tmax,N,sizeof(double));     
    Gamer                 =(double ***)Alloc3d (Tmax,N,M,sizeof(double));  
    N_Prob                =(double ***)Alloc3d (Tmax,N,M,sizeof(double));
    Mixture               =(double ***)Alloc3d(N,M,p,sizeof(double));  
    Cov                   =(double ***)Alloc3d(N,M,p,sizeof(double));  
	af                    =(double ** )Alloc2d (Tmax,N,sizeof(double));  
    back                   =(double ** )Alloc2d (Tmax,N,sizeof(double));  
    Eita                  =(double **)Alloc2d (N, M, sizeof(double));	
	Cpai                  =(double **)Alloc2d (N, M,sizeof(double));
    Cpaitemp              =(double **)Alloc2d (N, M,sizeof(double));
    Tempmean              =(double ***)Alloc3d (N,M,p,sizeof(double));
    Tempcov               =(double ***)Alloc3d (N,M,p,sizeof(double));
//Mixture, Cov
//	InitMixture(Sequence,Mixture,Cov,T,N,M,L,p);
	InitAllPlus(WordFlag,Mixture,Cov,Pi,A,APre,Cpai,M,N,p);

	ite=0;
	while(ite<ITMAX)
	{
		 InitZero(Cpaitemp,Gamatemp,Atemp,Abtemp,Eita,Tempmean,Tempcov,M,N,L,p);
 
         for(int l=0; l<L; l++) 
		 {
			 if(ite==0)
			 {
	 	      CalculateBPlus(B,m_pHmmWordIndex[WordFlag]->C,Sequence[l].O,Sequence[l].dwWeight,
				  m_pHmmWordIndex[WordFlag]->Mix,
				  m_pHmmWordIndex[WordFlag]->Cov,N_Prob,
				  m_pHmmWordIndex[WordFlag]->MixS,m_pHmmWordIndex[WordFlag]->MixT,
				  p,T[l]);
			 }
			 else
              CalculateBPlus(B,Cpai,Sequence[l].O,Sequence[l].dwWeight,Mixture,Cov,N_Prob,M,N,p,T[l]);

			  forward_backward(Pi,A,B,N,T[l],C,af,back);
              semi_con_probility(af,back,C,Cpai,Gamer,N_Prob,B,T[l],N,M);
			  CaculateC(Cpaitemp,Gamatemp,Gamer,M, N, T[l]);
		      CaculateA(A,B,af,back,C,Atemp,Abtemp,N,T[l]);
              CaculateMX(Sequence[l].O,Gamer,Mixture,Cov,Tempmean,Tempcov,
				         Eita,M,N,T[l],p);
		 }
		 Restamate(Cpaitemp,Gamatemp,Atemp,Abtemp,Tempmean,Tempcov,Eita,Cpai,A,
			       Mixture,Cov,M,N,p);
		 //if(ite==1)
		   //  Eps=1;
		 //else
		//	Eps=Caculate(A,APre,N);
		// if(Eps<0.000001)
			 //break;
		 ite++;
	}
	
//	m_pTrain->SaveNewHmmCode(Pi, A, Cpai, Mixture,Cov,M,N,p);
	FileOutput(FileOut,Pi,A,Cpai,Mixture,Cov,M,N, p);

	CString fileName=FileOut;
	CString WaitWord=fileName.Mid(fileName.ReverseFind('\\')+1,fileName.ReverseFind('.')-fileName.ReverseFind('\\')-1);
	WriteHmmFile(Pi,A,Cpai,Mixture,Cov,M,N,p,WaitWord);

    delete []Pi;
    delete []LgP;
    delete []Gamatemp;
    delete []Abtemp;
    delete []C;
	Free3d((char***)Mixture);
	Free3d((char***)Cov);
	Free3d((char ***)N_Prob);
	Free2d((char **)Eita);
	Free2d((char **)A);
    Free2d((char **)APre);
    Free2d((char **)Atemp);  
    Free2d((char **)B);	  
    Free2d((char **)gamer);     
    Free3d((char ***)Gamer);  
    Free2d((char **)af);  
    Free2d((char **)back);  
    Free2d((char **)Cpai);
    Free2d((char **)Cpaitemp);
    Free3d((char ***)Tempmean);  
    Free3d((char ***)Tempcov);  
}

void CHMM::DHMM(char* FileIn,char* FileOut,int L,int MaxN, int M, BOOL bFlag)
{//	if(bFlag)//已经归一
	int* T;
	int  N; 
	int Tmax, ST, p;
	Seq* Sequence;
	FILE* fp;
	char buffer[60000]; //Modified by Hanjie Wang

	T = new int [600];		//Pointer to the time

	if (!(fp = fopen(FileIn, "r"))) {
		  AfxMessageBox("Invalid training file.");
		  return;
	}
	int l;
	
	if(L==0)
	{

		l=0;
		while(1)
		{
			T[l]=0;
			GetT(fp,&T[l],&p);
			if(T[l]==0)
				break;
			l++;
		}
		if(l>40)
		{
			L=40;
			sprintf(buffer, "%s %d", FileIn, l);
		//	AfxMessageBox(buffer);
		}
		else
			L=l;// l or 1
		p = 51;
	}
	else
	{
		for(int l=0; l<L; l++)
		{
			GetT(fp,&T[l],&p);
			if(T[l]==0) {
			sprintf(buffer,"%d",m_nTotalHmmWord);
				AfxMessageBox(buffer);
			 return;
			}
		}
	}

	fclose(fp);

	Tmax=0;
    ST=0;
    for(int l=0; l<L; l++) {
           ST+=T[l];
           if(T[l]>Tmax) Tmax=T[l];
    }

    Sequence= (Seq *)malloc(L*sizeof(Seq));		//Pointer to the time
	if (!(fp = fopen(FileIn, "r")))
					 exit(1);

	for(int l=0; l<L; l++)  {
		//bFlag = TRUE; //Modified by Hanjie Wang
		if(bFlag)//已经归一
		{
		  Sequence[l].O=GetData(fp,Tmax,&T[l],FALSE);//fgl 2003.11 bu 已经归一了
		}
		else
		{
		  Sequence[l].O=GetData(fp,Tmax,&T[l],TRUE);//fgl 2003.11
		}
	}
	fclose(fp);
    
	InitN( Sequence, T, L, MaxN, &N, p);

	//m_pTrain->AllocOneHmmModel(m_pTrain->m_nTotalHmmWord-1, M, N,p);

	Train( Sequence, T, L, M, N, p, Tmax, FileOut, -1 );

	for(int l=0; l<L; l++ )
		//FreeData(Sequence[l].O,T[l]);
		Free2d((char**)Sequence[l].O);
	free(Sequence);
	delete []T;
} 

void CHMM::Train( Seq* Sequence, int* T, int L, int M, int N, int p,int Tmax, char* FileOut, int NodeIdx )
{
	int   l, ite;
	double ***Mixture,***Cov; 
	double ***Gamer,**gamer,***N_Prob,*Gamatemp;
	double **Cpai,**Cpaitemp;
    double *Abtemp,**Atemp,**Eita; 
    double **A,**B,*C,*Pi,*LgP,**af,**back;
    double ***Tempmean,***Tempcov,**APre,Eps=1.0;
    const int ITMAX = 50;//250;//50;

	Pi                      = new double [N];         //Initial Probibility  
    LgP                     = new double [L];		   // log P*
    Gamatemp                = new double [N];
    Abtemp                  = new double [N];
	C                       =new double [Tmax];
    
    A                     =(double **)Alloc2d (N, N,sizeof(double));	   //transion matrix
    APre                  =(double **)Alloc2d (N, N,sizeof(double));	   //transion matrix
	Atemp                 =(double **)Alloc2d (N, N,sizeof(double));  
    B                     =(double **)Alloc2d (N,Tmax,sizeof(double));	  
    gamer                 =(double **)Alloc2d (Tmax,N,sizeof(double));     
    Gamer                 =(double ***)Alloc3d (Tmax,N,M,sizeof(double));  
    N_Prob                =(double ***)Alloc3d (Tmax,N,M,sizeof(double));
    Mixture               =(double ***)Alloc3d(N,M,p,sizeof(double));  
    Cov                   =(double ***)Alloc3d(N,M,p,sizeof(double));  
	af                    =(double ** )Alloc2d (Tmax,N,sizeof(double));  
    back                  =(double ** )Alloc2d (Tmax,N,sizeof(double));  
    Eita                  =(double **)Alloc2d (N, M, sizeof(double));	
	Cpai                  =(double **)Alloc2d (N, M,sizeof(double));
    Cpaitemp              =(double **)Alloc2d (N, M,sizeof(double));
    Tempmean              =(double ***)Alloc3d (N,M,p,sizeof(double));
    Tempcov               =(double ***)Alloc3d (N,M,p,sizeof(double));

	InitMixture(Sequence,Mixture,Cov,T,N,M,L,p);
	InitAll(Pi,A,APre,Cpai,M,N);

	ite=0;
	while(ite<ITMAX)
	{
		 InitZero(Cpaitemp,Gamatemp,Atemp,Abtemp,Eita,Tempmean,Tempcov,M,N,L,p);
 
         for(int l=0; l<L; l++)  
		 {
              CalculateB(B,Cpai,Sequence[l].O,Mixture,Cov,N_Prob,M,N,p,T[l]);//scaling problem
			  forward_backward(Pi,A,B,N,T[l],C,af,back);
              semi_con_probility(af,back,C,Cpai,Gamer,N_Prob,B,T[l],N,M);//beta problem
			  CaculateC(Cpaitemp,Gamatemp,Gamer,M, N, T[l]);
		      CaculateA(A,B,af,back,C,Atemp,Abtemp,N,T[l]);
              CaculateMX(Sequence[l].O,Gamer,Mixture,Cov,Tempmean,Tempcov,
				         Eita,M,N,T[l],p);
		 }
		 Restamate(Cpaitemp,Gamatemp,Atemp,Abtemp,Tempmean,Tempcov,Eita,Cpai,A,
			       Mixture,Cov,M,N,p);
		 if(ite==1)
		     Eps=1;
		 else
			Eps=Caculate(A,APre,N);
		 ite++;
		// if(Eps<0.000001)
			 //break;
	}
//	CString str;
//	str.Format("%f d%",Eps,ite);
	//AfxMessageBox(str);
//	m_pTrain->SaveNewHmmCode(Pi, A, Cpai, Mixture,Cov,M,N,p);
	CString fileName=FileOut;
	CString WaitWord=fileName.Mid(fileName.ReverseFind('\\')+1,fileName.ReverseFind('.')-fileName.ReverseFind('\\')-1);

	WriteHmmFile(Pi,A,Cpai,Mixture,Cov,M,N,p,WaitWord);
	FileOutput(FileOut,Pi,A,Cpai,Mixture,Cov,M,N, p);
	
    delete []Pi;
    delete []LgP;
    delete []Gamatemp;
    delete []Abtemp;
    delete []C;
	Free3d((char***)Mixture);
	Free3d((char***)Cov);
	Free3d((char ***)N_Prob);
	Free2d((char **)Eita);
	Free2d((char **)A);
    Free2d((char **)APre);
    Free2d((char **)Atemp);  
    Free2d((char **)B);	  
    Free2d((char **)gamer);     
    Free3d((char ***)Gamer);  
    Free2d((char **)af);  
    Free2d((char **)back);  
    Free2d((char **)Cpai);
    Free2d((char **)Cpaitemp);
    Free3d((char ***)Tempmean);  
    Free3d((char ***)Tempcov);  
}

void  CHMM::GetT(FILE *fp,int *T,int *p)
{
  int Ti,pi,iin,il;
  float buffer;
  Ti=0;
  pi=0;
  fscanf(fp, "%d", &Ti);
  fscanf(fp, "%d", &pi);

  *T=Ti;
  *p=pi;
    for (iin = 0; iin < Ti; iin++) 
	{
        for (il = 0; il < pi; il++)	
		{
			if (fscanf(fp, "%f", &buffer) == EOF)
				exit(1);
		}
	}
}

double** CHMM::GetData(FILE *fp,int Tmax,int *tl,BOOL bNeedCalibrate)
{//true 表示没有归一，需要归一
//false表示归一了。
  int no_of_inputs,ip_dimension,il;
  float  buffer;
  double **totaldata;

  fscanf(fp, "%d", &no_of_inputs);
  fscanf(fp, "%d", &ip_dimension);

  *tl=no_of_inputs;
/*  double MinVa[51]={ -0.866, -1.000,  -1.000,  -0.982,  -0.950,  -0.809,  -0.793,  -0.983,  -1.000,  -0.995,  -0.998,  -0.516, 1.383, 9.321, 8.023, 1.000,  25.000, 1.000, 1.000,  36.000, 1.000,  13.000,  49.000, 1.000,  36.000,  47.000, 1.000,  20.000,  1.000, 1.000,  14.000,  17.000, 1.000,  62.000,  55.000,  26.000, 1.000,  29.000, 2.000, 1.000,  41.000, 1.000,  60.000,  50.000, 1.000,  22.000, 1.000, 1.000,  42.000, 1.000, 1.000};
  double MaxVa[51]={1.000, 1.000, 0.999, 0.978, 0.990, 1.000, 1.000, 1.000, 1.000, 0.889, 0.507, 1.000,  69.890,  66.501,  49.668, 213.000, 220.000, 157.000, 223.000, 183.000, 193.000, 165.000, 202.000, 255.000, 166.000, 138.000, 252.000, 185.000, 211.000, 227.000, 255.000, 189.000,  84.000, 222.000, 250.000, 191.000, 251.000, 196.000, 207.000, 166.000, 208.000, 255.000, 199.000, 208.000, 247.000, 255.000, 209.000, 237.000, 254.000, 202.000, 84.000};
//lizheng 2002, 9.18
*/

  //double MinVa[51]={-0.406,   -0.979,   -1.000,   -0.655,   -0.174,    0.246,   -0.493,   -1.000,   -1.000,   -0.612,   -0.910,    0.411,    3.807,   10.100,    9.293,   62.000,   40.000,   26.000,    1.000,    1.000,    1.000,    5.000,    1.000,   29.000,    5.000,   57.000,    2.000,   44.000,    1.000,   29.000,   57.000,   47.000,   26.000,   99.000,   61.000,    1.000,   82.000,    1.000,   40.000,    1.000,   61.000,   24.000,   13.000,   61.000,    1.000,   45.000,   44.000,   70.000,  143.000,   26.000,    1.000}; 
  //double MaxVa[51]={1.000,    0.999,    0.999,    0.909,    0.904,    1.000,    1.000,    0.987,    0.913,    0.789,    0.152,    1.000,   41.683,   40.850,   22.843,  185.000,  209.000,  176.000,  207.000,  167.000,  213.000,  155.000,  187.000,  233.000,  172.000,  255.000,  227.000,  177.000,  230.000,  197.000,  255.000,  191.000,  123.000,  216.000,  195.000,  191.000,  241.000,  157.000,  218.000,  195.000,  213.000,  218.000,  203.000,  220.000,  202.000,  197.000,  255.000,  255.000,  255.000,  255.000,  160.000}; 

  //double MinVa[51]={-0.406,   -1.000,   -1.000,   -0.639,   -0.229,    0.213,   -0.493,   -1.000,   -0.999,   -0.612,   -0.920,    0.355,    3.535,   10.100,    9.293,   62.000,   40.000,   26.000,    1.000,    1.000,    1.000,    5.000,   29.000,   23.000,    6.000,   57.000,    2.000,   43.000,    1.000,   29.000,   47.000,   29.000,   23.000,   84.000,   56.000,    1.000,   70.000,    1.000,   36.000,    1.000,   56.000,   20.000,   11.000,   65.000,    1.000,   25.000,   56.000,   70.000,  143.000,   26.000,    1.000};
  //double MaxVa[51]={1.000,    0.999,    0.999,    0.931,    0.904,    1.000,    1.000,    0.987,    0.913,    0.789,    0.152,    1.000,   41.683,   42.784,   24.459,  184.000,  209.000,  176.000,  205.000,  166.000,  213.000,  153.000,  187.000,  206.000,  191.000,  255.000,  227.000,  174.000,  230.000,  197.000,  238.000,  187.000,  123.000,  216.000,  212.000,  187.000,  241.000,  157.000,  218.000,  192.000,  214.000,  218.000,  200.000,  223.000,  202.000,  197.000,  255.000,  255.000,  255.000,  255.000,  160.000}; 

//mayan 2003, 10, 31.


 // double MinVa[51]={-0.653,   -1.000,   -1.000,   -0.977,   -0.659,   -0.998,   -0.723,   -0.998,   -1.000,   -0.758,   -0.980,   -0.944,    1.798,    9.661,    7.881,   50.000,   21.000,    1.000,    1.000,    1.000,    1.000,    2.000,    1.000,    1.000,    8.000,   33.000,    1.000,   16.000,    1.000,   10.000,    1.000,   35.000,    9.000,   40.000,   56.000,    1.000,    1.000,    1.000,   20.000,    1.000,    1.000,    1.000,    1.000,   51.000,    1.000,    1.000,    1.000,    1.000,    1.000,   30.000,    1.000};
 // double MaxVa[51]={1.000,    0.999,    1.000,    0.992,    0.960,    1.000,    1.000,    0.999,    0.999,    0.908,    0.873,    1.000,   63.283,   53.565,   36.555,  212.000,  217.000,  180.000,  230.000,  170.000,  240.000,  162.000,  202.000,  222.000,  194.000,  255.000,  255.000,  185.000,  243.000,  189.000,  255.000,  199.000,  195.000,  235.000,  220.000,  210.000,  255.000,  158.000,  230.000,  198.000,  216.000,  255.000,  202.000,  229.000,  222.000,  202.000,  255.000,  255.000,  255.000,  255.000,  175.000};
//mayan 2003. 12 new


//  double MinVa[51]={-0.406,   -1.000,   -1.000,   -0.639,   -0.300,    0.097,   -0.493,   -1.000,   -0.999,   -0.612,   -0.920,    0.355,    2.976,   10.100,    9.293,   52.000,   33.000,    1.000,    1.000,    1.000,    1.000,    3.000,   14.000,   15.000,    6.000,   57.000,    1.000,   27.000,    1.000,    1.000,   42.000,   29.000,    9.000,   84.000,   56.000,    1.000,   67.000,    1.000,   27.000,    1.000,   50.000,    1.000,   10.000,   65.000,    1.000,   25.000,   27.000,   70.000,  137.000,   14.000,    1.000};
 // double MaxVa[51]={1.000,    0.999,    0.999,    0.931,    0.904,    1.000,    1.000,    0.987,    0.960,    0.789,    0.243,    1.000,   44.380,   43.914,   27.505,  190.000,  209.000,  176.000,  205.000,  166.000,  213.000,  159.000,  187.000,  227.000,  191.000,  255.000,  227.000,  174.000,  230.000,  197.000,  246.000,  187.000,  123.000,  216.000,  222.000,  187.000,  242.000,  157.000,  232.000,  192.000,  214.000,  235.000,  200.000,  223.000,  202.000,  197.000,  255.000,  255.000,  255.000,  255.000,  162.000}; 
//mayan 2003. 12 small

  double MinVa[57];
  double MaxVa[57];

  for (int i=6; i<57; i++)
  {
	  MinVa[i] = -0.0295;
	  MaxVa[i] = 0.0149;
  }


  //测试轨迹用
//    double MinVa[6]={-0.1973, -0.3116, 1.0967, 0.0333, -0.35, 1.1214};
//    double MaxVa[6]={-0.0164, 0.1556, 1.2683, 0.2098, 0.1453, 1.2693};
// 	bNeedCalibrate = FALSE;

  //mayan 2004, 3

  //totaldata = new double *[no_of_inputs];
  totaldata=(double**)Alloc2d(no_of_inputs,ip_dimension,sizeof(double));
  for (int iin = 0; iin < no_of_inputs; iin++) 
  {
	  //totaldata[iin] = new double [ip_dimension];
      for ( il = 0; il < ip_dimension; il++)	
	  {
		  
	    	if (fscanf(fp, "%f", &buffer) == EOF)
				exit(1);
	
			if(bNeedCalibrate && il>5) //Modified by Hanjie Wang
				totaldata[iin][il] = (buffer-MinVa[il])/(MaxVa[il]-MinVa[il]);
			else
				totaldata[iin][il] = buffer;

	  }
  }
  return (totaldata);
}

double** CHMM::GetDataFromMemory(double **feature, int frameNum)
{
	int no_of_inputs,ip_dimension,il;
	float  buffer;
	double **totaldata;
	no_of_inputs = frameNum;
	ip_dimension = 61;
	totaldata=(double**)Alloc2d(no_of_inputs,ip_dimension,sizeof(double));
	for (int iin = 0; iin < no_of_inputs; iin++) 
	{
		//totaldata[iin] = new double [ip_dimension];
		for ( il = 0; il < ip_dimension; il++)	
		{
			totaldata[iin][il] = feature[iin][il];
		}
	}
	return (totaldata);
}

double CHMM::N_probility(double *X,double *mu,double *xigamer,int p)
{   
    int i;
    double temp,r,r0;
    temp=1.0;
	r=0;					
    for(int i=0; i<p; i++) 
	{
	  r0 =(X[i]-mu[i]);
	  r +=r0*r0/xigamer[i];
    }

	if(m_bFlagTrain)
	{
		for(int i=0; i<p; i++)
			temp *=qConst/sqrt(xigamer[i]);
	}
	else
		temp*=m_pHmmWordIndex[0]->ConS[0][0];

	temp =temp*exp(-0.5*r);
	//temp=temp*exp(-0.5*r)*qConst;
    return(temp);
}

double CHMM::cal_prob(double *O,double **Mixture,double **Cov,double *Cpai, double *N_Pro,int M,int p)
{
	int i;
	double temp,temp1;
	double sum;
	sum = 0.0;
	for(int i=0;i<M;i++)
	{   
	    temp1 = N_probility(O,Mixture[i],Cov[i],p);

		temp = Cpai[i]*temp1;//fgl
		sum +=temp;//fgl
		//if(temp1>sum)
		//	sum=temp1;
	    N_Pro[i] = temp1;
	}
    return(sum);
}


void CHMM::InitAll(double *Pi,double **A,double **APre,double **Cpai,int M,int N)
{
	int i,j;
    for(int i=0; i<N; i++)
	{
		Pi[i]=0.0;
		for(int j=0; j<N; j++) 
		{
			if(j<i)
				A[i][j]=0.0;
			else
			{
				A[i][j]=1.0/(N-i);
			}
		   APre[i][j]=A[i][j];
		}
	}
    Pi[0]=1.0;
	for(int i=0;i<N;i++)
		for(int j=0; j<M; j++)
			Cpai[i][j]=(double)(1.0/M);
}

void CHMM::InitAllPlus(int nWord,double*** Mixture,double*** Cov,double *Pi,double **A,double **APre,double **Cpai,int M,int N,int p)
{
	int i,j;
    for(int i=0; i<N; i++)
	{
		Pi[i]=m_pHmmWordIndex[nWord]->Pai[i];
		for(int j=0; j<N; j++) 
		{
		   A[i][j]=m_pHmmWordIndex[nWord]->A[i][j];
		   APre[i][j]=A[i][j];
		}
	}
    for(int i=0;i<N;i++)
		for(int j=0; j<M; j++)
		{
			Cpai[i][j]=m_pHmmWordIndex[nWord]->C[i][j];
			for(int k=0;k<p;k++)
			{
				Mixture[i][j][k]=m_pHmmWordIndex[nWord]->Mix[i][j][k];
				Cov[i][j][k]=m_pHmmWordIndex[nWord]->Cov[i][j][k];
			}
		}


}

void  CHMM::InitN(Seq *Sequnce, int *T, int L, int MaxN, int *N, int p)
{
	int j,l,t,Max,Kt,flag;
	int *Seg;
	double iteration;
	double *con,sumcon;

    Seg = new int[MaxN+1];
	con = new double[MaxN+1];

	iteration = 0.2;
	Max=0;
	for(int l=0;l<L;l++)
	{
		flag=0;
		for (Kt=3;((Kt<=MaxN)&&(flag==0));Kt++)
		{
			wAutoSegmentBasicGesture(Sequnce[l].O,T[l],p,Kt,Seg);
			sumcon=0.0;
			for (t=0;t<Kt;t++)
			{
				if ((Seg[t]+1)==Seg[t+1])
				{
					sumcon = 0;
					con[t] = sumcon;
				}
				else
				{
					sumcon = Diavation(Seg[t]+2,Seg[t+1]+1,Sequnce[l].O,p);
 					con[t] = sumcon;
				}
			}
			flag=1;
			for (j=0;j<Kt;j++)	
				if (con[j]>iteration)
					flag=0;
		}
		if (Kt-1>Max)
			Max=Kt-1;
	}
    //*N=Max; 
	*N=MaxN; //Modified by Hanjie Wang
	delete []Seg;
    delete []con;
}


void  CHMM::InitMixture(Seq *Sequnce,double ***Mixture,double ***Cov,int *T,int N,int M,int L,int p)
{
	int j,l,k,t,numdata,number,**Kts,**Kte,*Seg_point;
    double **mu;
	
	Seg_point = new int[N+1];
	Kts = new int*[L];
	Kte = new int*[L];
    for(int l=0;l<L;l++)
	{
		wAutoSegmentBasicGesture(Sequnce[l].O,T[l],p,N,Seg_point);
        Kts[l] = new int[N];
		Kte[l] = new int[N];
		for (k=0;k<N;k++)
		{
			Kts[l][k]=Seg_point[k]+1;
			Kte[l][k]=Seg_point[k+1];
		}
	}
    mu =(double **)Alloc2d (L,p,sizeof(double));     
	number = 0;
	for(int k=0;k<N; k++)  
	{
		for(int l=0; l<L; l++)  
		{
            numdata = Kte[l][k]-Kts[l][k]+1; 
			for(int j=0;j<p;j++)
				mu[l][j]=0.0;
			for(t=Kts[l][k]; t<=Kte[l][k]; t++) 
			{
				for(int j=0; j<p; j++) 
				{
					 mu[l][j] +=Sequnce[l].O[t][j];
				}
			}
			for(int j=0; j<p; j++)
			{
				 mu[l][j] /=numdata;
			}
		}
		LBG_Train(Mixture[k],mu,M,L,p,10,0.1);
	}
	for(int k=0;k<N;k++)
		for(int j=0;j<M;j++)
			for(int l=0;l<p;l++)
				Cov[k][j][l] = 0.2;

    for(int l=0;l<L;l++)
	{
		delete []Kts[l];
		delete []Kte[l];
	}
	delete []Kts;
	delete []Kte;
	delete []Seg_point;

	Free2d((char **)mu);
	////////////////
}

void  CHMM::LBG_Train(double **mu,double **Ov, int  M,int numdata,int p,int iterations,double Up)
{
  int   i, j, k;                          /* loop counter variables      */
  int   count = 0;
  int   *labels;
  double *dist, sum;
  double prev = 1000.0;
  if (!(labels = (int *) malloc(numdata * sizeof(int))))
    exit(1);
  if (!(dist   = (double *) malloc(p * sizeof(double))))
    exit(1);

  int *rndvec,flag,index,l;
	rndvec =new int [M];
    srand(123);
    for(int i=0; i<M; i++)
        rndvec[i] = -1;
	if(M<=numdata)
	{
		for (j=0; j<M; j++)
		{
			 flag = 1;
			 while (flag)
			 {
				  index = (int) (rand_val()*((double) numdata));
				  flag = 0;
				  for(int i=0; i<M; i++)
					  flag = flag || (rndvec[i] == index);//find there no used
			 }
			 rndvec[j] = index;
			 for(int l=0; l<p; l++)
				mu[j][l]=Ov[index][l];
		}
	}
	else
	{
		for(int j=0;j<numdata;j++)
		{
			flag=1;
			while(flag)
			{
				index = (int) (rand_val()*((double) numdata));
				flag = 0;
				for(int i=0; i<numdata; i++)
				  flag = flag || (rndvec[i] == index);//find there no used
			 }
			 rndvec[j] = index;
			 for(int l=0; l<p; l++)
				mu[j][l]=Ov[index][l];
		}
		for(int k=0;k<M/numdata;k++)
			for(int j=0;j<numdata;j++)
				for(int l=0;l<p;l++)
					mu[k*numdata+j][l]=mu[j][l];
		for(int j=0;j<M%numdata;j++)
			for(int l=0;l<p;l++)
				mu[numdata*(M/numdata-1)+j][l]=mu[j][l];
			

	}
	delete rndvec;
    while (count<iterations) {
       sum = 0.0;
       for (j=0; j<numdata; j++) {
           labels[j] = winning_cells(Ov,mu, &dist, j, M, p);
		   for (k=0; k<p; k++)
	            sum += dist[k] * dist[k];
	   }
       sum /= ((double) numdata)*((double) p);
       if (prev-sum<Up)
           break;
       for(int i=0; i<M ; i++) {
            Centroids(mu[i], Ov, labels, numdata, p, i);
	   }
       count++;
       prev = sum;
	}
	free(dist);
	free(labels);
}


int  CHMM::winning_cells(double **input_matrix,double **vect,double **diffvect, 
		 int I,int numcv,int ip_dimension)
{
  int   i, j,   w_cell;
  double *y, vx, vv, max;
  if (!(y = (double *) malloc(numcv * sizeof(double))))
    exit(1);

  for (i = 0; i < numcv; i++) 
  {
    vv = 0.0;
    vx = 0.0;

    for (j = 0; j < ip_dimension; j++) 
	{
      vv += vect[i][j] * vect[i][j];
	  vx += vect[i][j] * input_matrix[I][j];
	}
    y[i] =(double)( vx - (vv / 2.0));
  }

  max = y[0];
  w_cell = 0;
  for (i = 1; i < numcv; i++) 
  {
    if (y[i] > max) 
	{
      max = y[i];
      w_cell = i;
    }
  }
  for (i = 0; i < ip_dimension; i++)  
    (*diffvect)[i] = input_matrix[I][i] - vect[w_cell][i];
  
  free((char *) y);
  
  return (w_cell);
}


int  CHMM::Centroids(double *centvec,double **data, int *labelvec,
	           int numdata, int dimension,int label)
{
  int i, j,p;
  int numlabel = 0;

  for (j=0; j< dimension; j++)
    centvec[j] = 0.0;

  for(int i=0; i<numdata; i++) 
  {
    if (labelvec[i] == label) 
	{
      for (j=0; j<dimension; j++) 
	  {
	      centvec[j] += data[i][j];
      }
      numlabel++;
    }
  }
  if (numlabel == 0) 
  {
	 p=int(rand_val()*numdata);
	  for (j=0; j<dimension; j++) 
	  {
	      centvec[j] = data[p][j];
      }
	  numlabel++;
  }

   for (j=0; j<dimension; j++)
   centvec[j] /= (double) numlabel;
   return(numlabel);
}


void CHMM::wAutoSegmentBasicGesture(double **data,int TFrames,int Dimen,int K_Seg,int *Seg)
{
   int t,k,j;
   double **F,temp;
   int **B;
   F=new double *[K_Seg+1];
   for(int k=0; k<K_Seg+1; k++)
	   F[k]=new double[TFrames+1];
   B=new int *[K_Seg+1];
   for(int k=0; k<K_Seg+1; k++)
	  B[k]=new int[TFrames+1];
   F[1][1]=99999.0;
   for(j=2;j<TFrames+1; j++) 
   {
	  F[1][j]=Diavation(1,j,data,Dimen);
   }
   for(t=1; t<TFrames+1; t++)  
   {
       for(int k=2; k<K_Seg+1; k++ )  
	   {
		  F[k][t]=(double)1.e30;
		  B[k][t]=j;
		  for(j=1; j<t-1; j++)	   
		  {
		     temp=F[k-1][j]+Diavation(j+1, t,data,Dimen);
			 if(temp<F[k][t]) 
			 {
				 F[k][t]=temp;
				 B[k][t]=j;
			 }
		  }
	   }
   }
   //////////////backtrace//////////////////////
   int *T_Seg;
   T_Seg=new int[K_Seg+1];
   T_Seg[K_Seg]=TFrames;

   for(int k=K_Seg; k>1; k--) {
	   T_Seg[k-1]=B[k][T_Seg[k]];
   }

   for(int k=1; k<K_Seg; k++)
       Seg[k]=T_Seg[k]-1;

   Seg[K_Seg]=TFrames-1;
   Seg[0]=-1;
   
   for(int k=0; k<K_Seg+1; k++)
      delete []F[k];
   delete []F;
   for(int k=0; k<K_Seg+1; k++)
	   delete []B[k];
   delete []B;
   delete []T_Seg;
}

double CHMM::Diavation(int j,int i,double **data,int p) 
{
	double *mean;
	double sum,r;
    int l,lp;

	if(i<=j+1)  
	{
		return(100);
	}
	i--;
	j--;

	mean=new double[p];

	for( lp=0; lp<p; lp++)
		mean[lp]=0;
	for(lp=0; lp<p; lp++)	
	{
		for(int l=j; l<i+1; l++)   
			mean[lp] +=data[l][lp];
		mean[lp] /=(i-j+1);
	}
	sum = 0.0;
    for(int l=j;l<i+1;l++)
		for(lp = 0;lp<p;lp++)
		{
			r = data[l][lp]-mean[lp];
			sum += r*r;
		}
	sum = sum/(i-j+1);

	delete []mean;
	return(sum);
}


void CHMM::InitZero(double **C_mid,double *sump,double **A_up,double *temp,double **Eita,
			        double ***Tempmean,double ***Tempcov,int M,int N,int L,int p)
{
	 int i,j,k,lp; 
     for(int j=0; j<N; j++)
         for(int k=0; k<M; k++)
		 {
             C_mid[j][k]=0.0;
		 }
      for(int j=0; j<N; j++)
	  {
          sump[j]=0.0;
	  }
      for(int i=0; i<N; i++)
         for(int j=0; j<N; j++)
            A_up[i][j]=0.0; 
      for(int j=0; j<N; j++)
         temp[j]=0.0;
	  for(int k=0; k<N; k++)
		  for(int j=0;j<M;j++)
       		  Eita[k][j]=0.0;
     for(int j=0; j<N; j++)
         for(int k=0; k<M; k++)
			 for(lp=0;lp<p;lp++)
			 {
				 Tempmean[j][k][lp] = 0.0;
				 Tempcov [j][k][lp] = 0.0;
			 }
}


void CHMM::CalculateB(double **B, double **Cpai,double **O,
					  double ***Mixture,double ***Cov,double ***N_prob,
					  int M,int N,int p,int T)
{
	  int j,t;
      for(int j=0; j<N; j++)  
	  {
           for(t=0; t<T; t++)  
		   {
              B[j][t] = cal_prob(O[t],Mixture[j],Cov[j],Cpai[j],N_prob[t][j],M,p)*10;
		   }
	  }
}

void CHMM::CalculateBPlus(double **B, double **Cpai,double **O,double **dwWeight,
					  double ***Mixture,double ***Cov,double ***N_prob,
					  int M,int N,int p,int T)
{
	  int j,t;
      for(int j=0; j<N; j++)  
	  {
           for(t=0; t<T; t++)  
		   {
              B[j][t] = cal_prob(O[t],Mixture[j],Cov[j],Cpai[j],N_prob[t][j],M,p);
			  B[j][t]*=dwWeight[j][t];
		   }
	  }
}
void CHMM::forward_backward(double *Pi,double **A,double **B,int N,int T,
							double *C,double **af,double **back) 
{
	int i,j,t;
	double temp=0.0;
	//// need to change "T"-->"N"
	double af_temp[20];
	for(int i=0; i<N; i++) 
	{
	   af[0][i]=Pi[i]*B[i][0]; 
	   temp +=af[0][i];
	}
	C[0]=1.0/(temp+1.e-20);
	for(int i=0; i<N; i++)
	{
		af[0][i] *=C[0];
	 }    
	for(t=1; t<T; t++)
	{
		temp=0.0;
		for(int j=0; j<N; j++)  
		{
			af_temp[j]=0;
			for(int i=0; i<N; i++)  
			{
			   af_temp[j] +=af[t-1][i]*A[i][j]*B[j][t];
			}
			temp +=af_temp[j];
		}
		C[t]=1.0/temp;
		for(int i=0; i<N; i++) 
		{
			af[t][i]=C[t]*af_temp[i];
		 }
	}

	for(int j=0; j<N; j++) 
	{
		back[T-1][j]=C[T-1];
	}
	for(t=T-2 ;t >-1; t--)  
	{
		for(int i=0; i<N; i++)     
		{
			temp=0.0;
			for(int j=0; j<N; j++)    
			{
				temp+=A[i][j]*B[j][t+1]*back[t+1][j];
			}
			back[t][i]=C[t]*temp;
		}
	}
}
 
void CHMM::semi_con_probility(double **af,double **back,double *C,
							  double **Cpai,double ***Gamer,
							  double ***N_Prob,double** B,int T,int N,int M)
{
	double gama,beta;
	int t,j,k;
	for(t=0; t<T; t++)	
	{
		/*beta=0.0;
		for(int j=0;j<N;j++)
		{
			beta+=af[t][j]*back[t][j];
		}*/

		for(int j=0; j<N; j++)  
		{
			//beta=af[t][j]*back[t][j]/(beta+double(1.e-20));//(C[t]+double(1.e-20));// ????
			beta=af[t][j]*back[t][j]/(C[t]+double(1.e-20));// ????
			for(int k=0; k<M; k++)  
			{
				gama=Cpai[j][k]*N_Prob[t][j][k]/(B[j][t]+double(1.e-20));
                Gamer[t][j][k]=gama*beta;
			}
		}
	}
}

void CHMM::CaculateC(double **Cpaitemp,double *Gamatemp,double ***Gamer,
					 int M,int N,int T)
{
	int j,k,t;
	double temp1,temp0;

   for(int j=0; j<N; j++) 
   {
	   temp1 = 0.0;
       for(int k=0; k<M; k++)  
	   {
           temp0=0.0;
           for(t=0; t<T; t++)  
		   {
              temp0 +=Gamer[t][j][k];
		   }
           temp1 +=temp0;
           Cpaitemp[j][k] +=temp0;// Cpaitemp is sum(y(j,m) which calculate with time
	   }
	   Gamatemp[j] += temp1;//Gamatemp is Cjm denominator
   }
}

void CHMM::CaculateMX(double **O,double ***Gamer,double ***Mixture,double ***Cov,
					  double ***Tempmean,double ***Tempcov,double **Eita,
					  int M,int N,int T,int p)
{
	int j,lp,t,m;
	double temp1,temp2,r;
	for(int j=0; j<N; j++)
	{
	   for(m=0;m<M;m++)
	   {
		  temp1=0;
		  for(t=0; t<T; t++)  
		  {
			 temp2 =Gamer[t][j][m];
			 for(lp=0; lp<p; lp++)   
			 {
				 Tempmean[j][m][lp]+=temp2*O[t][lp];//Tempmean is numerator of mu
				 r = O[t][lp]-Mixture[j][m][lp];
				 Tempcov[j][m][lp] +=temp2*r*r;//Tempcov is numerator of sigma
			 }
			 temp1 +=temp2;
		  }
		  Eita[j][m] +=temp1;
	   }
	}		  
}

void CHMM::CaculateA(double **A,double **B,double **af,double **back,double *C,double **A_up,double *A_down,int N,int T)
{
	 /*int i, j, t;
	 double temp0, temp1;
     for(int i=0; i<N; i++)  
	 {
          for(int j=0; j<N; j++)
		  { 
               temp0=0.0;                 
               for(t=0; t<T-1; t++) 
			   {
				   temp0 += af[t][i]*A[i][j]*B[j][t+1]*back[t+1][j]*C[t]/1200;//1200 is scaling number can be modify
			   }
               A_up[i][j] +=temp0;         
		  }
          temp1=0;
          for(t=0; t<T-1; t++) 
		  {   
              temp1 +=af[t][i]*back[t][i]*C[t]/1200;//(C[t]+(double)1.e-20); //????
		  }
          A_down[i] +=temp1;
	 }*/
/*	 for(int i=0; i<N; i++)  
	 {
		  temp1=0.0;
          for(int j=0; j<N; j++)
		  { 
               temp0=0.0;                 
               for(t=0; t<T-1; t++) 
			   {
				   temp0 += af[t][i]*A[i][j]*B[j][t+1]*back[t+1][j];
			   }
               A_up[i][j] +=temp0;         

			   temp1+=temp0;
		  }
          A_down[i] +=temp1;
	 }*/
	 int i, j, t;
	 double temp0, temp1;
     for(int i=0; i<N; i++)  {
          for(int j=0; j<N; j++)  { 
              temp0=0.0;                 
               for(t=0; t<T-1; t++) {
				   temp0 += af[t][i]*A[i][j]*B[j][t+1]*back[t+1][j];
			   }
               A_up[i][j] +=temp0;         
		  }
          temp1=0.0;
          for(t=0; t<T-1; t++) {   
              temp1 +=af[t][i]*back[t][i]/(C[t]+(double)1.e-20);
		  }
          A_down[i] +=temp1;
	 }

}

void CHMM::Restamate(double **Cpaitemp,double *Gamatemp,double **Atemp,
					 double *Abtemp,double ***Tempmean,double ***Tempcov,
					 double **Eita,double **Cpai,double **A,double ***Mixture, 
					 double ***Cov,int M,int N,int p)
{
  int i,j,k,lp;
  double sum;

  for(int j=0; j<N; j++) 
  {
	  sum = 0.0;
	  for(int k=0; k<M; k++)  
	  {
         Cpai[j][k] = double (Cpaitemp[j][k] / (Gamatemp[j] + 1.e-20));//+0.001);
		 sum +=Cpai[j][k];
	  }
	  for(int k=0; k<M; k++)  
	  {
         Cpai[j][k] = Cpai[j][k]/(sum+1.e-20);
	  }
  }
 
  for(int i=0;i<N;i++)
  {
	  sum=0.0;
      for(int j=0; j<N; j++) 
	  {
		 A[i][j] = double (Atemp[i][j] / (Abtemp[i] + 1.e-20)); 
		 sum+=A[i][j];
	  }
	  for(int j=0; j<N; j++)
	  {
		  if(sum!=0)
			A[i][j]=A[i][j]/sum;
	  }
      for(int j=0;j<M;j++)
	  {
		for (lp=0;lp<p;lp++)
		{
		  Mixture[i][j][lp] = Tempmean[i][j][lp]/(Eita[i][j]+(double)1.e-20);
		 // Cov[i][j][lp] = Tempcov[i][j][lp]/(Eita[i][j]+(double)1.e-20)+0.15;// fgl

//		  Cov[i][j][lp] = 0.05;
				if(lp<15) //右手方向
					Cov[i][j][lp] = 0.02;// 0.2 //double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else
					Cov[i][j][lp] = 0.05;//   0.05 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.05;
/*///*
/* 				if(lp<3) //右手方向
					Cov[i][j][lp] = 0.1;// 0.2 //double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else if(lp>5 && lp<9)6 7 8
						Cov[i][j][lp] =0.2;//  0.2 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else if(lp>11&&lp<30)
						Cov[i][j][lp] = 0.2;//  0.2 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else
						Cov[i][j][lp] = 0.1;//   0.05 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.05;
*/
		  /*右手相对参照（即颈部的接受器）的3维方向和3维位置数据，
		左手相对参照（即颈部的接受器）的3维方向和3维位置数据，
		左右手间的欧式距离1维，左手与参照间的欧式距离1维，右手与参照间的欧式距离1维，
		左手手形18维，右手手形18维。*/
/*		  				if(lp<3) //右手方向
					Cov[i][j][lp] = 0.1;// 0.2 //double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else if(lp>=3 && lp < 6)//右手位置
					Cov[i][j][lp] = 0.05;
				else if(lp>=6 && lp<9)//左手方向
					Cov[i][j][lp] =0.1;//  0.2 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else if(lp>=9 && lp<12)//左手位置
					Cov[i][j][lp] =0.1;
				else if(lp>=12 && lp <15)//左右手间的欧式距离1维，左手与参照间的欧式距离1维，右手与参照间的欧式距离1维，
					Cov[i][j][lp] =0.1;
				else if(lp>=15&&lp<33)//左手手形
					Cov[i][j][lp] = 0.1;//  0.2 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else//右手手形
					Cov[i][j][lp] = 0.05;//   0.05 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.05;
*/
/*
				if(lp<3) //右手方向
					Cov[i][j][lp] = 0.05;// 0.2 //double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else if(lp>=3 && lp < 6)//右手位置
					Cov[i][j][lp] = 0.02;
				else if(lp>=6 && lp<9)//左手方向
					Cov[i][j][lp] =0.05;//  0.2 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else if(lp>=9 && lp<12)//左手位置
					Cov[i][j][lp] =0.05;
				else if(lp>=12 && lp <15)//左右手间的欧式距离1维，左手与参照间的欧式距离1维，右手与参照间的欧式距离1维，
					Cov[i][j][lp] =0.05;
				else if(lp>=15&&lp<33)//左手手形
					Cov[i][j][lp] = 0.05;//  0.2 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.20;
				else//右手手形
					Cov[i][j][lp] = 0.02;//   0.05 double (Tempxigamer[i][lp] / (Eita[i] + (double)1.e-20))+(double)0.05;
			
//*/

		}
	  }
  }
  for(int i=0;i<N;i++)
	  for(int j=i;j<N;j++)
	  {
		  if(A[i][j]<0.01)
		  {
			  A[i][j]=0.01;
			  sum=0.0;
			  for(int k=i;k<N;k++)
			  {
				  if(k==j)
					  continue;
				  sum+=A[i][k];
			  }
			  for(int k=i;k<N;k++)
			  {
				  if(k==j)
					  continue;
				  A[i][k]=A[i][k]*0.99/sum;
			  }
		  }
	  }
}

double CHMM::Caculate(double **A,double **APre,int N)
{
	int i,j;
	double eps;
	eps=0;
	for(int i=0; i<N; i++)  {
		for(int j=0; j<N; j++)  {
			eps +=(double)fabs(A[i][j]-APre[i][j]);
			APre[i][j]=A[i][j];
		}
	}
	eps /=N;
	return(eps);
}
void  CHMM::FileOutput(char *FileName,double *Pi,double **A,double **Cpai,
					   double ***Mixture,double ***Cov,int M,int N,int p)
{
	    int i,l,lp;
	    FILE *fpout;

	  	fpout=fopen(FileName,"w");
		if(fpout==NULL) {
			AfxMessageBox("Can not create output file.");
			return;
		}
		fprintf(fpout, "MixS= %-10d", M);     /*  codebook data */
		fprintf(fpout, "MixT= %-10d", N);     /*  codebook data */
		fprintf(fpout, "ShapeDimension1= %-10d\n", p);     /*  codebook data */

		fprintf(fpout, "\n\n");
		fprintf(fpout, "Pai(N):\n");
		for (i = 0; i < N; i++) {
				if (fprintf(fpout, "%8.3f", (float)Pi[i]) == EOF){
					printf("error: Insufficient codebook data");
					return;
				}
		}
		fprintf(fpout, "\n");

		fprintf(fpout, "\n\n");
		fprintf(fpout, "A(N*N):\n");
		for (i = 0; i < N; i++) {
			for (l = 0; l <N; l++)       {
				if (fprintf(fpout, "%8.3f", (float)A[i][l]) == EOF){
					printf("error: Insufficient codebook data");
					return;
				}
			}
			fprintf(fpout, "\n");
		}

		fprintf(fpout, "\n\n");
		fprintf(fpout, "Cpai(N*M):\n");
		for (i = 0; i < N; i++)
		{
			for (l = 0; l <M; l++)       
			{
				if (fprintf(fpout, "%8.3f", (float)Cpai[i][l]) == EOF)
				{
					printf("error: Insufficient codebook data");
					return;
				}
			}
			fprintf(fpout, "\n");
		}

		fprintf(fpout, "\n\n");
		fprintf(fpout, "Mixture(N*M):\n");
		for (i = 0; i < N; i++) 
		{
			for (l = 0; l < M; l++)       
			{
				for(lp = 0; lp<p; lp++)
				{
					if (fprintf(fpout, "%8.3f", (float)Mixture[i][l][lp]) == EOF)
					{
						printf("error: Insufficient codebook data");
						return;
					} 
				}
				fprintf(fpout, "\n");
			}
			fprintf(fpout, "\n");
		}
		fprintf(fpout, "\n\n");
		fprintf(fpout, "Cov(N*M):\n");
		for (i = 0; i < N; i++) 
		{
			for (l = 0; l < M; l++)       
			{
				for(lp = 0; lp<p; lp++)
				{
					if (fprintf(fpout, "%8.3f", (float)Cov[i][l][lp]) == EOF)
					{
						printf("error: Insufficient codebook data");
						return;
					} 
				}
				fprintf(fpout, "\n");
			}
			fprintf(fpout, "\n");
		}
		fclose(fpout);
}


double CHMM::Viterbi(double *Pi,double **A,double **B,int T,int N)//, int* StateSeq, bool btr)
{
	double **Fi,fm0;
	int **Psi,i,j,imax,t;
    Fi=(double **)Alloc2d(T,N,sizeof(double));
    Psi=(int **)Alloc2d(T,N,sizeof(int));
    for(int i=0; i<N; i++)  {
		if(B[i][0]<1.e-30) B[i][0]=(double)1.e-30;
        if(Pi[i] >0){
			if(B[i][0]<9.e-31)
			   Fi[0][i]=(double)log(Pi[i])-30;
			else
			   Fi[0][i]=(double)log(Pi[i])+(double)log(B[i][0]);
        }
		else
            Fi[0][i]=(double)-1.e30;
        Psi[0][i]=0;
    }
    for(t=1; t<T; t++)  {
        for(int j=0; j<N; j++)  {
            fm0=(double)-1.e30;
            for(int i=0; i<N; i++)  {
                if((Fi[t-1][i]+log(A[i][j])) >fm0) {
					fm0=Fi[t-1][i]+(double)log(A[i][j]);
					imax = i;
                }
            }
			if(B[j][t]<1.e-30) B[j][t]=(double)1.e-30;
            Fi[t][j]=fm0+(double)log(B[j][t]);
            Psi[t][j]=imax;
		}
	}
       
	fm0=(double)-1.e30;
    for(int i=0; i<N; i++)  {
		if(Fi[T-1][i] >fm0) {
			fm0=Fi[T-1][i];
            imax=i;
		}
	}

	//// Path (state sequence) backtracking:
/*
	if ( btr ) {
		StateSeq[T-1] = imax;
		for(int i=T-2; i>=0; i--) {
			StateSeq[i] = Psi[i+1][StateSeq[i+1]];
		}
	}
*/
	///////////////////////////////////
	   
	Free2d((char**) Fi);
	Free2d((char**) Psi);

	return(fm0);
}

double CHMM::ViterbiSeg(double *Pi,double **A,double **B,int T,int N, int* T_Seg)
{
	double **Fi,fm0;
	int **Psi,i,j,imax,t,*StateSeq;;
    StateSeq=new int[T];    
	Fi=(double **)Alloc2d(T,N,sizeof(double));
    Psi=(int **)Alloc2d(T,N,sizeof(int));
    
    for(int i=0; i<N; i++)  {
		if(B[i][0]<1.e-30) B[i][0]=(double)1.e-30;
        if(Pi[i] >0)
			Fi[0][i]=(double)log(Pi[i])+(double)log(B[i][0]);
        else
            Fi[0][i]=(double)-1.e30;
        Psi[0][i]=0;
    }
    for(t=1; t<T; t++)  {
        for(int j=0; j<N; j++)  {
            fm0=(double)-1.e30;
            for(int i=0; i<N; i++)  {
                if((Fi[t-1][i]+log(A[i][j])) >fm0) {
					fm0=Fi[t-1][i]+(double)log(A[i][j]);
					imax = i;
                }
            }
			if(B[j][t]<1.e-30) B[j][t]=(double)1.e-30;
            Fi[t][j]=fm0+(double)log(B[j][t]);
            Psi[t][j]=imax;
		}
	}
       
	fm0=(double)-1.e30;
    for(int i=0; i<N; i++)  {
		if(Fi[T-1][i] >fm0) {
			fm0=Fi[T-1][i];
            imax=i;
		}
	}

	//// Path (state sequence) backtracking:
		StateSeq[T-1] = imax;
		for(int i=T-2; i>=0; i--) {
			StateSeq[i] = Psi[i+1][StateSeq[i+1]];
		}
	T_Seg[0]=StateSeq[0];
	int st=1;
	for(int i=1; i<N; i++)  {
		T_Seg[i]=-1;
		for(t=st; t<T; t++)  {
			if(StateSeq[t]!=StateSeq[t-1]) { 
			   T_Seg[i]=t-1;
			   st=t+1;
			   break;
			}
		}
		if(T_Seg[i]<0 )	{
			AfxMessageBox("<0");
			T_Seg[i]=T-1;
		}
	}
	T_Seg[N]=T-1;
	///////////////////////////////////
	delete StateSeq;   
	Free2d((char**) Fi);
	Free2d((char**) Psi);
	return(fm0);
}


double CHMM::SViterbi(double *Pi,double **A,double **B,int T,int N, int* StateSeq, bool btr)
{
	double sum,**af;
    af =(double ** )Alloc2d (T,N,sizeof(double));  
	sum=-forward_backwards(Pi,A,B,N,T,af); 
	Free2d((char **)af);
	return(sum);
}

double CHMM::forward_backwards(double *Pi,double **A,double **B,int N,int T,
	                   double **af) 
{
	    int i,j,t;
	    double *af_temp,*C,temp=0,sum=0;
        af_temp=(double *)malloc(N*sizeof(double));
		C=new double[T];
        for(int i=0; i<N; i++) {
           af[0][i]=Pi[i]*B[i][0]; 
           temp +=af[0][i];
         }
        C[0]=double(1.0/(temp+1.e-20));
		sum=(double)log(C[0]+1.e-20);
        for(int i=0; i<N; i++) {
			af[0][i] *=C[0];
         }    
        for(t=1; t<T; t++)   {
            temp=0;
            for(int j=0; j<N; j++)  {
               af_temp[j]=0;
               for(int i=0; i<N; i++)  {
                   af_temp[j] +=af[t-1][i]*A[i][j]*B[j][t];
                  }
               temp +=af_temp[j];
            }
            C[t]=double(1.0/temp);
            for(int i=0; i<N; i++) {
                af[t][i]=C[t]*af_temp[i];
             }
			sum +=(double)log(C[t]+1.e-20);
        }
		free(af_temp);
		delete C;
		return(sum);
}
void CHMM::ReadHmmFile(CHMMStruct**& pHmmIndex,CString fileName)
{
//	long nBegin=GetTickCount();
	CFile fpDat;
	if( !fpDat.Open(fileName, CFile::modeRead |CFile::typeBinary) ) 
	{
		AfxMessageBox("Cann't Open hmmData file");
		exit(1);	//first build 
	}
	//
	LONG FileLen=fpDat.GetLength();
	HGLOBAL hGlobal=::GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE|GMEM_ZEROINIT,FileLen+20);
	if (hGlobal == NULL ) // 内存不够
	{
		AfxMessageBox("内存不够,cann't allocte memory");
		exit(1);
	}
	LONG lBuffSize=0;
	char* pMem=(char*)::GlobalLock(hGlobal);
	if(pMem==NULL)
	{
		AfxMessageBox("Cann't access memory");
		exit(1);
	}
	fpDat.Read(pMem,FileLen);
	fpDat.Close();


	//file header , m_nTotalWord, m_nMaxStateSize, m_nDimension
	memcpy(&m_nTotalHmmWord,pMem+lBuffSize,sizeof(int));
	lBuffSize+=sizeof(int);
	
	memcpy(&m_nMaxStateSize,pMem+lBuffSize,sizeof(int));
	lBuffSize+=sizeof(int);

	memcpy(&m_nDimension,pMem+lBuffSize,sizeof(int));
	lBuffSize+=sizeof(int);

	int M,N;
	float buff;
	for(int nWord=0;nWord<m_nTotalHmmWord;nWord++)//m_nTotalHmmWord;nWord++)
	{
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		memcpy(&M,pMem+lBuffSize,sizeof(int));
		lBuffSize+=sizeof(int);//
	
		memcpy(&N,pMem+lBuffSize,sizeof(int));
		lBuffSize+=sizeof(int);
				
		pHmmIndex[nWord]=new CHMMStruct(M,N,m_nDimension);

		pHmmIndex[nWord]->MixT=N;//StateNum
		pHmmIndex[nWord]->MixS=M;//MixItem
		//omit dimension
		pHmmIndex[nWord]->Dimension=m_nDimension;

		for(int i=0;i<N;i++)
		{
			memcpy(&buff,pMem+lBuffSize,sizeof(float));
			pHmmIndex[nWord]->Pai[i]=buff;
			lBuffSize+=sizeof(float);
		}
		for(int i=0; i<N; i++)
		{
			for(int j=0;j<N;j++)
			{
				memcpy(&buff,pMem+lBuffSize,sizeof(float));
				pHmmIndex[nWord]->A[i][j]=buff;
				lBuffSize+=sizeof(float);
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				memcpy(&buff,pMem+lBuffSize,sizeof(float));
				pHmmIndex[nWord]->C[i][j]=buff;
				lBuffSize+=sizeof(float);
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				for(int k=0;k<m_nDimension;k++)
				{
					memcpy(&buff,pMem+lBuffSize,sizeof(float));
					pHmmIndex[nWord]->Mix[i][j][k]=buff;
					lBuffSize+=sizeof(float);
				}
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				for(int k=0;k<m_nDimension;k++)
				{
					memcpy(&buff,pMem+lBuffSize,sizeof(float));
					pHmmIndex[nWord]->Cov[i][j][k]=buff;
					lBuffSize+=sizeof(float);
				}
				pHmmIndex[nWord]->ConS[i][j] = CalCons(pHmmIndex[nWord]->Cov[i][j],m_nDimension);

			}
		}
	
		//distance omit

		memcpy(pHmmIndex[nWord]->Word,pMem+lBuffSize,MAX_WORD_NUM*sizeof(char));
		lBuffSize+=(sizeof(char)*MAX_WORD_NUM);
	}
	::GlobalUnlock(hGlobal);
	::GlobalFree(hGlobal);
	m_nMixS=pHmmIndex[0]->MixS;

	/*	long nEnd=GetTickCount();
	float Time=(float)(nEnd-nBegin)/1000;
	CString str;
	str.Format("The time is %f second",Time);
	AfxMessageBox(str);*/
}
void CHMM::ReadEmbedHmmFile(CHMMStruct**& pHmmIndex,CString fileName)
{
	CFile fpDat;
	if( !fpDat.Open(fileName, CFile::modeRead |CFile::typeBinary) ) 
	{
		AfxMessageBox("Cann't Open hmmData file");
		exit(1);	//first build 
	}
	//
	LONG FileLen=fpDat.GetLength();
	HGLOBAL hGlobal=::GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE|GMEM_ZEROINIT,FileLen+20);
	if (hGlobal == NULL ) // 内存不够
	{
		AfxMessageBox("内存不够,cann't allocte memory");
		exit(1);
	}
	LONG lBuffSize=0;
	char* pMem=(char*)::GlobalLock(hGlobal);
	if(pMem==NULL)
	{
		AfxMessageBox("Cann't access memory");
		exit(1);
	}
	fpDat.Read(pMem,FileLen);
	fpDat.Close();


	//file header , m_nTotalWord, m_nMaxStateSize, m_nDimension
	int TotalHmmWord;
	memcpy(&TotalHmmWord,pMem+lBuffSize,sizeof(int));
	lBuffSize+=sizeof(int);
	
	memcpy(&m_nMaxStateSize,pMem+lBuffSize,sizeof(int));
	lBuffSize+=sizeof(int);

	memcpy(&m_nDimension,pMem+lBuffSize,sizeof(int));
	lBuffSize+=sizeof(int);

	int M,N;
	float buff;
	for(int nWord=m_nTotalHmmWord;nWord<m_nTotalHmmWord+TotalHmmWord;nWord++)//m_nTotalHmmWord;nWord++)
	{
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		memcpy(&M,pMem+lBuffSize,sizeof(int));
		lBuffSize+=sizeof(int);//
	
		memcpy(&N,pMem+lBuffSize,sizeof(int));
		lBuffSize+=sizeof(int);
				
		pHmmIndex[nWord]=new CHMMStruct(M,N,m_nDimension);;

		pHmmIndex[nWord]->MixT=N;//StateNum
		pHmmIndex[nWord]->MixS=M;//MixItem
		//omit dimension
		pHmmIndex[nWord]->Dimension=m_nDimension;

		for(int i=0;i<N;i++)
		{
			memcpy(&buff,pMem+lBuffSize,sizeof(float));
			pHmmIndex[nWord]->Pai[i]=buff;
			lBuffSize+=sizeof(float);
		}
		for(int i=0; i<N; i++)
		{
			for(int j=0;j<N;j++)
			{
				memcpy(&buff,pMem+lBuffSize,sizeof(float));
				pHmmIndex[nWord]->A[i][j]=buff;
				lBuffSize+=sizeof(float);
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				memcpy(&buff,pMem+lBuffSize,sizeof(float));
				pHmmIndex[nWord]->C[i][j]=buff;
				lBuffSize+=sizeof(float);
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				for(int k=0;k<m_nDimension;k++)
				{
					memcpy(&buff,pMem+lBuffSize,sizeof(float));
					pHmmIndex[nWord]->Mix[i][j][k]=buff;
					lBuffSize+=sizeof(float);
				}
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				for(int k=0;k<m_nDimension;k++)
				{
					memcpy(&buff,pMem+lBuffSize,sizeof(float));
					pHmmIndex[nWord]->Cov[i][j][k]=buff;
					lBuffSize+=sizeof(float);
				}
				pHmmIndex[nWord]->ConS[i][j] = CalCons(pHmmIndex[nWord]->Cov[i][j],m_nDimension);

			}
		}
	
		//distance omit

		memcpy(pHmmIndex[nWord]->Word,pMem+lBuffSize,MAX_WORD_NUM*sizeof(char));
		lBuffSize+=sizeof(char)*MAX_WORD_NUM;
	}
	::GlobalUnlock(hGlobal);
	::GlobalFree(hGlobal);
	m_nMixS=pHmmIndex[0]->MixS;
	m_nTotalHmmWord+=TotalHmmWord;

}
void CHMM::WriteHmmFile(double* Pai,double** A,double** CPai,double*** Mixture, double*** Cov,int M, int N, int Dimension,CString WaitWord)
{
//	long nBegin=GetTickCount();
	m_nDimension=Dimension;
	float buff;
	if(m_nTotalHmmWord==1)
	{
		m_hGlobal=::GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE|GMEM_ZEROINIT, TEMPMEM);
		if (m_hGlobal == NULL ) // 内存不够
		{
			AfxMessageBox("内存不够,cann't allocte memory");
			exit(1);
		}
		m_lBuffSize=0;
		m_pMem=(char*)::GlobalLock(m_hGlobal);
		if(m_pMem==NULL)
		{
			AfxMessageBox("Cann't access memory");
			exit(1);
		}

		//file header , m_nTotalWord, m_nMaxStateSize, m_nDimension
		memcpy(m_pMem+m_lBuffSize,&m_nTotalHmmWord,sizeof(int));
		m_lBuffSize+=sizeof(int);
		
		memcpy(m_pMem+m_lBuffSize,&m_nMaxStateSize,sizeof(int));
		m_lBuffSize+=sizeof(int);

		memcpy(m_pMem+m_lBuffSize,&m_nDimension,sizeof(int));
		m_lBuffSize+=sizeof(int);
	}
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	memcpy(m_pMem+m_lBuffSize,&M,sizeof(int));
	m_lBuffSize+=sizeof(int);//
	
	memcpy(m_pMem+m_lBuffSize,&N,sizeof(int));
	m_lBuffSize+=sizeof(int);
	
	//omit dimension
	for(int i=0;i<N;i++)
	{
		buff=(float)Pai[i];
		memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
		m_lBuffSize+=sizeof(float);
	}
	for(int i=0; i<N; i++)
	{
		for(int j=0;j<N;j++)
		{
			buff=(float)A[i][j];
			memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
			m_lBuffSize+=sizeof(float);
		}
	}

	for(int i=0; i<N; i++)
	{
		for(int j=0;j<M;j++)
		{
			buff=(float)CPai[i][j];
			memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
			m_lBuffSize+=sizeof(float);
		}
	}

	for(int i=0; i<N; i++)
	{
		for(int j=0;j<M;j++)
		{
			for(int k=0;k<Dimension;k++)
			{
				buff=(float)Mixture[i][j][k];
				memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
				m_lBuffSize+=sizeof(float);
			}
		}
	}

	for(int i=0; i<N; i++)
	{
		for(int j=0;j<M;j++)
		{
			for(int k=0;k<Dimension;k++)
			{
				buff=(float)Cov[i][j][k];
				memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
				m_lBuffSize+=sizeof(float);
			}

		}
	}

	//distance omit
	char tempbuffer[500];
	strcpy(tempbuffer, WaitWord);
	memcpy(m_pMem+m_lBuffSize,tempbuffer,MAX_WORD_NUM*sizeof(char));
	
	m_lBuffSize+=sizeof(char)*MAX_WORD_NUM;

	if(m_nTotalHmmWord==MAXWORDNUM)
	{
		CFile fpDat;
		if( !fpDat.Open("HmmData.dat", CFile::modeWrite | CFile::modeCreate|CFile::typeBinary) ) 
		{
			AfxMessageBox("Cann't Open hmmData file");
			exit(1);	//first build 
		}
		memcpy(m_pMem,&m_nTotalHmmWord,sizeof(int));
		fpDat.Write(m_pMem,m_lBuffSize);
		fpDat.Close();
		::GlobalUnlock(m_hGlobal);
		::GlobalFree(m_hGlobal);

	}

/*	long nEnd=GetTickCount();
	float Time=(float)(nEnd-nBegin)/1000;
	CString str;
	str.Format("The time is %f second",Time);
	AfxMessageBox(str);*/
}

double CHMM::CalCons(double *xigamer,int p)
{
    int i;
    double temp=1.0;
    for(int i=0; i<p; i++) {
	  temp *=qConst/sqrt(xigamer[i]);
    }
	return(temp);
}

void CHMM::SegmentIntoWordUnit(int *Maping,double **data,int TFrames,int WordSize,int *T_Seg)
{
   int t,k,j,v;
   double **F,temp,***BWS,**Pailog,***Alog;
   int **B;
   
   F=(double**)Alloc2d(WordSize+1,TFrames+1,sizeof(double));
   B=(int**)Alloc2d(WordSize+1,TFrames+1,sizeof(int));
   BWS =(double***)Alloc3d(WordSize,TFrames,STATENUM,sizeof(double));
   Pailog = (double **)Alloc2d(WordSize,STATENUM,sizeof(double));
   Alog =(double***)Alloc3d(WordSize,STATENUM,STATENUM,sizeof(double));
   
   double*** N_prob=(double***)Alloc3d(MAXFRAMENUM,STATENUM,m_nMixS,sizeof(double));

   for(v=0;v<WordSize; v++)
   {
	   for(int i=0;i<STATENUM; i++)
	   {
		   Pailog[v][i]=(double)log(m_pHmmWordIndex[Maping[v]]->Pai[i]+1.e-30);
		   for(t=0;t<TFrames; t++)
		   {
				   //BWS[v][t][i]=EmissionProbility(i,//state 
					//					  data[t],//t frames data
					//					  m_pHmmWordIndex[Maping[v]]->MeanS,
					//					  m_pHmmWordIndex[Maping[v]]->CovS);
				   BWS[v][t][i]=cal_prob(data[t],m_pHmmWordIndex[Maping[v]]->Mix[i],m_pHmmWordIndex[Maping[v]]->Cov[i],
					   m_pHmmWordIndex[Maping[v]]->C[i],N_prob[t][i],m_pHmmWordIndex[Maping[v]]->MixS,DIMENSION)*10;
				   
				   BWS[v][t][i]=(double)log(BWS[v][t][i]);
		   }
		   for(int k=0; k<STATENUM; k++)
			   Alog[v][i][k]=(double)log(m_pHmmWordIndex[Maping[v]]->A[i][k]+(double)1.e-30);
	   }
   }

   //F[1][1]=0;
   F[1][1]=-(double)1.e30;
   for(int j=2;j<TFrames+1; j++) 
   {
	   if(j>3&&j<60)
	      F[1][j]=E(1, j,data,0,Maping,BWS,Pailog[0],Alog[0]);//Word 0
	   else
		  F[1][j]=-(double)1.e30;
   }
   for(t=1; t<TFrames+1; t++)  
   {
       for(int k=2; k<WordSize+1; k++ ) 
	   {
		  F[k][t]=-(double)1.e30;
		  for(int j=1; j<t-1; j++)	   
		  {
				if((t-j)>3&&(t-j)<60)
			         temp=F[k-1][j]+E(j+1, t,data,k-1,Maping,BWS,Pailog[k-1],Alog[k-1]); //Word k-1
				else
 			         temp=-(double)1.e29;
				 if(temp>F[k][t]) 
				 {
					 F[k][t]=temp;
					 B[k][t]=j;
				 }
		  }
	   }
   }
   T_Seg[WordSize]=TFrames;
   for(int k=WordSize; k>1; k--) {
	   T_Seg[k-1]=B[k][T_Seg[k]];
   }
   for(int k=1; k<WordSize+1; k++)
      T_Seg[k]=T_Seg[k]-1;
   T_Seg[0]=-1;

   //////////////backtrace//////////////////////
   Free3d((char***)N_prob);

   Free3d((char***)BWS);
   Free2d((char**)Pailog);
   Free3d((char***)Alog);
   Free2d((char**)F);
   Free2d((char**)B);

}

double CHMM::E(int j,int i,double **data,int v,int *Maping,double ***BSW,
				double *Pailog,double **Alog) 
{

	    int Tframes;
	    double **Blog,prob;
		Tframes=i-j+1;
		Blog = &BSW[v][j-1];
		prob =InWordViterbi(Pailog,Alog, Blog,Tframes, STATENUM);
		return(prob);


}

double CHMM::InWordViterbi(double *Pilog,double **Alog,double **Blog,int T,int N)
{
	double **Fi,fm0;
	int i,j,imax,t;
        
	Fi=new double *[T];
	for(t=0;t<T; t++)
		Fi[t]=new double [N];
    
    for(int i=0; i<N; i++)  
	{
		Fi[0][i]=Pilog[i]+Blog[0][i];
    }
    for(t=1; t<T; t++)
	{
        for(int j=0; j<N; j++)
		{
            fm0=(double)-1.e30;
            for(int i=0; i<N; i++)
			{
                if((Fi[t-1][i]+Alog[i][j]) >fm0) 
				{
					fm0=Fi[t-1][i]+Alog[i][j];
                }
            }
            Fi[t][j]=fm0+Blog[t][j];//inword there no backtrace
		}
	}
       
	fm0=(double)-1.e30;
    for(int i=0; i<N; i++)  
	{
		if(Fi[T-1][i] >fm0) 
		{
			fm0=Fi[T-1][i];
            imax=i;
		}
	}
	for(t=0;t<T; t++)
		delete Fi[t];
	delete Fi;
	return(fm0);
}

void CHMM::ConvertSourceHMMToBinary(CString strPath, CString strOutFile, CString signsList)
{
	FILE *fp, *fp1;
	int nCount=0;
	char temp[256];
	char word[256];
	CString strName;
	int M, N, nMixS, nMixT, nDimension;
	float buff;
	CHMMStruct* pHmmIndex;

	int m_nTotalHmmWord = 0;
	int m_nMaxStateSize = 0;
	int m_nDimension = 0;

	m_hGlobal=::GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE|GMEM_ZEROINIT, TEMPMEM);
	if (m_hGlobal == NULL ) // 内存不够
	{
		AfxMessageBox("内存不够,cann't allocte memory");
		exit(1);
	}
	m_lBuffSize=0;
	m_pMem=(char*)::GlobalLock(m_hGlobal);
	if(m_pMem==NULL)
	{
		AfxMessageBox("Cann't access memory");
		exit(1);
	}

	//file header , m_nTotalWord, m_nMaxStateSize, m_nDimension
	memcpy(m_pMem+m_lBuffSize,&m_nTotalHmmWord,sizeof(int));
	m_lBuffSize+=sizeof(int);
	
	memcpy(m_pMem+m_lBuffSize,&m_nMaxStateSize,sizeof(int));
	m_lBuffSize+=sizeof(int);

	memcpy(m_pMem+m_lBuffSize,&m_nDimension,sizeof(int));
	m_lBuffSize+=sizeof(int);

	CString strFileName = signsList;
	//SelectFile(strFileName);
	fp1 = Myfopen(strFileName, "r");
	fscanf(fp1, "%d", &nCount);
	for(int i=0; i<nCount; i++)
	{
		fscanf(fp1, "%s", word);
//		strName = m_BaseDir;
//		strName += '\\';
		strName = strPath + '\\';
		strName += word;
		strName += ".hmm";
		fp = Myfopen(strName, "r");
	
		fscanf(fp, "%s %d", temp, &nMixS);
		fscanf(fp, "%s %d", temp, &nMixT);
		fscanf(fp, "%s %d", temp, &nDimension);
		M = nMixS;
		N = nMixT;
		pHmmIndex = new CHMMStruct(M,N,nDimension);
		
		m_nTotalHmmWord++;
		m_nDimension = nDimension;
		if( N > m_nMaxStateSize)
			m_nMaxStateSize = N;

		fscanf(fp, "%s", temp);
		for(int i=0;i<N;i++)
		{
			fscanf(fp, "%f", &buff);
			pHmmIndex->Pai[i]=buff;
		}

		fscanf(fp, "%s", temp);
		for(int i=0; i<N; i++)
		{
			for(int j=0;j<N;j++)
			{
				fscanf(fp, "%f", &buff);
				pHmmIndex->A[i][j] = buff;
			}
		}

		fscanf(fp, "%s", temp);
		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				fscanf(fp, "%f", &buff);
				pHmmIndex->C[i][j] = buff;
			}
		}


		fscanf(fp, "%s", temp);
		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				for(int k=0;k<nDimension;k++)
				{
					fscanf(fp, "%f", &buff);
					pHmmIndex->Mix[i][j][k] = buff;
				}
			}
		}

		fscanf(fp, "%s", temp);
		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				for(int k=0;k<nDimension;k++)
				{
					fscanf(fp, "%f", &buff);
					pHmmIndex->Cov[i][j][k]=buff;
				}
			//	pHmmIndex[nWord]->ConS[i][j] = CalCons(pHmmIndex[nWord]->Cov[i][j],m_nDimension);

			}
		}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		memcpy(m_pMem+m_lBuffSize,&M,sizeof(int));
		m_lBuffSize+=sizeof(int);//
		
		memcpy(m_pMem+m_lBuffSize,&N,sizeof(int));
		m_lBuffSize+=sizeof(int);
		
		//omit dimension
		for(int i=0;i<N;i++)
		{
			buff=(float)pHmmIndex->Pai[i];
			memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
			m_lBuffSize+=sizeof(float);
		}
		for(int i=0; i<N; i++)
		{
			for(int j=0;j<N;j++)
			{
				buff=(float)pHmmIndex->A[i][j];
				memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
				m_lBuffSize+=sizeof(float);
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				buff=(float)pHmmIndex->C[i][j];
				memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
				m_lBuffSize+=sizeof(float);
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				for(int k=0;k<nDimension;k++)
				{
					buff=(float)pHmmIndex->Mix[i][j][k];
					memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
					m_lBuffSize+=sizeof(float);
				}
			}
		}

		for(int i=0; i<N; i++)
		{
			for(int j=0;j<M;j++)
			{
				for(int k=0;k<nDimension;k++)
				{
					buff=(float)pHmmIndex->Cov[i][j][k];
					memcpy(m_pMem+m_lBuffSize,&buff,sizeof(float));
					m_lBuffSize+=sizeof(float);
				}

			}
		}

		fclose(fp);
		//distance omit
		memcpy(m_pMem+m_lBuffSize,word,MAX_WORD_NUM*sizeof(char));
		m_lBuffSize+=sizeof(char)*MAX_WORD_NUM;
		delete pHmmIndex;
	}
	fclose(fp1);

	CFile fpDat;
	if( !fpDat.Open(strOutFile, CFile::modeWrite | CFile::modeCreate|CFile::typeBinary) ) 
	{
		AfxMessageBox("Cann't Open hmmData file");
		exit(1);	//first build 
	}
	memcpy(m_pMem,&m_nTotalHmmWord,sizeof(int));
	memcpy(m_pMem+sizeof(int), &m_nMaxStateSize, sizeof(int));
	memcpy(m_pMem+2*sizeof(int), &m_nDimension, sizeof(int));
	fpDat.Write(m_pMem,m_lBuffSize);
	fpDat.Close();
	::GlobalUnlock(m_hGlobal);
	::GlobalFree(m_hGlobal);



}
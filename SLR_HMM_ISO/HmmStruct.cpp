#include "StdAfx.h"
#include "HmmStruct.h"

CHMMStruct::CHMMStruct()
{
	Pai=NULL;
	A=NULL;
	C=NULL;
	Mix=NULL;
	Cov=NULL;
	ConS=NULL;
	
	Distance=NULL;
	MixS=0;
	MixT=0;
	memset(Word,0,sizeof(char)*MAX_WORD_NUM);

}
CHMMStruct::CHMMStruct(int M, int N ,int Dimension)
{
	int i,j;

	MixS = M;
	MixT = N;
	Pai = new double [N];

	A = new double* [N];
	for ( i=0; i<N; i++)
		A[i] = new double [N];

	C = new double* [N];
	for(int i=0; i<N; i++)
		C[i] = new double [M];

	Mix = new double** [N];
	for(int i=0; i<N; i++)
	{
		Mix[i] = new double* [M];
		for (j=0; j<M; j++)
			Mix[i][j] = new double [Dimension];
	}
	
	Cov = new double** [N];
	for(int i=0; i<N; i++)
	{
		Cov[i] = new double* [M];
		for (j=0; j<M; j++)
			Cov[i][j] = new double [Dimension];
	}

	ConS = new double* [N];
	for(int i=0;i<N;i++)
		ConS[i] = new double [M];

}
CHMMStruct::~CHMMStruct()
{
	int S,T;
	if(Pai!=NULL)
	{
		delete []Pai;
		
		S = MixS;
		T = MixT;
		for ( int j=0; j<T; j++ )
			delete []A[j];
		delete []A;

		for(int j=0; j<T; j++ )
			delete []C[j];
		delete []C;

		for(int j=0; j<T; j++ )
		{
			for(int k=0;k<S;k++)
				delete []Mix[j][k];
			delete []Mix[j];
		}
		delete []Mix;
		
		for(int j=0; j<T; j++ )
		{
			for(int k=0;k<S;k++)
				delete []Cov[j][k];
			delete []Cov[j];
		}
		delete []Cov;
		
		for(int j=0; j<T; j++ )
			delete []ConS[j];
		delete []ConS;
	}
}
Linklists::Linklists()
{
	Psi=NULL;
	Fi=NULL;
	Score=NULL;
	next=NULL;
	nWordNum = 0;
	pWordList = NULL;

}
Linklists::~Linklists()
{
	if(Psi!=NULL)
	{
		for(int i=0;i<nWordNum;i++)
		{
			delete []Psi[i];
			delete []Fi[i];
			delete []Score[i];
		}
		delete []Psi;
		delete []Fi;
		delete []Score;
		delete []pWordList;
	}
}

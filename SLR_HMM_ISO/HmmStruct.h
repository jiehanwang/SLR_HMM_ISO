#ifndef _HMMSTRUCT_H
#define _HMMSTRUCT_H

//must be re-modify this value
#define MAXWORDNUM 369 //1424//1537 // //240 //89//243 //246//5008////4942 //4930//208//99//98////321//405//306//0
//must be consistent with the size of vocabulary

#define DIMENSION 61 //how much dimension
//must be consistent with the data dimension of data
#define STATENUM 10
//Only for segment sequence !

//no usaually change
#define MAXFRAMENUM 300 //define possible max frames, for allocate space

#define TEMPMEM 100000000
//for temporary store HMM file 

#define qConst 1.0/sqrt(2.0*3.1415926)
#define MAX_WORD_NUM 40

struct CHMMStruct
{
	int     MixS; //混合项数
	int     MixT; //状态数
	int Dimension;
	double   *Pai;
	double   **A;
	double   **C;
	double   ***Mix;
	double   ***Cov;
	double   **ConS;
	double   *Distance;
	char  Word[MAX_WORD_NUM];
	CHMMStruct(int M, int N ,int Dimension);
	CHMMStruct();
	~CHMMStruct();
};

typedef struct 
{
	double **O;
	double** dwWeight;

} Seq;

struct Linklists
{
	int   **Psi;		   //to trace back to get the optimal word前一个词的索引号

	int   **Fi;           //[WordSize][StateSize] ;	//to trace back to get the optimal state
	double **Score;	     // path score at time t 
	//前面的二维表示应为[nWordNum][StateSize]

	int nWordNum;//    // the active number of words on this node
	int *pWordList;//     // the ID号 of the words on this node 

	double BeamMax;//
	double BeamEndMax;
	double BeamThreshold;
	double BeamEndThreshold;

	Linklists* next;//下一个指针
	Linklists();
	~Linklists();
};


#endif




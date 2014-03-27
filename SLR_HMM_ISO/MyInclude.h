#ifndef _MYINCLUDE_H
#define _MYINCLUDE_H
/*
��#include "MyInclude.h" �ŵ�Stdafx.h����� ..App.h�м��ɣ�����MyInclude.h .cpp������Ŀ
Ȼ������ʹ�ã��ǳ��򻯡�

1	��̬������ά����10*20*30
	double*** ppp=(double***)Alloc3d(10, 20, 30, sizeof(double));
	
2	������ͷ�
	free3d((double***)ppp);
	��ά��ͬ�������char***�Ͳ���ǿ��ת��
	
3	CString strFolder;//�ļ���·��
	SelectFolder("My title", strFolder);
	//ʹ���ļ���·��
	�ó���������һ�һ�����ʱ����ͨ�ģ�����ͬ��ѡ�ļ��Ի����������

4	CString strFile;
	SelectFile(strFile);

5	������Ի����·��
	CopyFolder("test\\my", "test1\\my1");

6	���µ��÷�
	void CTextTool::FilterFreqFile(CString fileNameIn, CString fileNameOut)
	{
	CMyStdioFile fileR;
	fileR.MyOpen(fileNameIn,"r");
	
	CMyStdioFile fileW;
	fileW.MyOpen(fileNameOut,"w");

	CString strLine;
	while(fileR.MyReadString(strLine))
	{
		if(strLine.Left(2)>="��")
			fileW.MyWriteString(strLine);

	}
	fileR.Close();
	fileW.Close();

	}
	
  */
#include <stdlib.h>
#include <math.h>

char ** Alloc2d (int dim1, int dim2,int size);//��̬����2ά����
int Free2d(char **mat );//�ͷ�2λ����
char ***Alloc3d(int dim1,int dim2, int dim3, int size);//��̬����3ά����
int Free3d (char ***mat);//�ͷ�3λ����

char ** LAlloc2d (int dim1, int dim2,int size);
int LFree2d(char **mat,int dim1);//�ͷ�2λ����
char ***LAlloc3d(int dim1,int dim2, int dim3, int size);//��̬����3ά����
int LFree3d (char ***mat, int dim1, int dim2);//�ͷ�3λ����

FILE *Myfopen( const char *filename, const char *mode ); //�������ļ�
char *Myfgets( char *string, int n, FILE *stream );//������һ��

BOOL SelectFolder(CString strTitle,CString& strFolder);//����һ��ѡ���ļ��жԻ���
void CopyFolder(CString strFrom, CString strTo);//�����ļ���
BOOL DeleteAllFileInFolder(CString strFolder);

void SelectFile(CString& strFile);
void Search_Directory(char* szFilename, CString &strPath);

enum
{
	IntType=0,
	FloatType,
	DoubleType,
	ShortType
};

void DebugOutputDynamicArray(void* pPointer, int nCount, CString fileName, int Type);
void DebugOutputCStringArray(CStringArray& pPointer, int nCount, CString fileName);

class CMyStdioFile: public CStdioFile
{
public:
	BOOL MyReadString(CString& rString);//��һ���ļ�, rString �������ȥ���س�����
	BOOL MyOpen(LPCTSTR fileName,LPCSTR flag);//flag="rb" �����ƶ��� "wb" ������д��"r"�ı�ֻ���� "w"�ı�д��ʽ
	void MyWriteString(LPCTSTR rString);//дһ���ļ� rString дһ�У�����ӻس�����
};
#endif
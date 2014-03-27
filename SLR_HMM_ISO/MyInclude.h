#ifndef _MYINCLUDE_H
#define _MYINCLUDE_H
/*
将#include "MyInclude.h" 放到Stdafx.h里，或者 ..App.h中即可，并把MyInclude.h .cpp加入项目
然后如下使用，非常简化。

1	动态分配三维数据10*20*30
	double*** ppp=(double***)Alloc3d(10, 20, 30, sizeof(double));
	
2	用完后释放
	free3d((double***)ppp);
	二维相同，如果用char***就不需强制转换
	
3	CString strFolder;//文件夹路径
	SelectFolder("My title", strFolder);
	//使用文件夹路径
	该程序参数是我花一下午的时间试通的，它不同于选文件对话框，你可试试

4	CString strFile;
	SelectFile(strFile);

5	可是相对或绝对路径
	CopyFolder("test\\my", "test1\\my1");

6	底下的用法
	void CTextTool::FilterFreqFile(CString fileNameIn, CString fileNameOut)
	{
	CMyStdioFile fileR;
	fileR.MyOpen(fileNameIn,"r");
	
	CMyStdioFile fileW;
	fileW.MyOpen(fileNameOut,"w");

	CString strLine;
	while(fileR.MyReadString(strLine))
	{
		if(strLine.Left(2)>="啊")
			fileW.MyWriteString(strLine);

	}
	fileR.Close();
	fileW.Close();

	}
	
  */
#include <stdlib.h>
#include <math.h>

char ** Alloc2d (int dim1, int dim2,int size);//动态分配2维数组
int Free2d(char **mat );//释放2位数组
char ***Alloc3d(int dim1,int dim2, int dim3, int size);//动态分配3维数组
int Free3d (char ***mat);//释放3位数组

char ** LAlloc2d (int dim1, int dim2,int size);
int LFree2d(char **mat,int dim1);//释放2位数组
char ***LAlloc3d(int dim1,int dim2, int dim3, int size);//动态分配3维数组
int LFree3d (char ***mat, int dim1, int dim2);//释放3位数组

FILE *Myfopen( const char *filename, const char *mode ); //用流打开文件
char *Myfgets( char *string, int n, FILE *stream );//用流读一行

BOOL SelectFolder(CString strTitle,CString& strFolder);//弹出一个选择文件夹对话框
void CopyFolder(CString strFrom, CString strTo);//拷贝文件夹
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
	BOOL MyReadString(CString& rString);//读一行文件, rString 保存的是去掉回车换行
	BOOL MyOpen(LPCTSTR fileName,LPCSTR flag);//flag="rb" 二进制读， "wb" 二进制写，"r"文本只读， "w"文本写方式
	void MyWriteString(LPCTSTR rString);//写一行文件 rString 写一行，不需加回车换行
};
#endif
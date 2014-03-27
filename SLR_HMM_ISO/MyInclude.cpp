#include "stdafx.h"
#include "MyInclude.h"
#include <direct.h>
#include <io.h>
#include <Shlobj.h>
/*
 * Matrix allocation routines.  
 * To allocate a 10 by 10 matrix of floats use:
 *
 *	char **alloc2d();
 *	float **x;
 *
 *	x = (float **) alloc2d(10, 10, sizeof(float));
 *
 * To free this matrix use:
 *
 *	free2d(x);
 */
	
char **Alloc2d(int dim1, int dim2,int size)
{
	int		i;
	unsigned	nelem;
	char	*p, **pp;

	nelem = (unsigned) dim1*dim2;

	p = (char *)calloc(nelem, (unsigned) size);

	if( p == NULL ) {
		return(NULL);
	}

	pp = (char **) calloc((unsigned) dim1, (unsigned) sizeof(char *));

	if (pp == NULL)
	{
		free(p);
		return( NULL );
	}

	for(int i=0; i<dim1; i++)
		pp[i] = p + i*dim2*size; 

	return(pp);	
}


int Free2d(char **mat )

{
	if (mat != NULL && *mat != NULL)
		free((char *) *mat);
	if (mat != NULL)
		free((char *) mat); 
	return(0);
}


char ***Alloc3d(int dim1,int dim2, int dim3, int size )
{
	int	i;
	char	**pp, ***ppp;
	pp = (char **) Alloc2d(dim1*dim2,dim3, size);

	if(pp == NULL) {
	
		return(NULL);
	}

	ppp = (char ***) calloc((unsigned) dim1, (unsigned) sizeof(char **));
	if(ppp == NULL)
	{
		Free2d(pp);
		return(NULL);
	}

	for(int i=0; i< dim1; i++)
		ppp[i] = pp + i*dim2 ;
	return(ppp);
}

int Free3d(char ***mat)
{
	Free2d( *mat );
	if (mat != NULL)
	   free((char *) mat);
	return(0);
}


char **LAlloc2d(int dim1, int dim2,int size)
{
	int		i;
	char	**pp;
	
	pp = (char **) calloc((unsigned) dim1, (unsigned) sizeof(char *));

	if (pp == NULL)
	{
		return( NULL );
	}

	for(int i=0; i<dim1; i++)
	{
		pp[i] = (char *)calloc(dim2, (unsigned) size);
		
		if (pp[i] == NULL)
		{
			LFree2d(pp,i);
			return( NULL);
		}
	}
	return(pp);	
}

int LFree2d(char **mat,int dim1)
{
	for(int i=0;i<dim1;i++)
		free(mat[i]);
	free(mat);
	mat=NULL;
	return(0);
}

char ***LAlloc3d(int dim1,int dim2, int dim3, int size )
{
	int	i;
	char ***ppp;
	
	ppp = (char ***) calloc((unsigned) dim1, (unsigned) sizeof(char **));
	
	if(ppp == NULL)
	{
		return(NULL);
	}
	
	for(int i=0; i< dim1; i++)
	{
		ppp[i]=LAlloc2d(dim2,dim3,size);
		if(ppp[i]==NULL)
		{
			LFree3d(ppp,i,dim2);
			return(NULL);
		}
	}
	return(ppp);
}

int LFree3d(char ***mat, int dim1, int dim2)
{
	for(int i=0;i<dim1;i++)
		LFree2d(mat[i],dim2);
	if (mat != NULL)
		free(mat);
	mat=NULL;
	return(0);
}
///////////////

//file operation
FILE *Myfopen( const char *filename, const char *mode )
{
	FILE* p;
	if((p=fopen(filename,mode))==NULL)
	{
		CString strTmp=filename;
		AfxMessageBox(strTmp+" cann't be opened");
		return NULL;
	}
	return p;
}

char *Myfgets( char *string, int n, FILE *stream )
{
	char* pp=fgets(string,n,stream);
	char* p;
	if((p=strchr(string,'\r'))!=NULL)
		*p='\0';
	if((p=strchr(string,'\n'))!=NULL)
		*p='\0';
	return pp;
}

BOOL CMyStdioFile::MyOpen(LPCTSTR fileName,LPCSTR flag)
{
	CString strFlag=flag;
	if(strFlag=="r")
	{
		if(!CStdioFile::Open(fileName,CFile::modeRead |CFile::typeText))
		{
			AfxMessageBox((CString)fileName+" cann't be opened");
			return FALSE;
		}
	}
	else if(strFlag=="rb")
	{
		if(!CStdioFile::Open(fileName,CFile::modeRead |CFile::typeBinary ))
		{
			AfxMessageBox((CString)fileName+" cann't be opened");
			return FALSE;
		}
	}
	else if(strFlag=="w")
	{
		if(!CStdioFile::Open(fileName,CFile::modeCreate |CFile::modeWrite|CFile::typeText ))
		{
			AfxMessageBox((CString)fileName+" cann't be created");
			return FALSE;
		}
	}
	else if(strFlag=="wb")
	{
		if(!CStdioFile::Open(fileName,CFile::modeCreate |CFile::modeWrite|CFile::typeBinary))
		{
			AfxMessageBox((CString)fileName+" cann't be created");
			return FALSE;
		}
	}
	else if(strFlag=="rw")
	{
		if(!CStdioFile::Open(fileName,CFile::modeReadWrite|CFile::typeText))
		{
			AfxMessageBox((CString)fileName+" cann't be created");
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox("file flag is not correct.");
		return FALSE;
	}
	return TRUE;
}

BOOL CMyStdioFile::MyReadString(CString& rString)
{
	int bFlag=CStdioFile::ReadString(rString);
	int nLoc;
	if((nLoc=rString.Find('\r'))!=-1)
	{
		rString=rString.Left(nLoc);
	}
	if((nLoc=rString.Find('\n'))!=-1)
	{
		rString=rString.Left(nLoc);
	}
	return bFlag;
}

void CMyStdioFile::MyWriteString(LPCTSTR rString)
{
	CString strTmp=rString;
	CStdioFile::WriteString(strTmp+"\r\n");
}


INT CALLBACK BrowseCallbackProc(HWND hwnd, 
                                UINT uMsg,
                                LPARAM lp, 
                                LPARAM pData) 
{
   TCHAR szDir[MAX_PATH];

   switch(uMsg) 
   {
   case BFFM_INITIALIZED: 
      if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir))
      {
         // WParam is TRUE since you are passing a path.
         // It would be FALSE if you were passing a pidl.
         SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
      }
      break;

   case BFFM_SELCHANGED: 
      // Set the status window to the currently selected path.
      if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir))
      {
         SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
      }
      break;
   }
   return 0;
}

BOOL SelectFolder(CString strTitle,CString& strFolder)
{
	BROWSEINFO lpbi;
	TCHAR m_szselected[MAX_PATH];
	LPCITEMIDLIST pidl;
	LPMALLOC pMalloc;

   if (SUCCEEDED(SHGetMalloc(&pMalloc)))
   {
		ZeroMemory(&lpbi,sizeof(lpbi));
		lpbi.hwndOwner=NULL;
		lpbi.pidlRoot= NULL;
		lpbi.pszDisplayName=m_szselected;
		lpbi.lpszTitle="Choose a folder";
		lpbi.lpfn=BrowseCallbackProc;
		lpbi.ulFlags=BIF_EDITBOX | BIF_STATUSTEXT | BIF_VALIDATE  ;
		if ((pidl = ::SHBrowseForFolder(&lpbi)) != NULL)
		{
			strFolder.Empty();
			if (SUCCEEDED(::SHGetPathFromIDList(pidl, m_szselected)))
			{
				strFolder = m_szselected;
			}
			pMalloc->Free(&pidl);
			pMalloc->Release();
			
		}
	}
   return TRUE;
}
   
void CopyFolder(CString strFrom, CString strTo)
{
   SHFILEOPSTRUCT fileop;
   fileop.hwnd=NULL;
   fileop.wFunc=FO_COPY;
   strFrom += "\\*.*";
   strFrom += '\0';
   fileop.pFrom=strFrom;
   fileop.pTo=strTo;
   fileop.fFlags=FOF_NOCONFIRMMKDIR | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMATION;
   
   fileop.fAnyOperationsAborted=FALSE;       
   fileop.hNameMappings=" ";
   
   CString msg="正在拷贝文件夹内容...";
   
   fileop.lpszProgressTitle=msg;
   
   if( SHFileOperation(&fileop) !=0 )
	   MessageBox(NULL," 拷贝文件夹内容失败 !","提示",MB_ICONINFORMATION);
}

void SelectFile(CString& strFile)
{
	char directory[200];
	GetCurrentDirectory(200,directory);
	CFileDialog dlgfile(TRUE, NULL, NULL, OFN_HIDEREADONLY, "txt File(*.txt) | *.txt");
	dlgfile.m_ofn.lpstrTitle="请选择文件";
	if (dlgfile.DoModal() == IDCANCEL)
		exit(1);
	strFile = dlgfile.GetPathName();
	SetCurrentDirectory(directory);
}

void Search_Directory(char* szFilename, CString &strPath)
{//strPath must be empty
	long handle;
	struct _finddata_t filestruct;    //表示文件(或目录)的信息
	char path_search[_MAX_PATH];  //表示查找到的路径结果	// 开始查找工作, 找到当前目录下的第一个实体(文件或子目录)，
		// "＊"表示查找任何的文件或子目录, filestruct为查找结果
	handle = _findfirst("*", &filestruct); // 如果handle为－1, 表示当前目录为空, 则结束查找而返回
	if(handle == -1)
		return; // 检查找到的第一个实体是否是一个目录(filestruct.name为其名称)
	if( ::GetFileAttributes(filestruct.name) & FILE_ATTRIBUTE_DIRECTORY )
	{// 如果是目录, 则进入该目录并递归调用函数Search_Dirctory进行查找，
	// 注意: 如果目录名的首字符为′.′(即为"."或".."), 则不用进行查找
		if( filestruct.name[0] != '.' )
		{
			_chdir(filestruct.name);
			Search_Directory(szFilename,strPath);	// 查找完毕之后, 返回上一级目录
			_chdir("..");
		}
	}
	else // 如果第一个实体不是目录, 则检查是否是要查找的文件
	{// stricmp对两字符串进行小写形式的对比, 返回为0表示完全一致
		if( !stricmp(filestruct.name, szFilename) )
		{// 先获得当前工作目录的全路径
			_getcwd(path_search,_MAX_PATH);
			//再获得文件的完整的路径名(包含文件的名称)
			strcat(path_search,"\\");
			strcat(path_search,filestruct.name);
			AfxMessageBox(path_search); //输出显示
			strPath+="\r\n";
			strPath+=path_search;
		}
	}// 继续对当前目录中的下一个子目录或文件进行与上面同样的查找
	while(!(_findnext(handle,&filestruct)))
	{
		if( ::GetFileAttributes(filestruct.name) & FILE_ATTRIBUTE_DIRECTORY )
		{
			if(*filestruct.name != '.')
			{
				_chdir(filestruct.name);
				Search_Directory(szFilename,strPath);
				_chdir("..");
			}
		}
		else
		{
			if(!stricmp(filestruct.name,szFilename))
			{
				_getcwd(path_search,_MAX_PATH);
				strcat(path_search,"\\");
				strcat(path_search,filestruct.name);
				AfxMessageBox(path_search);
				strPath+="\r\n";
				strPath+=path_search;
			}
		}
	}
	_findclose(handle);	// 最后结束整个查找工作
}



void DebugOutputDynamicArray(void* pPointer, int nCount, CString fileName, int Type)
{//为了在文件里调试是动态现实结果
//pPointer指针，
	//nCount表示多少个数祖， fileName写的文件名，Type表示Intype或者FloatType,DoubleType
#ifdef _DEBUG
	FILE* fp=Myfopen(fileName,"w");
	float* pTempFloat;
	int* pTempInt;
	short* pTempShort;

	int i;
	double* pTempDouble;
	
	switch (Type)
	{
	case IntType:
		pTempInt =(int*)pPointer;
		for(int i=0; i< nCount; i++)
			fprintf(fp, "%d, %d,  ", i, pTempInt[i]);
		break;
	case FloatType:
		pTempFloat=(float*)pPointer;
		for(int i=0; i<nCount; i++)
			fprintf(fp, "%d, %f,  ", i, pTempFloat[i]);
		break;
	case DoubleType:
		pTempDouble=(double*)pPointer;
		for(int i=0; i<nCount; i++)
			fprintf(fp, "%d %3.8f,  ", i, pTempDouble[i]);
		break;
	case ShortType:
		pTempShort = (short*)pPointer;
		for(int i=0; i<nCount; i++)
			fprintf(fp, "%d, %d,  ", i, pTempShort[i]);
		break;
	default:
		break;

	}
	fprintf(fp, "\n");
	fclose(fp);
#endif

}



void DebugOutputCStringArray(CStringArray& pPointer, int nCount, CString fileName)
{
#ifdef _DEBUG
	FILE* fp=Myfopen(fileName,"w");
	for(int i=0; i< nCount; i++)
	{
		fprintf(fp, "%d, %s", i, pPointer[i]);
	}
	fprintf(fp, "\n");
	fclose(fp);
#endif
}

BOOL DeleteAllFileInFolder(CString strFolder)
{
	CFileFind m_FileFind;
	BOOL bFlag=TRUE;
	char directory[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,directory);
	SetCurrentDirectory(strFolder);

	bFlag=m_FileFind.FindFile("*.*");
	while(bFlag)
	{ 
		bFlag=m_FileFind.FindNextFile();
		CString fileName=m_FileFind.GetFilePath();
		if(!m_FileFind.IsDirectory())
			DeleteFile(fileName);
	}
	SetCurrentDirectory(directory);
	return TRUE;
}

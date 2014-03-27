#include "StdAfx.h"
#include "Readvideo.h"


Readvideo::Readvideo(void)
{
	//depthData = new ushort[640*480];
}


Readvideo::~Readvideo(void)
{
	vSkeletonData.clear();

	for(int i=0;i<vDepthData.size();i++)
		vDepthData[i].release();
	vDepthData.clear();

	for(int i=0;i<vColorData.size();i++)
		cvReleaseImage(&(vColorData[i]));
	vColorData.clear();

	//////////////////////////////////////////////////////////////////////////
	//delete[] depthData;
}

void Readvideo::readvideo(string filePath)
{
	clock_t start, durTime;
	clock_t start_color, durTime_color;
	clock_t start_depth, durTime_depth;
	clock_t start_skeleton, durTime_skeleton;
	start=clock();
	if( !( readColorFrame(filePath) && readDepthFrame(filePath) && readSkeletonFrame(filePath) ))
	{
		return;
	}	

	if( !( vSkeletonFrame.size() == vColorFrame.size() && vSkeletonFrame.size() == vDepthFrame.size() ))
	{
		return;
	}

	string str = filePath.substr(filePath.length()-25,21);
	cout<<str<<endl;
	cout<<"Reading..."<<endl;
	const char* savefilename=str.c_str();

	//读取彩色、深度和骨架数据
	
	const char* filePathChar=filePath.c_str();
	char filePathName[100];
	strcpy(filePathName,filePathChar);
	strcat(filePathName,"\\color.avi");
	CvCapture *capture = cvCreateFileCapture(filePathName);
	if( NULL == capture )
		return;
	

	
	int x,y;
	ifstream depthFileReader;
	depthFileReader.open(filePath+"\\depth.dat",ios::binary);
	if(depthFileReader == NULL)
		return;
	

	
	ifstream skeletonFileReader;
	skeletonFileReader.open(filePath+"\\skeleton.dat",ios::binary);
	if(skeletonFileReader == NULL)
		return;
	

	//read first color depth and skeleton data
	durTime_skeleton = 0;
	start_skeleton = clock();
	SLR_ST_Skeleton mSkeleton;
	skeletonFileReader.read((char*)&mSkeleton,sizeof(mSkeleton));
	SLR_ST_Skeleton sLast = mSkeleton;
	SLR_ST_Skeleton sCurrent;
	int index = 0;
	vSkeletonData.clear();
	vSkeletonData.push_back(mSkeleton);
	durTime_skeleton += clock()-start_skeleton;

	durTime_color = 0;
	start_color = clock();
	IplImage *frame = cvQueryFrame(capture);
	vColorData.push_back(cvCloneImage(frame));
	durTime_color += clock()-start_color;
	

	
	durTime_depth = 0;
	
	Mat depthMat;
	depthMat.create(480,640,CV_16UC1);
	//ushort *depthData = new ushort[640*480];
	start_depth = clock();
	ushort *depthData = new ushort[640*480*vSkeletonFrame.size()];
	depthFileReader.read((char*)depthData,640*480*vSkeletonFrame.size()*sizeof(ushort));
	durTime_depth += clock() - start_depth;
	
	if (!depthFileReader.fail())
	{
		for (int i=0; i<vSkeletonFrame.size();i++)
		{
			for(y=0; y<480; y++)
			{
				for(x=0; x<640; x++)
				{
					depthMat.at<ushort>(y,x) = depthData[640*480*i + y*640+x];
				}
			}
			vDepthData.push_back(depthMat.clone());
		}
	}
	delete[] depthData;

// 	depthFileReader.read((char*)depthData,640*480*sizeof(ushort));
// 	if( !depthFileReader.fail() )
// 	{
// 		for(y=0; y<480; y++)
// 		{
// 			for(x=0; x<640; x++)
// 			{
// 				depthMat.at<ushort>(y,x) = depthData[y*640+x];
// 			}
// 		}
// 	}	
// 	//delete [] depthData;
// 	vDepthData.push_back(depthMat.clone());
	
	int stillCount = 0;

	while(index < vSkeletonFrame.size()-1)
	{
		//读取下一帧的彩色、深度和骨架数据
		index++;
		start_color = clock();
		frame = cvQueryFrame(capture);
		vColorData.push_back(cvCloneImage(frame));
		durTime_color += clock()-start_color;
		
		
		//depthData = new ushort[640*480];
// 		depthFileReader.read((char*)depthData,640*480*sizeof(ushort));
// 		start_depth = clock();
// 		if( !depthFileReader.fail() )
// 		{
// 			for(y=0; y<480; y++)
// 			{
// 				for(x=0; x<640; x++)
// 				{
// 					depthMat.at<ushort>(y,x) = depthData[y*640+x];
// 				}
// 			}
// 		}	
// 		//delete [] depthData;
// 		vDepthData.push_back(depthMat.clone());
// 		durTime_depth += clock() - start_depth;

		start_skeleton = clock();
		skeletonFileReader.read((char*)&sCurrent,sizeof(sCurrent));
		durTime_skeleton += clock()-start_skeleton;

		//if begin, judge end time and do hand sgementation
		vSkeletonData.push_back(sCurrent);
		sLast = sCurrent;

	}

	cout<<"Color Time: "<<durTime_color<<endl;
	cout<<"Depth Time: "<<durTime_depth<<endl;
	cout<<"Skeleton Time: "<<durTime_skeleton<<endl;
	durTime=clock()-start;
	cout<<"Read Data Time:	"<<durTime<<endl;

	start=clock();

	depthFileReader.close();
	skeletonFileReader.close();
	cvReleaseCapture(&capture);
}

bool Readvideo::readColorFrame(string filename)
{
	vColorFrame.clear();
	ifstream colorFrameReader;
	const char* filePathChar=filename.c_str();
	char filePathName[100];
	strcpy(filePathName,filePathChar);
	strcat(filePathName,"\\color.frame");
	colorFrameReader.open(filePathName,ios::binary);
	if(colorFrameReader == NULL)
		return false;
	while( !colorFrameReader.eof() )
	{
		LONGLONG colorframeno;
		colorFrameReader.read((char*)&colorframeno,sizeof(LONGLONG));
		if( colorFrameReader.fail() )
			break;
		vColorFrame.push_back(colorframeno);
	}
	colorFrameReader.close();
	return true;
}

bool Readvideo::readDepthFrame(string filename)
{
	vDepthFrame.clear();
	ifstream depthFrameReader;
	const char* filePathChar=filename.c_str();
	char filePathName[100];
	strcpy(filePathName,filePathChar);
	strcat(filePathName,"\\depth.frame");
	depthFrameReader.open(filePathName,ios::binary);
	if(depthFrameReader == NULL)
		return false;
	while( !depthFrameReader.eof() )
	{
		LONGLONG depthframeno;
		depthFrameReader.read((char*)&depthframeno,sizeof(LONGLONG));
		if( depthFrameReader.fail() )
			break;
		vDepthFrame.push_back(depthframeno);
	}
	depthFrameReader.close();
	return true;
}

bool Readvideo::readSkeletonFrame(string filename)
{
	vSkeletonFrame.clear();
	ifstream skeletonFrameReader;
	const char* filePathChar=filename.c_str();
	char filePathName[100];
	strcpy(filePathName,filePathChar);
	strcat(filePathName,"\\skeleton.frame");
	skeletonFrameReader.open(filePathName,ios::binary);
	if(skeletonFrameReader == NULL)
		return false;
	while(!skeletonFrameReader.eof())
	{
		LONGLONG skeletonframeno;
		skeletonFrameReader.read((char*)&skeletonframeno,sizeof(LONGLONG));
		if( skeletonFrameReader.fail() )
			break;
		vSkeletonFrame.push_back(skeletonframeno);
	}
	skeletonFrameReader.close();
	return true;
}

Mat Readvideo::retrieveColorDepth(Mat depthMat)
{
	double maxDisp = -1.f;
	float S = 1.f;
	float V = 1.f;
	Mat disp;
	disp.create( Size(640,480), CV_32FC1);
	disp = cv::Scalar::all(0);
	for( int y = 0; y < disp.rows; y++ )
	{
		for( int x = 0; x < disp.cols; x++ )
		{
			unsigned short curDepth = depthMat.at<unsigned short>(y,x);
			if( curDepth != 0 )
				disp.at<float>(y,x) = (75.0 * 757) / curDepth;
		}
	}
	Mat gray;
	disp.convertTo( gray, CV_8UC1 );
	if( maxDisp <= 0 )
	{
		maxDisp = 0;
		minMaxLoc( gray, 0, &maxDisp );
	}
	Mat _depthColorImage;
	_depthColorImage.create( gray.size(), CV_8UC3 );
	_depthColorImage = Scalar::all(0);
	for( int y = 0; y < gray.rows; y++ )
	{
		for( int x = 0; x < gray.cols; x++ )
		{
			uchar d = gray.at<uchar>(y,x);
			if (d == 0)
				continue;

			unsigned int H = ((uchar)maxDisp - d) * 240 / (uchar)maxDisp;

			unsigned int hi = (H/60) % 6;
			float f = H/60.f - H/60;
			float p = V * (1 - S);
			float q = V * (1 - f * S);
			float t = V * (1 - (1 - f) * S);

			Point3f res;

			if( hi == 0 ) //R = V,   G = t,   B = p
				res = Point3f( p, t, V );
			if( hi == 1 ) // R = q,   G = V,   B = p
				res = Point3f( p, V, q );
			if( hi == 2 ) // R = p,   G = V,   B = t
				res = Point3f( t, V, p );
			if( hi == 3 ) // R = p,   G = q,   B = V
				res = Point3f( V, q, p );
			if( hi == 4 ) // R = t,   G = p,   B = V
				res = Point3f( V, p, t );
			if( hi == 5 ) // R = V,   G = p,   B = q
				res = Point3f( q, p, V );

			uchar b = (uchar)(std::max(0.f, std::min (res.x, 1.f)) * 255.f);
			uchar g = (uchar)(std::max(0.f, std::min (res.y, 1.f)) * 255.f);
			uchar r = (uchar)(std::max(0.f, std::min (res.z, 1.f)) * 255.f);

			_depthColorImage.at<Point3_<uchar> >(y,x) = Point3_<uchar>(b, g, r);     
		}
	}
	return _depthColorImage;
}

Mat Readvideo::retrieveGrayDepth(Mat depthMat)
{
	double maxDisp = -1.f;
	float S = 1.f;
	float V = 1.f;
	Mat disp;
	disp.create( Size(640,480), CV_32FC1);
	disp = cv::Scalar::all(0);
	for( int y = 0; y < disp.rows; y++ )
	{
		for( int x = 0; x < disp.cols; x++ )
		{
			unsigned short curDepth = depthMat.at<unsigned short>(y,x);
			if( curDepth != 0 )
				disp.at<float>(y,x) = (75.0 * 757) / curDepth;
		}
	}
	Mat gray;
	disp.convertTo( gray, CV_8UC1 );
	if( maxDisp <= 0 )
	{
		maxDisp = 0;
		minMaxLoc( gray, 0, &maxDisp );
	}
	Mat _depthColorImage;
	_depthColorImage.create( gray.size(), CV_8UC3 );
	_depthColorImage = Scalar::all(0);
	for( int y = 0; y < gray.rows; y++ )
	{
		for( int x = 0; x < gray.cols; x++ )
		{
			uchar d = gray.at<uchar>(y,x);
			if (d == 0)
				continue;

			unsigned int H = ((uchar)maxDisp - d) * 240 / (uchar)maxDisp;

// 			unsigned int hi = (H/60) % 6;
// 			float f = H/60.f - H/60;
// 			float p = V * (1 - S);
// 			float q = V * (1 - f * S);
// 			float t = V * (1 - (1 - f) * S);
// 
// 			Point3f res;
// 
// 			if( hi == 0 ) //R = V,   G = t,   B = p
// 				res = Point3f( p, t, V );
// 			if( hi == 1 ) // R = q,   G = V,   B = p
// 				res = Point3f( p, V, q );
// 			if( hi == 2 ) // R = p,   G = V,   B = t
// 				res = Point3f( t, V, p );
// 			if( hi == 3 ) // R = p,   G = q,   B = V
// 				res = Point3f( V, q, p );
// 			if( hi == 4 ) // R = t,   G = p,   B = V
// 				res = Point3f( V, p, t );
// 			if( hi == 5 ) // R = V,   G = p,   B = q
// 				res = Point3f( q, p, V );
// 
// 			uchar b = (uchar)(std::max(0.f, std::min (res.x, 1.f)) * 255.f);
// 			uchar g = (uchar)(std::max(0.f, std::min (res.y, 1.f)) * 255.f);
// 			uchar r = (uchar)(std::max(0.f, std::min (res.z, 1.f)) * 255.f);

			_depthColorImage.at<Point3_<uchar> >(y,x) = Point3_<uchar>(H, H, H);     
		}
	}
	return _depthColorImage;
}

void Readvideo::release(void)
{
	vSkeletonData.clear();

	for(int i=0;i<vDepthData.size();i++)
		vDepthData[i].release();
	vDepthData.clear();

	for(int i=0;i<vColorData.size();i++)
		cvReleaseImage(&(vColorData[i]));
	vColorData.clear();

	//////////////////////////////////////////////////////////////////////////
	//delete[] depthData;
}

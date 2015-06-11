#include "StdAfx.h"
#include "HandSegment_HMM.h"

CHandSegment_HMM::CHandSegment_HMM(void)
{

}


CHandSegment_HMM::~CHandSegment_HMM(void)
{
}

void CHandSegment_HMM::init()
{
	m_pHeadImage = NULL;
	m_pFaceNeckBiImg = NULL;

	pcaMat = cvCreateMat(SRC_FEA_NUM,DES_FEA_NUM,CV_32FC1);
	fstream fr(PCA_FILE_NAME,ios::in);
	float data;
	for(int i=0; i<SRC_FEA_NUM; i++)
	{
		for(int j=0; j<DES_FEA_NUM; j++)
		{
			fr >> data;
			cvmSet(pcaMat,i,j,data);
			//fr >> pcaMat->data.db[i*1764+j];
		}
	}
	fr.close();
}

void CHandSegment_HMM::destroy()
{
	if(m_pFaceNeckBiImg != NULL)
	{
		cvReleaseImage(&m_pFaceNeckBiImg);
	}

	if(m_pHeadImage != NULL)
	{
		cvReleaseImage(&m_pHeadImage);
	}
	//if(pcaMat)
	//{
	//	cvReleaseMat(&pcaMat);
	//}
}

double CHandSegment_HMM::getDistanceOfPoints(CvPoint point1, CvPoint point2)
{
	return sqrt(double(point1.x-point2.x)*(point1.x-point1.x) + (point1.y-point2.y)*(point1.y-point2.y));
}

void CHandSegment_HMM::kickHands(IplImage *rgbImg, Mat mDepth, bool bLeft, bool bRight, CvPoint leftPoint, CvPoint rightPoint, CvPoint& outLeftPoint, CvPoint& outRightPoint, CvPoint& outLeftCorner, CvPoint& outRightCorner,
	LONGLONG timeStamp, Posture &posture, bool bVideo)
{
	if(leftPoint.x > 640 || leftPoint.y < 0 || leftPoint.y > 480 || leftPoint.y < 0 ||
		rightPoint.x > 640 || rightPoint.x < 0 || rightPoint.y > 480 || rightPoint.y < 0)
	{
		return;
	}
	unsigned short depthValueLeft = mDepth.at<unsigned short>(leftPoint.y,leftPoint.x);
	unsigned short depthValueRight = mDepth.at<unsigned short>(rightPoint.y,rightPoint.x);

	CRect leftRect ,rightRect;
	IplImage *leftOutputImg = NULL;
	IplImage *rightOutputImg = NULL;

	//CvPoint outLeftPoint,outRightPoint;

	if(bLeft || bRight)
	{
		leftOutputImg =  kickOneHand(rgbImg,mDepth,leftPoint,leftRect,0,outLeftPoint,timeStamp,bVideo);
		rightOutputImg = kickOneHand(rgbImg,mDepth,rightPoint,rightRect,1,outRightPoint,timeStamp,bVideo);
	}

	outLeftCorner = cvPoint(leftRect.left,leftRect.top);
	outRightCorner = cvPoint(rightRect.left,rightRect.top);

	CString fileName;
	CString depthFileName;
	CString colorFileName;
	int t = clock();
	fileName.Format(".\\handImages\\hands\\%d",timeStamp);
	depthFileName.Format(".\\handImages\\depth\\%d_d.bmp",timeStamp);
	colorFileName.Format(".\\handImages\\color\\%d_c.bmp",timeStamp);
	Mat _depthMat = retrieveColorDepth(mDepth);
	IplImage thisDepth(_depthMat);
	//cvSaveImage(depthFileName,&thisDepth);
	//cvSaveImage(colorFileName,rgbImg);

	//if( (bLeft || bRight) && (leftRect&rightRect).IsRectEmpty() )
	{
		bool bTogether = true;
		bTogether = judgeTwoImageHaveUnion(leftOutputImg,leftRect,rightOutputImg,rightRect);
		if( leftOutputImg != NULL && bLeft && !bTogether)
		{
			//cvSaveImage(fileName +"_0.bmp",leftOutputImg);	
			posture.leftHandImg = cvCreateImage(cvGetSize(leftOutputImg),leftOutputImg->depth,1);
			cvCvtColor(leftOutputImg,posture.leftHandImg,CV_BGR2GRAY);
			posture.leftHandPt = cvPoint((leftRect.left+leftRect.right)/2 - leftRect.left,(leftRect.top+leftRect.bottom)/2 - leftRect.top);
		}
		else
		{
			posture.leftHandImg = NULL;
		}

		if(rightOutputImg != NULL && bRight && !bTogether)
		{
			//cvSaveImage( fileName +"_1.bmp",rightOutputImg);
			posture.rightHandImg = cvCreateImage(cvGetSize(rightOutputImg),rightOutputImg->depth,1);
			cvCvtColor(rightOutputImg,posture.rightHandImg,CV_BGR2GRAY);
			posture.rightHandPt = cvPoint((rightRect.left+rightRect.right)/2 - rightRect.left, (rightRect.top+rightRect.bottom)/2 - rightRect.top);
		}
		else
		{
			posture.rightHandImg = NULL;
		}
	}

	if( leftOutputImg )
		cvReleaseImage(&leftOutputImg);
	if( rightOutputImg )
		cvReleaseImage(&rightOutputImg);
}

void CHandSegment_HMM::kickHandsAll(IplImage* rgbImg, Mat mDepth, CvPoint leftPoint, CvPoint rightPoint, Posture & posture,CvRect &leftHand,CvRect &rightHand)
{
	if(leftPoint.x > 640 || leftPoint.y < 0 || leftPoint.y > 480 || leftPoint.y < 0 ||
		rightPoint.x > 640 || rightPoint.x < 0 || rightPoint.y > 480 || rightPoint.y < 0)
	{
		return;
	}
	unsigned short depthValueLeft = mDepth.at<unsigned short>(leftPoint.y,leftPoint.x);
	unsigned short depthValueRight = mDepth.at<unsigned short>(rightPoint.y,rightPoint.x);

	CRect leftRect ,rightRect;
	IplImage *leftOutputImg = NULL;
	IplImage *rightOutputImg = NULL;


	leftOutputImg =  kickOneHandAll(rgbImg,mDepth,leftPoint,leftHand);
	rightOutputImg = kickOneHandAll(rgbImg,mDepth,rightPoint,rightHand);


	if( leftOutputImg != NULL)
	{	
		posture.leftHandImg = cvCreateImage(cvGetSize(leftOutputImg),leftOutputImg->depth,1);
		cvCvtColor(leftOutputImg,posture.leftHandImg,CV_BGR2GRAY);
	}
	else
	{
		posture.leftHandImg = NULL;
	}

	if(rightOutputImg != NULL)
	{
		posture.rightHandImg = cvCreateImage(cvGetSize(rightOutputImg),rightOutputImg->depth,1);
		cvCvtColor(rightOutputImg,posture.rightHandImg,CV_BGR2GRAY);
	}
	else
	{
		posture.rightHandImg = NULL;
	}

	if( leftOutputImg )
		cvReleaseImage(&leftOutputImg);
	if( rightOutputImg )
		cvReleaseImage(&rightOutputImg);
}

IplImage* CHandSegment_HMM::kickOneHandAll(IplImage* img, Mat depthMat, CvPoint point,CvRect &HandRegion)
{
	int y,x;
	bool bDepthUsed = true;
	unsigned short depthValue = depthMat.at<unsigned short>(point.y,point.x);
	if( 0 == depthValue )
	{
		bDepthUsed = false;
	}
	int skinWidth = 30;
	int handWidth = 30;
	int left,right,top,bottom;
	left = max(point.x-handWidth,0);
	top = max(point.y-handWidth,0);
	right = min(point.x+handWidth, 640);
	bottom = min(point.y+handWidth,480);
	CvRect handCvRect = cvRect(left,top,right-left,bottom-top);
	IplImage *handImg = getROIImage(img,handCvRect);

	unsigned short minDepth = 0xffff;
	CvPoint minPoint = cvPoint(0,0);

#ifdef OUTPUT_TEMP_IMAGE
	CString strFlag;
	strFlag.Format("_%d",flag);
	IplImage *oriImg1 = cvCreateImage(cvGetSize(handImg),handImg->depth,handImg->nChannels);
	cvCopy(handImg,oriImg1);
	cvCircle(oriImg1,cvPoint(handImg->width/2,handImg->height/2),3,cvScalar(0,0,255));
	cvSaveImage(fileName + "_1_T" + strFlag + ".bmp",oriImg1);
	cvReleaseImage(&oriImg1);
	oriImg1 = NULL;
#endif

	unsigned char r,g,b;
	double curCr,curCb;
	double cr,cb;

	//查找肤色部分深度最小值作为估计手部
	for(y=0; y<handCvRect.height; y++)
	{
		for(x=0; x<handCvRect.width; x++)
		{
			r = (handImg->imageData + handImg->widthStep*y)[x*3+2];
			g = (handImg->imageData + handImg->widthStep*y)[x*3+1];
			b = (handImg->imageData + handImg->widthStep*y)[x*3+0];
			cr = 0.5000*r - 0.4187*g - 0.0813*b + 128; // cr
			cb = -0.1687*r - 0.3313*g + 0.5000*b + 128; // cb
			unsigned short tempDepth = depthMat.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
			if( fabs(faceModel.mean_cr-cr) > 3*faceModel.d_cr ||
				fabs(faceModel.mean_cb-cb) > 3*faceModel.d_cb )
			{
				(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
				(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
				(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
			}
			else
			{
				if( tempDepth < minDepth && tempDepth != 0 )
				{
					minPoint = cvPoint(x,y);
					minDepth = tempDepth;
				}
			}
		}
	}

#ifdef OUTPUT_TEMP_IMAGE
	IplImage *oriImg2= cvCreateImage(cvGetSize(handImg),handImg->depth,handImg->nChannels);
	cvCopy(handImg,oriImg2);
	//cvCircle(oriImg2,cvPoint(handImg->width/2,handImg->height/2),1,cvScalar(0,0,255));
	cvSaveImage(fileName + "_2_skin" + strFlag + ".bmp",oriImg2);
	cvReleaseImage(&oriImg2);
	oriImg2 = NULL;
#endif

	//CString filePath2 = resultDir + m_OniName + "\\temp\\" + fileName + strFlag + "_color_tmep.bmp";
	//根据深度最小值进行深度限制
	for(y=0; y<handCvRect.height; y++)
	{
		for(x=0; x<handCvRect.width; x++)
		{
			unsigned short tempDepth = depthMat.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
			if( tempDepth > minDepth + 50)
			{
				(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
				(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
				(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
			}
		}
	}


#ifdef OUTPUT_TEMP_IMAGE
	IplImage *oriImg3= cvCreateImage(cvGetSize(handImg),handImg->depth,handImg->nChannels);
	cvCopy(handImg,oriImg3);
	//cvCircle(oriImg3,cvPoint(handImg->width/2,handImg->height/2),1,cvScalar(0,0,255));
	cvSaveImage(fileName + "_3_skin_depth" + strFlag + ".bmp",oriImg3);
	cvReleaseImage(&oriImg3);
	oriImg3 = NULL;
#endif
	
	//CString filePath1 = resultDir + m_OniName + "\\temp\\" + fileName + strFlag + "_temp.bmp";

	//获得肤色检测的最大连通域，求取该连通域中的最小深度，作为估计的手部
	IplImage *biImg = cvCreateImage(cvGetSize(handImg),8,1);
	cvCvtColor(handImg,biImg,CV_BGR2GRAY);
	cvThreshold(biImg,biImg,0,255,CV_THRESH_BINARY);

	cvErode(biImg,biImg,NULL,1);
	cvDilate(biImg,biImg,NULL,1);

	IplImage* resultImg = getConnexeImage(biImg);

	double minDepth2 = 0xffff;
	CvPoint minPoint2 = cvPoint(0,0);
	for(y=0; y<resultImg->height; y++)
	{
		for(x=0; x<resultImg->width; x++)
		{
			if( (resultImg->imageData + resultImg->widthStep*y)[x] == 0)
			{
				(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
				(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
				(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
			}
			else
			{
				unsigned short tempDepth = depthMat.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
				if( tempDepth < minDepth2 && tempDepth != 0 )
				{
					minPoint2 = cvPoint(x,y);
					minDepth2 = tempDepth;
				}
			}
		}
	}
	cvReleaseImage(&biImg);
	cvReleaseImage(&resultImg);

	//根据检测到的手部，更新肤色模型，确定手部的位置minPoint
	ColorModel colorModel;
	getColorModel(handImg,colorModel);
	if(colorModel.mean_cb == 0)
	{
		colorModel.mean_cb = faceModel.mean_cb;
		colorModel.mean_cr = faceModel.mean_cr;
		colorModel.d_cb = faceModel.d_cb;
		colorModel.d_cr = faceModel.d_cr;

		minPoint = cvPoint(point.x-handCvRect.x, point.y-handCvRect.y);
		return NULL;
	}
	else if(minDepth2 != 0xffff)
	{
		minDepth = minDepth2;
		minPoint = minPoint2;
	}

	CvPoint outPoint = cvPoint(handCvRect.x+minPoint.x, handCvRect.y+minPoint.y);

	unsigned char c_r,c_g,c_b;
	c_r = (handImg->imageData + handImg->widthStep*minPoint.y)[minPoint.x*3+2];
	c_g = (handImg->imageData + handImg->widthStep*minPoint.y)[minPoint.x*3+1];
	c_b = (handImg->imageData + handImg->widthStep*minPoint.y)[minPoint.x*3+0]; 
	double m_cr,m_cb;
	m_cr = 0.5000*c_r - 0.4187*c_g - 0.0813*c_b + 128; // cr
	m_cb = -0.1687*c_r - 0.3313*c_g + 0.5000*c_b + 128; // cb

	int addWidth = 30; //扩大估计手部的大小
	left = max(outPoint.x-handWidth-addWidth,0);
	top = max(outPoint.y-handWidth-addWidth,0);
	right = min(outPoint.x + handWidth+addWidth, 640);
	bottom = min(outPoint.y + handWidth+addWidth, 480);
	handCvRect = cvRect(left,top,right-left,bottom-top);
	cvReleaseImage(&handImg);
	handImg =getROIImage(img,handCvRect);

#ifdef OUTPUT_TEMP_IMAGE
	IplImage *oriImg4= cvCreateImage(cvGetSize(handImg),handImg->depth,handImg->nChannels);
	cvCopy(handImg,oriImg4);
	cvCircle(oriImg4,cvPoint(handImg->width/2,handImg->height/2),3,cvScalar(0,0,255));
	cvSaveImage(fileName + "_4_point" + strFlag + ".bmp",oriImg4);
	cvReleaseImage(&oriImg4);
	oriImg4 = NULL;
#endif
	/*
	//颜色、深度限制可以根据具体情况而决定参数的设置
	//如果用于少数数据采集、选择菜单操作，手部保持深度在最前面，则可以考虑放宽颜色限制的阈值，增大深度限制程度
	//如果用于采集完整手语数据的分割操作，根据具体数据情况调整阈值参数
	*/

	//根据肤色模型分割手部
	for(y=0; y<handCvRect.height; y++)
	{
		for(x=0; x<handCvRect.width; x++)
		{
			r = (handImg->imageData + handImg->widthStep*y)[x*3+2];
			g = (handImg->imageData + handImg->widthStep*y)[x*3+1];
			b = (handImg->imageData + handImg->widthStep*y)[x*3+0];
			cr = 0.5000*r - 0.4187*g - 0.0813*b + 128; // cr
			cb = -0.1687*r - 0.3313*g + 0.5000*b + 128; // cb
			//unsigned short tempDepth = depthMat.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
			//dis表示手心点Cr、Cb值与其它点之间的距离，阈值取16
			double dis = sqrt((colorModel.mean_cr-cr)*(colorModel.mean_cr-cr) + (colorModel.mean_cb-cb)*(colorModel.mean_cb-cb));
			double disCr = fabs(colorModel.mean_cr-cr);
			double disCb = fabs(colorModel.mean_cb-cb);
			double dis2 = sqrt((m_cr-cr)*(m_cr-cr) + (m_cb-cb)*(m_cb-cb));
			if( dis > 16 || cb >= 128 || cr <= 143 || disCr > 12 || disCb > 12)
			{
				(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
				(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
				(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
			}
		}
	}
	//当估计手部区域中存在脸部时，进行深度限制
	for(y=0; y<handCvRect.height; y++)
	{
		for(x=0; x<handCvRect.width; x++)
		{
			CvPoint tempPoint = cvPoint(x+handCvRect.x, y+handCvRect.y);
			unsigned short tempDepth = depthMat.at<unsigned short>(y+handCvRect.y, x+handCvRect.x);
			if( judgePointInRect(tempPoint,m_faceNeckRect) )
			{
				unsigned short oldDepth = m_depthCopyMat.at<unsigned short>(y+handCvRect.y, x+handCvRect.x); 
				if( abs(minDepth-m_faceDepthMin) > 200 )
				{
					if(	tempDepth >= m_faceDepthMin ) //|| tempDepth == 0 ) //abs(tempDepth - oldDepth) < 10 )// || tempDepth == 0)
					{
						(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
						(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
						(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
					}
					if((m_pFaceNeckBiImg->imageData + m_pFaceNeckBiImg->widthStep*(tempPoint.y-m_faceNeckRect.top))[tempPoint.x-m_faceNeckRect.left] != 0 && tempDepth == 0)
					{
						(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
						(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
						(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
					}
				}
				else if( (m_pFaceNeckBiImg->imageData + m_pFaceNeckBiImg->widthStep*(tempPoint.y-m_faceNeckRect.top))[tempPoint.x-m_faceNeckRect.left] != 0 )
				{
					if(tempDepth >= m_faceDepthMin || tempDepth == 0)
					{
						(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
						(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
						(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
					}
				}
			}
		}
	}
	
#ifdef OUTPUT_TEMP_IMAGE
		IplImage *oriImg5= cvCreateImage(cvGetSize(handImg),handImg->depth,handImg->nChannels);
		cvCopy(handImg,oriImg5);
		//cvCircle(oriImg3,cvPoint(handImg->width/2,handImg->height/2),1,cvScalar(0,0,255));
		cvSaveImage(fileName + "_5_skin_depth" + strFlag + ".bmp",oriImg5);
		cvReleaseImage(&oriImg5);
		oriImg5 = NULL;
#endif

	IplImage *binaryImg = cvCreateImage(cvSize(handImg->width,handImg->height),8,1);
	IplImage *tempBinImg = cvCreateImage(cvGetSize(binaryImg),8,1);
	cvCvtColor(handImg,binaryImg,CV_BGR2GRAY);
	cvThreshold(binaryImg,binaryImg,0,255,CV_THRESH_BINARY);
	cvCopy(binaryImg,tempBinImg);

	//手部填充，选择8邻域
	for(y=1; y<handCvRect.height-1; y++)
	{
		for(x=1; x<handCvRect.width-1; x++)
		{
			int b = (int)((binaryImg->imageData + binaryImg->widthStep*y)[x]);
			if(b == 0)
			{
				int neighbor =  (int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x-1]) + 
								(int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x])   + 
								(int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x+1]) + 
								(int)((binaryImg->imageData + binaryImg->widthStep*(y))[x-1])   + 
								(int)((binaryImg->imageData + binaryImg->widthStep*(y))[x+1])   + 
								(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x-1]) + 
								(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x])   + 
								(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x+1]) ;
				if(neighbor <= -3)
					(tempBinImg->imageData + tempBinImg->widthStep*y)[x] = 255;
			}
		}
	}

	cvReleaseImage(&binaryImg);
	binaryImg = cvCreateImage(cvGetSize(handImg),8,1);
	cvCopy(tempBinImg,binaryImg);
	cvReleaseImage(&tempBinImg);

	int *theBox = new int[4];
	int *theCent = new int[2];
	int nThreshold = 30;
	int nMaxRect = 0;
	
	//进行膨胀腐蚀操作，去除边缘信息
	IplConvKernel *strel = cvCreateStructuringElementEx(5,5,2,2,CV_SHAPE_CROSS);
	cvErode(binaryImg,binaryImg,strel,1);
	cvDilate(binaryImg,binaryImg,strel,1);
	cvReleaseStructuringElement(&strel);

	//获得最大连通域，即分割后的手部
	getConnexeCenterBox(binaryImg,nMaxRect,theCent,theBox,nThreshold);
	IplImage *outputImg = NULL;
	if(nMaxRect > 0)
	{
		CvPoint lt = cvPoint(theBox[0],theBox[1]);
		CvPoint rb = cvPoint(theBox[2],theBox[3]);

		cvReleaseImage(&handImg);
		handImg =getROIImage(img,handCvRect);

#ifndef GrayHandImage
		getForeImage(handImg,binaryImg);
#endif
		outputImg = getROIImage(handImg,cvRect(theBox[0],theBox[1],theBox[2]-theBox[0],theBox[3]-theBox[1]));

		HandRegion.x = handCvRect.x + theBox[0];
		HandRegion.y = handCvRect.y + theBox[1];
		HandRegion.width = theBox[2]-theBox[0];
		HandRegion.height = theBox[3]-theBox[1];

	}
	delete [] theBox;
	delete [] theCent;
	cvReleaseImage(&handImg);
	cvReleaseImage(&binaryImg);


#ifdef OUTPUT_TEMP_IMAGE
	cvSaveImage(fileName + "_6_result" + strFlag + ".bmp",outputImg);
#endif


	return outputImg;
}

bool CHandSegment_HMM::headDetection(IplImage* colorImage, Mat mDepth, CvPoint headCenter)
{

    //JDL Face Detection
// 	JFDetector *m_pFaceDetector = NULL;
// 	char* faceClassifierPath = "..\\input";
// 	if(m_pFaceDetector == NULL)
// 	{
// 		m_pFaceDetector = new JFDetector();
// 		m_pFaceDetector->SetClassifiersPath(faceClassifierPath);
// 		m_pFaceDetector->FDInitialize();
// 		m_pFaceDetector->SetSpeedAccuracyMode(14);
// 		m_pFaceDetector->SetInputBPP(Gray8);
// 		//m_pFaceDetector->SetPanClassifier(PL_1_30_degree);
// 		// m_pFaceDetector->SetRollClassifier(RL_1_20_degree);
// 	}
// 	
// 	CRect partRect = CRect(
// 		max<int>(0, headCenter.x - 100),
// 		max<int>(0, headCenter.y - 100),
// 		min<int>(640, headCenter.x + 100),
// 		min<int>(480, headCenter.y + 100)
// 		);
// 	IplImage *partImg = getROIImage(colorImage,cvRect(partRect.left,partRect.top,partRect.Width(),partRect.Height()));
// 	IplImage *grayImage = cvCreateImage(cvSize(partImg->width,partImg->height),partImg->depth,1);
// 	cvCvtColor(partImg,grayImage,CV_BGR2GRAY);
// 	
// 	bool bFound = false;
// 	if(grayImage)
// 	{
// 		int width = grayImage->width;
// 		int height = grayImage->height;
// 		m_pFaceDetector->SetInputResolusion(width,height);
// 		int faceNum = 0;
// 		m_pFaceDetector->FDFindFaces((const unsigned char*)grayImage->imageData,&faceNum);
// 		vector<FACEINFO> vFaceInfo;
// 		double minDis;
// 		if(faceNum > 0)
// 		{
// 			vFaceInfo.resize(faceNum);
// 			bFound = true;
// 			m_pFaceDetector->FDGetFacesInfo(&vFaceInfo[0],faceNum);
// 	
// 			CRect tempFaceRect;
// 			tempFaceRect = CRect( vFaceInfo[0].nLeft,
// 				vFaceInfo[0].nTop,
// 				vFaceInfo[0].nLeft + vFaceInfo[0].nWidth,
// 				vFaceInfo[0].nTop + vFaceInfo[0].nHeight);
// 			minDis = getDistanceOfPoints(cvPoint(headCenter.x-partRect.left,headCenter.y-partRect.top),cvPoint((tempFaceRect.left+tempFaceRect.right)/2,(tempFaceRect.top+tempFaceRect.bottom)/2));
// 			for(int h=1; h<faceNum; h++)
// 			{
// 				CRect tempRect = CRect( vFaceInfo[h].nLeft,
// 					vFaceInfo[h].nTop,
// 					vFaceInfo[h].nLeft + vFaceInfo[h].nWidth,
// 					vFaceInfo[h].nTop + vFaceInfo[h].nHeight);
// 				double dis = getDistanceOfPoints(cvPoint(headCenter.x-partRect.left,headCenter.y-partRect.top),cvPoint((tempRect.left+tempRect.right)/2,(tempRect.top+tempRect.bottom)/2));
// 				if(dis < minDis) //if(tempRect.top < m_faceRect.top)
// 					tempFaceRect = tempRect;
// 			}
// 			m_faceRect = CRect(tempFaceRect.left + partRect.left, tempFaceRect.top + partRect.top, tempFaceRect.right + partRect.left, tempFaceRect.bottom + partRect.top);
// 	
// 			m_pHeadImage = getROIImage(colorImage,cvRect(m_faceRect.left,m_faceRect.top,m_faceRect.Width(),m_faceRect.Height()));	
// 	
// 			//获得人脸的深度
// 			m_faceDepthMin = 0xFFFF;
// 			int i,j;
// 			for(j=m_faceRect.top; j<m_faceRect.bottom; j++)
// 			{
// 				for(i=m_faceRect.left; i<m_faceRect.right; i++)
// 				{
// 					if(mDepth.at<unsigned short>(j,i) < m_faceDepthMin && mDepth.at<unsigned short>(j,i) != 0)
// 						m_faceDepthMin = mDepth.at<unsigned short>(j,i) ;
// 				}
// 			}
// 		}
// 		else
// 		{
// 			bFound = false;
// 		}
// 	}
// 	cvReleaseImage(&partImg);
// 	cvReleaseImage(&grayImage);
// 	grayImage = NULL;
// 	delete m_pFaceDetector;
// 	m_pFaceDetector = NULL;
// 	
// 	return bFound;
	return false;

}

bool CHandSegment_HMM::headDetectionVIPLSDK(IplImage* colorImage, Mat mDepth, CvPoint headCenter)
{//head detection,if true, initial m_pHeadImage, m_faceRect,m_faceDepthMin
	m_pHeadImage = NULL;



	CRect partRect = CRect(
		max<int>(0, headCenter.x - 100),
		max<int>(0, headCenter.y - 100),
		min<int>(640, headCenter.x + 100),
		min<int>(480, headCenter.y + 100)
		);
	IplImage *partImg = getROIImage(colorImage,cvRect(partRect.left,partRect.top,partRect.Width(),partRect.Height()));
	IplImage *grayImage = cvCreateImage(cvSize(partImg->width,partImg->height),partImg->depth,1);
	cvCvtColor(partImg,grayImage,CV_BGR2GRAY);




	VIPLFACEDETECTOR faceDetector;
	char* faceClassifierPath = "..\\input";
	bool hr = FDInitialize(faceClassifierPath, PL_1_30_degree, RL_1_20_degree, TL_2_40_degree, 0.7);

	bool bFound = false;
	if (hr)
	{
		
		faceDetector = CreateFaceDetector();
		if (faceDetector != NULL)
		{
			int width = grayImage->width;
			int height = grayImage->height;
			int detectedFaceNum = 0;
			FACEINFO* vFaceInfo = new FACEINFO[250];
			SetInputResolusion(faceDetector, width, height);
			FDFindFaces(faceDetector, (const unsigned char*)grayImage->imageData, vFaceInfo, detectedFaceNum);

			double minDis;
			
			if(detectedFaceNum > 0)
			{
				bFound = true;
				CRect tempFaceRect;
				tempFaceRect = CRect( vFaceInfo[0].nLeft,
					vFaceInfo[0].nTop,
					vFaceInfo[0].nLeft + vFaceInfo[0].nWidth,
					vFaceInfo[0].nTop + vFaceInfo[0].nHeight);
				minDis = getDistanceOfPoints(cvPoint(headCenter.x-partRect.left,headCenter.y-partRect.top),
					cvPoint((tempFaceRect.left+tempFaceRect.right)/2,(tempFaceRect.top+tempFaceRect.bottom)/2));
				for(int h=1; h<detectedFaceNum; h++)
				{
					CRect tempRect = CRect( vFaceInfo[h].nLeft,
						vFaceInfo[h].nTop,
						vFaceInfo[h].nLeft + vFaceInfo[h].nWidth,
						vFaceInfo[h].nTop + vFaceInfo[h].nHeight);
					double dis = getDistanceOfPoints(cvPoint(headCenter.x-partRect.left,headCenter.y-partRect.top),
						cvPoint((tempRect.left+tempRect.right)/2,(tempRect.top+tempRect.bottom)/2));
					if(dis < minDis) //if(tempRect.top < m_faceRect.top)
						tempFaceRect = tempRect;
				}
				m_faceRect = CRect(tempFaceRect.left + partRect.left, 
					tempFaceRect.top + partRect.top, 
					tempFaceRect.right + partRect.left, 
					tempFaceRect.bottom + partRect.top);
	
				m_pHeadImage = getROIImage(colorImage,
					cvRect(m_faceRect.left,m_faceRect.top,m_faceRect.Width(),m_faceRect.Height()));	
	
				//Obtain the depth of human face
				m_faceDepthMin = 0xFFFF;
				int i,j;
				for(j=m_faceRect.top; j<m_faceRect.bottom; j++)
				{
					for(i=m_faceRect.left; i<m_faceRect.right; i++)
					{
						if(mDepth.at<unsigned short>(j,i) < m_faceDepthMin && mDepth.at<unsigned short>(j,i) != 0)
							m_faceDepthMin = mDepth.at<unsigned short>(j,i) ;
					}
				}
			}
			else
			{
				bFound = false;
			}
		}
		else
		{
			bFound = false;
		}
		ReleaseFaceDetector(faceDetector);
	}
	else
	{
		bFound = false;
	}

	FDClose();

	if (bFound == false)
	{
		m_faceRect = partRect;
		m_pHeadImage = getROIImage(colorImage,
			cvRect(m_faceRect.left,m_faceRect.top,m_faceRect.Width(),m_faceRect.Height()));	
		//Obtain the depth of human face
		m_faceDepthMin = 0xFFFF;
		int i,j;
		for(j=m_faceRect.top; j<m_faceRect.bottom; j++)
		{
			for(i=m_faceRect.left; i<m_faceRect.right; i++)
			{
				if(mDepth.at<unsigned short>(j,i) < m_faceDepthMin && mDepth.at<unsigned short>(j,i) != 0)
					m_faceDepthMin = mDepth.at<unsigned short>(j,i) ;
			}
		}

		bFound = true;
	}

	return bFound;

	
}

bool CHandSegment_HMM::headDetectionByOpenCV(IplImage* colorImage, Mat mDepth, CvPoint headCenter)
{
	CvMemStorage *storage = 0;
	CvHaarClassifierCascade *cascade = 0;
	char* cascade_name = "haarcascade_frontalface_alt.xml";

	cascade = (CvHaarClassifierCascade *)cvLoad(cascade_name,0,0,0);
	if(!cascade)
	{
		return false;
	}

	storage = cvCreateMemStorage(0);

	CvScalar colors[] = { 
		{{0,0,255}},{{0,128,255}},{{0,255,255}},{{0,255,0}}, 
		{{255,128,0}},{{255,255,0}},{{255,0,0}},{{255,0,255}} 
	};//Just some pretty colors to draw with

	CRect partRect = CRect(
		max<int>(0, headCenter.x - 100),
		max<int>(0, headCenter.y - 100),
		min<int>(640, headCenter.x + 100),
		min<int>(480, headCenter.y + 100)
		);
	IplImage *partImg = getROIImage(colorImage,cvRect(partRect.left,partRect.top,partRect.Width(),partRect.Height()));
	IplImage *grayImage = cvCreateImage(cvSize(partImg->width,partImg->height),partImg->depth,1);
	cvCvtColor(partImg,grayImage,CV_BGR2GRAY);

	cvEqualizeHist(grayImage,grayImage);

	cvClearMemStorage(storage);

	CvSeq *objects = cvHaarDetectObjects(grayImage,cascade,storage,1.1,2,0,cvSize(30,30));
	for(int i=0; i<objects->total; i++)
	{

		CvRect *tempFaceRect = (CvRect*)cvGetSeqElem(objects,i);

		m_faceRect = CRect(tempFaceRect->x + partRect.left, tempFaceRect->y + partRect.top, tempFaceRect->x + tempFaceRect->width + partRect.left, tempFaceRect->y + tempFaceRect->height + partRect.top);

		m_pHeadImage = getROIImage(colorImage,cvRect(m_faceRect.left,m_faceRect.top,m_faceRect.Width(),m_faceRect.Height()));	

		//获得人脸的深度
		m_faceDepthMin = 0xFFFF;
		int h,w;
		for(h=m_faceRect.top; h<m_faceRect.bottom; h++)
		{
			for(w=m_faceRect.left; w<m_faceRect.right; w++)
			{
				if(mDepth.at<unsigned short>(h,w) < m_faceDepthMin && mDepth.at<unsigned short>(h,w) != 0)
					m_faceDepthMin = mDepth.at<unsigned short>(h,w) ;
			}
		}
	}

	cvReleaseImage(&partImg);
	cvReleaseImage(&grayImage);
	partImg = NULL;
	grayImage = NULL;
	if(objects->total == 0)
		return false;
	return true;
}

IplImage* CHandSegment_HMM::kickOneHand(IplImage* rgbImg, Mat mDepth, CvPoint point, CRect& rect, int flag, CvPoint& outPoint,LONGLONG timeStamp,bool bVideo)
{
	bool bDepthUsed = true;
	unsigned short depthValue = mDepth.at<unsigned short>(point.y,point.x);
	if( 0 == depthValue )
	{
		bDepthUsed = false;
	}
	int skinWidth = 30;
	int handWidth = 30;
	int left,right,top,bottom;
	CvRect handCvRect;
	IplImage *handImg = NULL;

	unsigned char r,g,b;
	double cr,cb;
	int y,x;

	if(bVideo)
	{
		left = max(point.x-handWidth,0);
		top = max(point.y-handWidth,0);
		right = min(point.x+handWidth, 640);
		bottom = min(point.y+handWidth,480);
		handCvRect = cvRect(left,top,right-left,bottom-top);
		handImg = getROIImage(rgbImg,handCvRect);

		unsigned short minDepth = 0xffff;
		CvPoint minPoint = cvPoint(0,0);

		for(y=0; y<handCvRect.height; y++)
		{
			for(x=0; x<handCvRect.width; x++)
			{
				r = (handImg->imageData + handImg->widthStep*y)[x*3+2];
				g = (handImg->imageData + handImg->widthStep*y)[x*3+1];
				b = (handImg->imageData + handImg->widthStep*y)[x*3+0];
				cr = 0.5000*r - 0.4187*g - 0.0813*b + 128; // cr
				cb = -0.1687*r - 0.3313*g + 0.5000*b + 128; // cb
				unsigned short tempDepth = mDepth.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
				if( fabs(faceModel.mean_cr-cr) > 3*faceModel.d_cr ||
					fabs(faceModel.mean_cb-cb) > 3*faceModel.d_cb )
				{
					(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
				}
				else
				{
					if( tempDepth < minDepth && tempDepth != 0 )
					{
						minPoint = cvPoint(x,y);
						minDepth = tempDepth;
					}
				}
			}
		}
		for(y=0; y<handCvRect.height; y++)
		{
			for(x=0; x<handCvRect.width; x++)
			{
				unsigned short tempDepth = mDepth.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);

				if( tempDepth > minDepth + 50)
				{
					(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
				}
			}
		}


		IplImage *biImg = cvCreateImage(cvGetSize(handImg),8,1);
		cvCvtColor(handImg,biImg,CV_BGR2GRAY);
		cvThreshold(biImg,biImg,0,255,CV_THRESH_BINARY);

		cvErode(biImg,biImg,NULL,1);
		cvDilate(biImg,biImg,NULL,1);

		IplImage* resultImg = getConnexeImage(biImg);
		double minDepth2 = 0xffff;
		CvPoint minPoint2 = cvPoint(0,0);
		for(y=0; y<resultImg->height; y++)
		{
			for(x=0; x<resultImg->width; x++)
			{
				if( (resultImg->imageData + resultImg->widthStep*y)[x] == 0)
				{
					(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
				}
				else
				{
					unsigned short tempDepth = mDepth.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
					if( tempDepth < minDepth2 && tempDepth != 0 )
					{
						minPoint2 = cvPoint(x,y);
						minDepth2 = tempDepth;
					}
				}
			}
		}

		int t = clock();
		CString st;
		st.Format(".\\handImages\\%d.bmp",t);
		//cvSaveImage(st,handImg);

		cvReleaseImage(&biImg);
		cvReleaseImage(&resultImg);

		ColorModel colorModel;
		getColorModel(handImg,colorModel);
		if(colorModel.mean_cb == 0)
		{
			colorModel.mean_cb = faceModel.mean_cb;
			colorModel.mean_cr = faceModel.mean_cr;
			colorModel.d_cb = faceModel.d_cb;
			colorModel.d_cr = faceModel.d_cr;

			minPoint = cvPoint(point.x-handCvRect.x, point.y-handCvRect.y);
			return NULL;
		}
		else if(minDepth2 != 0xffff)
		{
			minDepth = minDepth2;
			minPoint = minPoint2;
		}

		//minPoint = cvPoint(point.x-handCvRect.x, point.y-handCvRect.y);
		outPoint = cvPoint(handCvRect.x+minPoint.x, handCvRect.y+minPoint.y);

		unsigned char c_r,c_g,c_b;
		c_r = (handImg->imageData + handImg->widthStep*minPoint.y)[minPoint.x*3+2];
		c_g = (handImg->imageData + handImg->widthStep*minPoint.y)[minPoint.x*3+1];
		c_b = (handImg->imageData + handImg->widthStep*minPoint.y)[minPoint.x*3+0]; 
		double m_cr,m_cb;
		m_cr = 0.5000*c_r - 0.4187*c_g - 0.0813*c_b + 128; // cr
		m_cb = -0.1687*c_r - 0.3313*c_g + 0.5000*c_b + 128; // cb

		left = max(outPoint.x-handWidth-10,0);
		top = max(outPoint.y-handWidth-10,0);
		right = min(outPoint.x + handWidth+10, 640);
		bottom = min(outPoint.y + handWidth+10, 480);
		handCvRect = cvRect(left,top,right-left,bottom-top);
		cvReleaseImage(&handImg);
		handImg = getROIImage(rgbImg,handCvRect);

		for(y=0; y<handCvRect.height; y++)
		{
			for(x=0; x<handCvRect.width; x++)
			{
				r = (handImg->imageData + handImg->widthStep*y)[x*3+2];
				g = (handImg->imageData + handImg->widthStep*y)[x*3+1];
				b = (handImg->imageData + handImg->widthStep*y)[x*3+0];
				cr = 0.5000*r - 0.4187*g - 0.0813*b + 128; // cr
				cb = -0.1687*r - 0.3313*g + 0.5000*b + 128; // cb
				unsigned short tempDepth = mDepth.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
				double dis = sqrt((colorModel.mean_cr-cr)*(colorModel.mean_cr-cr) + (colorModel.mean_cb-cb)*(colorModel.mean_cb-cb));
				double dis2 = sqrt((m_cr-cr)*(m_cr-cr) + (m_cb-cb)*(m_cb-cb));
				if( dis > 16  || cb >= 128)
				{
					(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
				}
			}
		}
		for(y=0; y<handCvRect.height; y++)
		{
			for(x=0; x<handCvRect.width; x++)
			{
				CvPoint tempPoint = cvPoint(x+handCvRect.x, y+handCvRect.y);
				unsigned short tempDepth = mDepth.at<unsigned short>(y+handCvRect.y, x+handCvRect.x);
				if( judgePointInRect(tempPoint,m_faceNeckRect) )
				{
					unsigned short oldDepth = m_depthCopyMat.at<unsigned short>(y+handCvRect.y, x+handCvRect.x); 
					if( abs(minDepth-m_faceDepthMin) > 200 )
					{
						if(	tempDepth >= m_faceDepthMin ) //|| tempDepth == 0 ) //abs(tempDepth - oldDepth) < 10 )// || tempDepth == 0)
						{
							(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
							(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
							(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
						}
						if((m_pFaceNeckBiImg->imageData + m_pFaceNeckBiImg->widthStep*(tempPoint.y-m_faceNeckRect.top))[tempPoint.x-m_faceNeckRect.left] != 0 && tempDepth == 0)
						{
							(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
							(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
							(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
						}
					}
					else if( (m_pFaceNeckBiImg->imageData + m_pFaceNeckBiImg->widthStep*(tempPoint.y-m_faceNeckRect.top))[tempPoint.x-m_faceNeckRect.left] != 0 )
					{
						if(tempDepth >= m_faceDepthMin || tempDepth == 0)
						{
							(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
							(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
							(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
						}
					}
				}
			}
		}
	}
	else
	{
		//静态手势识别
		if(depthValue > m_faceDepthMin - 200)
		{
			cvReleaseImage(&handImg);
			return NULL;
		}
		outPoint = point;
		left = max(outPoint.x-handWidth-70,0);
		top = max(outPoint.y-handWidth-70,0);
		right = min(outPoint.x + handWidth+70, 640);
		bottom = min(outPoint.y + handWidth+70, 480);
		handCvRect = cvRect(left,top,right-left,bottom-top);
		cvReleaseImage(&handImg);
		handImg = getROIImage(rgbImg,handCvRect);

		for(y=0; y<handCvRect.height; y++)
		{
			for(x=0; x<handCvRect.width; x++)
			{
				r = (handImg->imageData + handImg->widthStep*y)[x*3+2];
				g = (handImg->imageData + handImg->widthStep*y)[x*3+1];
				b = (handImg->imageData + handImg->widthStep*y)[x*3+0];
				cr = 0.5000*r - 0.4187*g - 0.0813*b + 128; // cr
				cb = -0.1687*r - 0.3313*g + 0.5000*b + 128; // cb
				unsigned short tempDepth = mDepth.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
				if( fabs(faceModel.mean_cr-cr) > 5*faceModel.d_cr ||
					fabs(faceModel.mean_cb-cb) > 5*faceModel.d_cb ||
					tempDepth >= depthValue + 150 ||
					tempDepth == 0)
				{
					(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
				}

				//if(	tempDepth >= minDepth + 150 || tempDepth == 0 ) //|| tempDepth == 0 ) //abs(tempDepth - oldDepth) < 10 )// || tempDepth == 0)
				//{
				//	(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
				//	(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
				//	(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
				//}

			}
		}

		int t = clock();
		CString st;
		st.Format(".\\handImages\\%d.bmp",t);
		if(flag == 1)
		{
			//cvSaveImage(st,handImg);
		}
	}



	IplImage *binaryImg = cvCreateImage(cvSize(handImg->width,handImg->height),8,1);
	IplImage *tempBinImg = cvCreateImage(cvGetSize(binaryImg),8,1);
	cvCvtColor(handImg,binaryImg,CV_BGR2GRAY);
	cvThreshold(binaryImg,binaryImg,0,255,CV_THRESH_BINARY);
	cvCopy(binaryImg,tempBinImg);

	for(y=1; y<handCvRect.height-1; y++)
	{
		for(x=1; x<handCvRect.width-1; x++)
		{
			int b = (int)((binaryImg->imageData + binaryImg->widthStep*y)[x]);
			if(b == 0)
			{
				int neighbor =  (int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x-1]) + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x])   + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x+1]) + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y))[x-1])   + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y))[x+1])   + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x-1]) + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x])   + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x+1]) ;

				if(neighbor <= -3)
					(tempBinImg->imageData + tempBinImg->widthStep*y)[x] = 255;
			}
		}
	}

	cvReleaseImage(&binaryImg);
	binaryImg = cvCreateImage(cvGetSize(handImg),8,1);
	cvCopy(tempBinImg,binaryImg);
	cvReleaseImage(&tempBinImg);

	int *theBox = new int[4];
	int *theCent = new int[2];
	int nThreshold = 30;
	int nMaxRect = 0;

	IplConvKernel *strel = cvCreateStructuringElementEx(5,5,2,2,CV_SHAPE_CROSS);
	cvErode(binaryImg,binaryImg,strel,1);
	cvDilate(binaryImg,binaryImg,strel,1);
	cvReleaseStructuringElement(&strel);

	getConnexeCenterBox(binaryImg,nMaxRect,theCent,theBox,nThreshold);
	IplImage *outputImg = NULL;
	if(nMaxRect > 0)
	{
		CvPoint lt = cvPoint(theBox[0],theBox[1]);
		CvPoint rb = cvPoint(theBox[2],theBox[3]);
		rect = CRect(theBox[0]+handCvRect.x,theBox[1]+handCvRect.y,theBox[2]+handCvRect.x,theBox[3]+handCvRect.y);
		cvReleaseImage(&handImg);
		handImg =getROIImage(rgbImg,handCvRect);
		getForeImage(handImg,binaryImg);
		outputImg = getROIImage(handImg,cvRect(theBox[0],theBox[1],theBox[2]-theBox[0],theBox[3]-theBox[1]));
	}
	delete [] theBox;
	delete [] theCent;
	cvReleaseImage(&handImg);
	cvReleaseImage(&binaryImg);
	return outputImg;
}

IplImage* CHandSegment_HMM::kickHandForSelect(IplImage *rgbImg, Mat mDepth, CvPoint point)
{
	bool bDepthUsed = true;
	unsigned short depthValue = mDepth.at<unsigned short>(point.y,point.x);
	if( 0 == depthValue )
	{
		bDepthUsed = false;
		return NULL;
	}
	int skinWidth = 30;
	int handWidth = 30;
	int left,right,top,bottom;
	CvRect handCvRect;
	IplImage *handImg = NULL;

	unsigned char r,g,b;
	double cr,cb;
	int y,x;

	{
		left = max(point.x-handWidth-70,0);
		top = max(point.y-handWidth-70,0);
		right = min(point.x + handWidth+70, 640);
		bottom = min(point.y + handWidth+70, 480);
		handCvRect = cvRect(left,top,right-left,bottom-top);
		cvReleaseImage(&handImg);
		handImg = getROIImage(rgbImg,handCvRect);

		for(y=0; y<handCvRect.height; y++)
		{
			for(x=0; x<handCvRect.width; x++)
			{
				r = (handImg->imageData + handImg->widthStep*y)[x*3+2];
				g = (handImg->imageData + handImg->widthStep*y)[x*3+1];
				b = (handImg->imageData + handImg->widthStep*y)[x*3+0];
				cr = 0.5000*r - 0.4187*g - 0.0813*b + 128; // cr
				cb = -0.1687*r - 0.3313*g + 0.5000*b + 128; // cb
				unsigned short tempDepth = mDepth.at<unsigned short>(y+handCvRect.y,x+handCvRect.x);
				if( fabs(faceModel.mean_cr-cr) > 4*faceModel.d_cr ||
					fabs(faceModel.mean_cb-cb) > 4*faceModel.d_cb ||
					tempDepth >= depthValue + 150 )// || tempDepth == 0)
				{
					(handImg->imageData + handImg->widthStep*y)[x*3+0] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+1] = 0;
					(handImg->imageData + handImg->widthStep*y)[x*3+2] = 0;
				}

			}
		}

		int t = clock();
		CString st;
		st.Format(".\\selectWord\\%d.bmp",t);
		cvSaveImage(st,handImg);
	}

	IplImage *binaryImg = cvCreateImage(cvSize(handImg->width,handImg->height),8,1);
	IplImage *tempBinImg = cvCreateImage(cvGetSize(binaryImg),8,1);
	cvCvtColor(handImg,binaryImg,CV_BGR2GRAY);
	cvThreshold(binaryImg,binaryImg,0,255,CV_THRESH_BINARY);
	cvCopy(binaryImg,tempBinImg);

	for(y=1; y<handCvRect.height-1; y++)
	{
		for(x=1; x<handCvRect.width-1; x++)
		{
			int b = (int)((binaryImg->imageData + binaryImg->widthStep*y)[x]);
			if(b == 0)
			{
				int neighbor =  (int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x-1]) + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x])   + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y-1))[x+1]) + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y))[x-1])   + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y))[x+1])   + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x-1]) + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x])   + 
					(int)((binaryImg->imageData + binaryImg->widthStep*(y+1))[x+1]) ;

				if(neighbor <= -3)
					(tempBinImg->imageData + tempBinImg->widthStep*y)[x] = 255;
			}
		}
	}

	cvReleaseImage(&binaryImg);
	binaryImg = cvCreateImage(cvGetSize(handImg),8,1);
	cvCopy(tempBinImg,binaryImg);
	cvReleaseImage(&tempBinImg);

	int *theBox = new int[4];
	int *theCent = new int[2];
	int nThreshold = 30;
	int nMaxRect = 0;

	IplConvKernel *strel = cvCreateStructuringElementEx(5,5,2,2,CV_SHAPE_CROSS);
	cvErode(binaryImg,binaryImg,strel,1);
	cvDilate(binaryImg,binaryImg,strel,1);
	cvReleaseStructuringElement(&strel);

	getConnexeCenterBox(binaryImg,nMaxRect,theCent,theBox,nThreshold);
	IplImage *outputImg = NULL;
	if(nMaxRect > 0)
	{
		CvPoint lt = cvPoint(theBox[0],theBox[1]);
		CvPoint rb = cvPoint(theBox[2],theBox[3]);
		cvReleaseImage(&handImg);
		handImg =getROIImage(rgbImg,handCvRect);
		getForeImage(handImg,binaryImg);
		outputImg = getROIImage(handImg,cvRect(theBox[0],theBox[1],theBox[2]-theBox[0],theBox[3]-theBox[1]));
	}
	delete [] theBox;
	delete [] theCent;
	cvReleaseImage(&handImg);
	cvReleaseImage(&binaryImg);
	return outputImg;
}

IplImage* CHandSegment_HMM::getROIImage(IplImage* src, CvRect roi)
{
	if(src == NULL) return NULL;
	if (roi.x<0) roi.x=0;
	if (roi.y<0) roi.y=0;
	if (roi.x+roi.width-1 >= src->width) roi.width=src->width-roi.x;
	if (roi.y+roi.height-1 >= src->height) roi.height=src->height-roi.y;
	cvSetImageROI(src,roi);
	IplImage* dst = cvCreateImage(cvSize(roi.width,roi.height),src->depth,src->nChannels);
	cvCopy(src,dst);
	cvResetImageROI(src);
	return dst;
}

void CHandSegment_HMM::colorClusterCv(IplImage* img, int clusterNum)
{
	int i = 0;
	int j = 0;
	int k = 0;

	int width = img->width;
	int height = img->height;

	//////////////////////////////////////////////////////////////////////////
	//clustering
	//save the Cr && Cb by CvMat format
	//////////////////////////////////////////////////////////////////////////

	CvMat *bufferMotionRegionImg = cvCreateMat(width*height,2,CV_32FC1);

	unsigned char r;
	unsigned char g;
	unsigned char b;
	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			k = j*width + i;

			r = (img->imageData + img->widthStep * j)[i*3+2];
			g = (img->imageData + img->widthStep * j)[i*3+1];
			b = (img->imageData + img->widthStep * j)[i*3+0];

			((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*k))[0]=
				0.5000*r-0.4187*g-0.0813*b+128; // cr
			((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*k))[1]=
				-0.1687*r-0.3313*g+0.5000*b+128; // cb
		}
	}

	cvSaveImage("ori.bmp",img);

	IplImage *crcbImg = cvCreateImage(cvGetSize(img),img->depth,img->nChannels);
	cvCvtColor(img,crcbImg,CV_BGR2YCrCb);

	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			(crcbImg->imageData + crcbImg->widthStep * j)[i*3+2] = 0;
		}
	}

	cvSaveImage("crcb.bmp",crcbImg);
	cvReleaseImage(&crcbImg);
	crcbImg = NULL;

	int maxIndex = 0;
	CvMat *clusterIndex = cvCreateMat(width*height,1,CV_32SC1);
	cvKMeans2(bufferMotionRegionImg,clusterNum,clusterIndex,cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.01 ));

	int indexCount[3] = {0,0,0};

	IplImage *testImg = cvCreateImage(cvGetSize(img),img->depth,img->nChannels);

	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			k = j*width + i;
			indexCount[clusterIndex->data.i[k]]++;
			if(clusterIndex->data.i[k] == 0)
			{
				(testImg->imageData + testImg->widthStep * j)[i*3+2] = 0;
				(testImg->imageData + testImg->widthStep * j)[i*3+1] = 0;
				(testImg->imageData + testImg->widthStep * j)[i*3+0] = 255;
			}
			else if(clusterIndex->data.i[k] == 1)
			{
				(testImg->imageData + testImg->widthStep * j)[i*3+2] = 0;
				(testImg->imageData + testImg->widthStep * j)[i*3+1] = 255;
				(testImg->imageData + testImg->widthStep * j)[i*3+0] = 0;
			}
			else
			{
				(testImg->imageData + testImg->widthStep * j)[i*3+2] = 255;
				(testImg->imageData + testImg->widthStep * j)[i*3+1] = 0;
				(testImg->imageData + testImg->widthStep * j)[i*3+0] = 0;
			}
		}
	}

	cvSaveImage("target.bmp",testImg);
	cvReleaseImage(&testImg);
	testImg = NULL;


	double dCr = 0;
	double dCb = 0;
	double sumCr = 0;
	double sumCb = 0;
	ColorModel model1,model2,model3;
	int index = 0;
	int count = 0;

	for(i=0; i<clusterNum; i++)
	{
		if(indexCount[i] > indexCount[maxIndex])
			maxIndex = i;
	}
	count = indexCount[maxIndex];

	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			k = j * width + i;
			index = clusterIndex->data.i[k];
			if(index == maxIndex)
			{
				sumCr += ((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*k))[0];//Cr
				sumCb += ((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*k))[1];//Cb
			}
		}
	}
	faceModel.mean_cr = sumCr/count;
	faceModel.mean_cb = sumCb/count;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			k = j * width + i;
			index = clusterIndex->data.i[k];
			if (index == maxIndex)
			{
				dCr += (((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*k))[0] - faceModel.mean_cr)
						*(((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*k))[0] - faceModel.mean_cr);//Cr
				dCb += (((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*k))[1] - faceModel.mean_cb)
						*(((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*k))[1] - faceModel.mean_cb);//Cb
			}
		}
	}
	faceModel.d_cr=sqrt(dCr/count);
	faceModel.d_cb=sqrt(dCb/count);

	cvReleaseMat(&bufferMotionRegionImg);
	cvReleaseMat(&clusterIndex);
}

void CHandSegment_HMM::getColorModel(IplImage* image, ColorModel& model)
{
	int width = image->width;
	int height = image->height;

	int i,j;
	int count = 0;
	double sumCr = 0;
	double sumCb = 0;
	float cr,cb;

	double back_meanCr = 0;
	double back_meanCb = 0;
	double back_dCr = 0;
	double back_dCb = 0;
	unsigned char r,g,b;
	CvMat *bufferMotionRegionImg;
	bufferMotionRegionImg = cvCreateMat(height*width,2,CV_32FC1);
	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			if((image->imageData+image->widthStep*j)[i*3+0] != 0 && 
				(image->imageData+image->widthStep*j)[i*3+1] != 0 &&
				(image->imageData+image->widthStep*j)[i*3+2] != 0 )
			{
				r = (image->imageData+image->widthStep*j)[i*3+2];
				g = (image->imageData+image->widthStep*j)[i*3+1];
				b = (image->imageData+image->widthStep*j)[i*3+0];

				cr = 0.5000*r-0.4187*g-0.0813*b+128; // cr
				((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*count))[0]= cr;
				sumCr += cr;

				cb = -0.1687*r-0.3313*g+0.5000*b+128; // cb
				((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step*count))[1]= cb;
				sumCb += cb;
				count++;
			}
		}
	}
	if(count != 0)
	{
		back_meanCr = sumCr/count;
		back_meanCb = sumCb/count;
	}
	else
	{
		back_meanCr = 0;
		back_meanCb = 0;
	}

	for(i=0; i<count; i++)
	{
		cr = ((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step * i) )[0];
		cb = ((float*)(bufferMotionRegionImg->data.ptr + bufferMotionRegionImg->step * i) )[1];

		back_dCr += (cr - back_meanCr) * (cr - back_meanCr);
		back_dCb += (cb - back_meanCb) * (cb - back_meanCb);
	}

	if(count != 0)
	{
		back_dCb = sqrt(back_dCb/count);
		back_dCr = sqrt(back_dCr/count);
	}
	else
	{
		back_dCb = 0;
		back_dCr = 0;
	}

	model.mean_cb = back_meanCb;
	model.mean_cr = back_meanCr;
	model.d_cb = back_dCb;
	model.d_cr = back_dCr;

	cvReleaseMat(&bufferMotionRegionImg);
}

void CHandSegment_HMM::getConnexeCenterBox(IplImage* image, int& nMaxRect, int* &theCent, int* &theBox, int nThreshold)
{
	int nconnectSrcArea = image->width*image->height;
	void *bufferOut =NULL;
	unsigned char *connectSrc =NULL;

	bufferOut  = (void*) malloc(nconnectSrcArea*sizeof(unsigned char));
	connectSrc = (unsigned char*) malloc(nconnectSrcArea*sizeof(unsigned char));
	memset(connectSrc,0,nconnectSrcArea*sizeof(unsigned char));
	int i,j;
	for(j = 0; j < image->height; j++)
	{
		for(i = 0; i < image->width; i++)
		{
			connectSrc[j*image->width+i]=(unsigned char)(image->imageData+image->widthStep*j)[i];		
		}
	}

	int nMax = 1;
	int bufferDims[3] = { image->width,image->height, 1 };
	Connexe_SetMinimumSizeOfComponents( nThreshold);		// original is set to 1;
	Connexe_SetMaximumNumberOfComponents( nMax );		// original is set to 1;

	int countConnect =  CountConnectedComponents( (void*)connectSrc, CONN_UCHAR,bufferOut, CONN_UCHAR,bufferDims );//bufferOut包含位源图像区域标号;

	free(connectSrc);
	connectSrc = (unsigned char*) bufferOut;

	for(j = 0; j < image->height; j ++)
	{
		for(i = 0; i < image->width; i ++)
		{
			(image->imageData+image->widthStep*j)[i] = connectSrc[j*image->width+i];
			if(connectSrc[j*image->width+i] != 0)
			{
				(image->imageData+image->widthStep*j)[i] = char (255);
			}
		}
	}
	nMaxRect = GetCenterofComponentWithBoxNew(bufferOut, bufferDims, theCent, theBox, nMax, FALSE, nThreshold);

	free(bufferOut);
	bufferOut = NULL;
	connectSrc = NULL;
}

void CHandSegment_HMM::getForeImage(IplImage* colorImage, IplImage* binaryImage)
{
	if(colorImage->width != binaryImage->width || colorImage->height != binaryImage->height)
		return;
	for(int y=0; y<colorImage->height; y++)
	{
		for(int x=0; x<colorImage->width; x++)
		{
			if( (binaryImage->imageData + binaryImage->widthStep*y)[x] == 0 )
			{
				(colorImage->imageData + colorImage->widthStep*y)[3*x+0] = 0;
				(colorImage->imageData + colorImage->widthStep*y)[3*x+1] = 0;
				(colorImage->imageData + colorImage->widthStep*y)[3*x+2] = 0;
			}
		}
	}
}

IplImage* CHandSegment_HMM::getConnexeImage(IplImage* image)
{
	int nconnectSrcArea = image->width*image->height;
	void *bufferOut =NULL;
	unsigned char *connectSrc =NULL;

	bufferOut  = (void*) malloc(nconnectSrcArea*sizeof(unsigned char));
	connectSrc = (unsigned char*) malloc(nconnectSrcArea*sizeof(unsigned char));
	memset(connectSrc,0,nconnectSrcArea*sizeof(unsigned char));
	int i,j;
	for(j = 0; j < image->height; j++)
	{
		for(i = 0; i < image->width; i++)
		{
			connectSrc[j*image->width+i]=(unsigned char)(image->imageData+image->widthStep*j)[i];		
		}
	}

	int nMax = 1;
	int bufferDims[3] = { image->width,image->height, 1 };
	Connexe_SetMinimumSizeOfComponents( 30);		// original is set to 1;
	Connexe_SetMaximumNumberOfComponents( 1 );		// original is set to 1;

	int countConnect =  CountConnectedComponents( (void*)connectSrc, CONN_UCHAR,bufferOut, CONN_UCHAR,bufferDims );//bufferOut包含位源图像区域标号;

	free(connectSrc);
	connectSrc = (unsigned char*) bufferOut;

	IplImage* img = cvCreateImage(cvGetSize(image),8,1);
	for(j = 0; j < image->height; j ++)
	{
		for(i = 0; i < image->width; i ++)
		{
			(image->imageData+image->widthStep*j)[i] = connectSrc[j*image->width+i];
			if(connectSrc[j*image->width+i] != 0)
			{
				(img->imageData+img->widthStep*j)[i] = char (255);
			}
			else
			{
				(img->imageData+img->widthStep*j)[i] = 0;
			}
		}
	}
	free(bufferOut);
	bufferOut = NULL;
	connectSrc = NULL;
	return img;
}

bool CHandSegment_HMM::judgePointInRect(CvPoint point, CRect rect)
{
	if( point.x >= rect.left && point.y >= rect.top &&
		point.x <= rect.right && point.y <= rect.bottom)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CHandSegment_HMM::judgeTwoImageHaveUnion(IplImage* leftImg, CRect leftRect, IplImage* rightImg, CRect rightRect)
{
	if(leftRect.top < rightRect.bottom || leftRect.bottom > rightRect.top ||
		leftRect.left > rightRect.right || leftRect.right < rightRect.left)
	{
		return false;
	}

	unsigned char lr,lg,lb;
	unsigned char rr,rg,rb;
	for(int y=leftRect.top; y<leftRect.bottom; y++)
	{
		for(int x=rightRect.left; x<rightRect.right; x++)
		{
			if( y >= rightRect.top && y <= rightRect.bottom && x >= leftRect.left && x <= leftRect.right )
			{
				lr = (leftImg->imageData + leftImg->widthStep*(y-leftRect.top))[(x-leftRect.left)*3+2];
				lg = (leftImg->imageData + leftImg->widthStep*(y-leftRect.top))[(x-leftRect.left)*3+2];
				lb = (leftImg->imageData + leftImg->widthStep*(y-leftRect.top))[(x-leftRect.left)*3+2];
				if( lr != 0 && lg != 0 && lb != 0)
				{
					rr = (rightImg->imageData + rightImg->widthStep*(y-rightRect.top))[(x-rightRect.left)*3+2];
					rg = (rightImg->imageData + rightImg->widthStep*(y-rightRect.top))[(x-rightRect.left)*3+2];
					rb = (rightImg->imageData + rightImg->widthStep*(y-rightRect.top))[(x-rightRect.left)*3+2];

					if( lr == rr && lg == rg && lb == rb )
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void CHandSegment_HMM::getFaceNeckRegion(Mat rgbMat, Mat depthMat)
{
	IplImage img(rgbMat);
	CvPoint headCenter = cvPoint((m_faceRect.left+m_faceRect.right)/2,(m_faceRect.top+m_faceRect.bottom)/2);
	CRect tempHeadNeckRect = CRect(max(headCenter.x-80,0),
		max(headCenter.y-80,0),
		min(headCenter.x+80,640),
		min(headCenter.y+80,480));
	int width = tempHeadNeckRect.right - tempHeadNeckRect.left;
	int height = tempHeadNeckRect.bottom - tempHeadNeckRect.top;
	CvRect tempHeadNeckCvRect = cvRect(tempHeadNeckRect.left,tempHeadNeckRect.top,width,height);
	IplImage *headNeckImg = getROIImage(&img,tempHeadNeckCvRect);
	//cvSaveImage("D:\\head.bmp",headNeckImg);
	unsigned char r,g,b;
	double cr,cb;
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			r = (headNeckImg->imageData + headNeckImg->widthStep*y)[x*3+2];
			g = (headNeckImg->imageData + headNeckImg->widthStep*y)[x*3+1];
			b = (headNeckImg->imageData + headNeckImg->widthStep*y)[x*3+0];
			cr = 0.5000*r - 0.4187*g - 0.0813*b + 128; // cr
			cb = -0.1687*r - 0.3313*g + 0.5000*b + 128; // cb
			unsigned short tempDepth = depthMat.at<unsigned short>(y+tempHeadNeckCvRect.y,x+tempHeadNeckCvRect.x);
			if( fabs(faceModel.mean_cr-cr) > 4*faceModel.d_cr ||
				fabs(faceModel.mean_cb-cb) > 4*faceModel.d_cb ||tempDepth > m_faceDepthMin + 300)
			{
				(headNeckImg->imageData + headNeckImg->widthStep*y)[x*3+0] = 0;
				(headNeckImg->imageData + headNeckImg->widthStep*y)[x*3+1] = 0;
				(headNeckImg->imageData + headNeckImg->widthStep*y)[x*3+2] = 0;
			}
		}
	}
	//cvSaveImage("D:\\headKick.bmp",headNeckImg);
	int *theBox = new int[4];
	int *theCent = new int[2];
	int nThreshold = 30;
	int nMaxRect = 0;
	IplImage *biImg = cvCreateImage(cvSize(width,height),8,1);
	cvCvtColor(headNeckImg,biImg,CV_BGR2GRAY);
	cvThreshold(biImg,biImg,0,255,CV_THRESH_BINARY);

	getConnexeCenterBox(biImg,nMaxRect,theCent,theBox,nThreshold);
	if(nMaxRect > 0)
	{
		CvPoint lt = cvPoint(theBox[0],theBox[1]);
		CvPoint rb = cvPoint(theBox[2],theBox[3]);
		m_pFaceNeckBiImg = getROIImage(biImg,cvRect(lt.x,lt.y,rb.x-lt.x,rb.y-lt.y));
		m_faceNeckRect = CRect(theBox[0]+tempHeadNeckCvRect.x,theBox[1]+tempHeadNeckCvRect.y,theBox[2]+tempHeadNeckCvRect.x,theBox[3]+tempHeadNeckCvRect.y);
	}
	delete [] theBox;
	delete [] theCent;
	cvReleaseImage(&headNeckImg);
	cvReleaseImage(&biImg);
}

void CHandSegment_HMM::postureVectorCopy(vector<Posture> src, vector<Posture> &dst)
{
	for(int i=0; i<src.size(); i++)
	{
		Posture srcPosture = src[i];
		Posture dstPosture;
		if(srcPosture.leftHandImg != NULL)
		{
			dstPosture.leftHandImg = cvCreateImage(cvGetSize(srcPosture.leftHandImg),srcPosture.leftHandImg->depth,srcPosture.leftHandImg->nChannels);
			cvCopy(srcPosture.leftHandImg,dstPosture.leftHandImg);
		}
		else
		{
			dstPosture.leftHandImg = NULL;
		}

		if(srcPosture.rightHandImg != NULL)
		{
			dstPosture.rightHandImg = cvCreateImage(cvGetSize(srcPosture.rightHandImg),srcPosture.rightHandImg->depth,srcPosture.rightHandImg->nChannels);
			cvCopy(srcPosture.rightHandImg,dstPosture.rightHandImg);
		}
		else
		{
			dstPosture.rightHandImg = NULL;
		}
		dstPosture.leftHandPt = srcPosture.leftHandPt;
		dstPosture.leftWristPt = srcPosture.leftWristPt;
		dstPosture.rightHandPt = srcPosture.rightHandPt;
		dstPosture.rightWristPt = srcPosture.rightWristPt;

		dst.push_back(dstPosture);
	}
}

void CHandSegment_HMM::clearPostureVector(vector<Posture> &vec)
{
	try
	{
		for(int i=0; i<vec.size(); i++)
		{
			Posture posture = vec[i];
			if(posture.leftHandImg != NULL)
			{
				cvReleaseImage(&(posture.leftHandImg));
			}
			if(posture.rightHandImg != NULL)
			{
				cvReleaseImage(&(posture.rightHandImg));
			}
		}
		vec.clear();
	}
	catch (...)
	{

	}

}

Mat CHandSegment_HMM::retrieveColorDepth(Mat depthMat)
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

void CHandSegment_HMM::copyDepthMat(Mat depthMat)
{
	m_depthCopyMat = depthMat.clone();
}

void CHandSegment_HMM::getHogFeature(IplImage *img1, IplImage *img2, double *hogFeas)
{
	if(img1 == NULL) img1 = cvLoadImage("black.jpg");
	if(img2 == NULL) img2 = cvLoadImage("black.jpg");

	IplImage *tmpImg1 = cvCreateImage(cvSize(IMG_SIZE,IMG_SIZE),img1->depth,img1->nChannels);
	cvResize(img1,tmpImg1);

	IplImage *tmpGray1 = cvCreateImage(cvSize(IMG_SIZE,IMG_SIZE),tmpImg1->depth,1);
	if(tmpImg1->nChannels == 3)
		cvCvtColor(tmpImg1,tmpGray1,CV_BGR2GRAY);
	else 
		cvCopy(tmpImg1,tmpGray1);

	IplImage *tmpImg2 = cvCreateImage(cvSize(IMG_SIZE,IMG_SIZE),img2->depth,img2->nChannels);
	cvResize(img2,tmpImg2);

	IplImage *tmpGray2 = cvCreateImage(cvSize(IMG_SIZE,IMG_SIZE),tmpImg2->depth,1);
	if(tmpImg2->nChannels == 3)
		cvCvtColor(tmpImg2,tmpGray2,CV_BGR2GRAY);
	else 
		cvCopy(tmpImg2,tmpGray2);

	IplImage *img = cvCreateImage(cvSize(IMG_SIZE*2,IMG_SIZE),tmpGray2->depth,tmpGray2->nChannels);
	CvRect roi = cvRect(0,0,IMG_SIZE,IMG_SIZE);
	cvSetImageROI(img,roi);
	cvCopy(tmpGray1,img);
	cvResetImageROI(img);

	roi = cvRect(IMG_SIZE,0,IMG_SIZE,IMG_SIZE);
	cvSetImageROI(img,roi);
	cvCopy(tmpGray2,img);
	cvResetImageROI(img);

	//cvReleaseImage(&img1);
	//cvReleaseImage(&img2);
	cvReleaseImage(&tmpImg1);
	cvReleaseImage(&tmpImg2);
	cvReleaseImage(&tmpGray1);
	cvReleaseImage(&tmpGray2);

	//writeHogFeature(img,hogFileName,1);
	//const int FEA_NUM = 51; //need mofify
	if(img == NULL || bBlackImg(img))
	{
		for(int i=0; i<DES_FEA_NUM; i++)
		{
			hogFeas[i] = 0.0;
		}
		return;
	}

	IplImage *tmpImg = cvCreateImage(cvSize(IMG_SIZE,IMG_SIZE),img->depth,img->nChannels);
	cvResize(img,tmpImg);

	IplImage *tmpGray = cvCreateImage(cvSize(IMG_SIZE,IMG_SIZE),tmpImg->depth,1);
	if(tmpImg->nChannels == 3)
		cvCvtColor(tmpImg,tmpGray,CV_BGR2GRAY);
	else 
		cvCopy(tmpImg,tmpGray);

	//need modify
	//HOGDescriptor *hog=new HOGDescriptor(cvSize(IMG_SIZE,IMG_SIZE),cvSize(16,16),cvSize(8,8),cvSize(8,8),9);
	HOGDescriptor *hog=new HOGDescriptor(cvSize(IMG_SIZE,IMG_SIZE),cvSize(32,32),cvSize(16,16),cvSize(16,16),9);  //324
	/////////////////////window大ä¨®小?为a64*64，ê?block大ä¨®小?为a8*8，ê?block步?长¡è为a4*4，ê?cell大ä¨®小?为a4*4
	Mat handMat(tmpGray);

	vector<float> descriptors;

	hog->compute(handMat, descriptors,Size(0,0), Size(0,0));


	double total = 0;
	int i;
	for(i=0; i<descriptors.size(); i++)
	{
		total += descriptors[i];
	}
	CvMat *feas = cvCreateMat(1,descriptors.size(),CV_32FC1);
	for(i=0; i<descriptors.size(); i++)
	{
		cvmSet(feas,0,i,descriptors[i]/total);
	}

	CvMat *pcaFeas = cvCreateMat(1,DES_FEA_NUM,CV_32FC1);
	cvMatMul(feas,pcaMat,pcaFeas);

#ifdef UsePCA
	for(i=0; i<DES_FEA_NUM; i++)
	{
		hogFeas[i] = (cvmGet(pcaFeas,0,i) + 0.0295)/(0.0149 + 0.0295);
	}
#endif

#ifndef UsePCA
	for(i=0; i<DES_FEA_NUM; i++)
	{
		hogFeas[i] = (cvmGet(feas,0,i) + 0.0295)/(0.0149 + 0.0295);
	}
#endif
	

	cvReleaseMat(&feas);
	cvReleaseMat(&pcaFeas);

	cvReleaseImage(&tmpImg);
	cvReleaseImage(&tmpGray);
	cvReleaseImage(&img);
}

bool CHandSegment_HMM::bBlackImg(IplImage *img)
{
	if(img->nChannels == 3)
	{
		for(int i=0; i<img->width; i++)
		{
			for(int j=0; j<img->height; j++)
			{
				if((int)((img->imageData + img->widthStep*j)[i*3+0]) != 0 ||
					(int)((img->imageData + img->widthStep*j)[i*3+1]) != 0 ||
					(int)((img->imageData + img->widthStep*j)[i*3+2]) != 0)
					return false;
			}
		}
		return true;
	}
	else
	{
		for(int i=0; i<img->width; i++)
		{
			for(int j=0; j<img->height; j++)
			{
				if((int)((img->imageData + img->widthStep*j)[i]) != 0)
					return false;
			}
		}
		return true;
	}

}





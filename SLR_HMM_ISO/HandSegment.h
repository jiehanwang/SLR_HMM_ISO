//////////////////////////////////////////////////////////////////////////
/// @file     handSegmentation.h
/// @author   xu zhihao
/// @created  2012-11-08

/// @brief    segment hand to get postures
/// @version  1.0
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include <iostream>
#include "connexe.h"
// #include "cv.h"
// #include "highgui.h"
#include <opencv2\opencv.hpp>
#include "JFD_api.h"
#include "JFD_define.h"
//#include "globalDefine.h"
#include <fstream>

using namespace std;
using namespace cv;

const int SRC_FEA_NUM = 324;
const int DES_FEA_NUM = 51;
const int IMG_SIZE = 64;
const CString PCA_FILE_NAME = "..\\input\\pca_51.txt";
struct Posture
{
	IplImage *leftHandImg;    ///< left hand image
	IplImage *rightHandImg;   ///< right hand image
	CvPoint leftHandPt;       ///< left hand point
	CvPoint leftWristPt;      ///< left wrist point
	CvPoint rightHandPt;      ///< right hand point
	CvPoint rightWristPt;     ///< right wrist point

	Posture():leftHandImg(NULL),rightHandImg(NULL) {};
};

struct ColorModel
{
	double mean_cr;     ///< mean of cr
	double mean_cb;     ///< mean of cb
	double d_cr;        ///< variance of cr
	double d_cb;        ///< variance of cb
	ColorModel():mean_cr(0),mean_cb(0),d_cr(0),d_cb(0){};
};


class CHandSegment
{
public:
	CHandSegment(void);
	~CHandSegment(void);

	//////////////////////////////////////////////////////////////////////////
	/// @brief initial the class 
	//////////////////////////////////////////////////////////////////////////
	void init();

	//////////////////////////////////////////////////////////////////////////
	/// @brief destroy the class 
	/////////////////////////////////////////////////////////////////////////
	void destroy();

	//////////////////////////////////////////////////////////////////////////
	/// @brief get hand postures of two hand
	/// @param[in] rgbImg original color image
	/// @param[in] mDepth original depth mat
	/// @param[in] bLeft if left hand does segmentation
	/// @param[in] bRight if right hand does segmentation
	/// @param[in] leftPoint left hand center point
	/// @param[in] rightPoint right hand center point
	/// @param[out] outLeftPoint estimate left hand center point by skin color and depth restriction
	/// @param[out] outRightPoint estimate right hand center point by skin color and depth restriction
	/// @param[out] outLeftCorner maintain to calculate the relative location of wrist point
	/// @param[out] outRightCorner maintain to calculate the relative location of wrist point
	/// @param[in] timeStamp capture time stamp for save image by different time name
	/// @param[out] posture get hand postures data
	/// @param[in] bVideo 0-read video, 1-real time,they have different operation
	/////////////////////////////////////////////////////////////////////////////
	void kickHands(IplImage *rgbImg, Mat mDepth, bool bLeft, bool bRight, CvPoint leftPoint, CvPoint rightPoint,CvPoint& outLeftPoint, CvPoint& outRightPoint, CvPoint& outLeftCorner, CvPoint& outRightCorner,
		LONGLONG timeStamp, Posture &posture, bool bVideo);

	//////////////////////////////////////////////////////////////////////////
	/// @brief head detection,if true, initial m_pHeadImage, m_faceRect,m_faceDepthMin
	/// @param[in] colorImage original color image
	/// @param[in] mDepth original depth mat
	/// @param[in] headCenter head center point by skeleton tracking
	/// @return bool judge if head exists
	/////////////////////////////////////////////////////////////////////////
	bool headDetection(IplImage* colorImage, Mat mDepth, CvPoint headCenter);
	bool headDetectionByOpenCV(IplImage* colorImage, Mat mDepth, CvPoint headCenter);
	//////////////////////////////////////////////////////////////////////////
	/// @brief get hand posture by depth and color data
	/// @param[in] rgbImg original color image
	/// @param[in] mDepth original depth image
	/// @param[in] point hand center point
	/// @param[out] hand posture rect
	/// @param[in] flag 0-left hand,1-right hand
	/// @param[out] outPoint estimate point(the min depth value point of skin color region)
	/// @param[in] timeStamp capture time stamp for save image by different time name
	/// @param[in] bVideo 0-read video, 1-real time,they have different operation
	/// @return IplImage* hand posture
	////////////////////////////////////////////////////////////////////////////
	IplImage* kickOneHand(IplImage *rgbImg, Mat mDepth, CvPoint point, CRect& rect, int flag, CvPoint& outPoint,LONGLONG timeStamp, bool bVideo);

	//////////////////////////////////////////////////////////////////////////
	/// @brief get hand posture by depth and color data to choose word
	/// @param[in] rgbImg original color image
	/// @param[in] mDepth original depth image
	/// @param[in] point hand center point
	/// @return IplImage* hand posture
	////////////////////////////////////////////////////////////////////////////
	IplImage* kickHandForSelect(IplImage *rgbImg, Mat mDepth, CvPoint point);

	//////////////////////////////////////////////////////////////////////////
	/// @brief calculate the distance of two point
	/// @param[in] point1 CvPoint
	/// @param[in] point2 CvPoint
	/// @return double get the distance 
	//////////////////////////////////////////////////////////////////////////
	double getDistanceOfPoints(CvPoint point1, CvPoint point2);

	//////////////////////////////////////////////////////////////////////////
	/// @brief get region of interest(ROI) of source image
	/// @param[in] src source image
	/// @param[in] roi region of interest(ROI) 
	/// @return IplImage* ROI image
	//////////////////////////////////////////////////////////////////////////
	IplImage* getROIImage(IplImage* src, CvRect roi);

	//////////////////////////////////////////////////////////////////////////
	/// @brief get color model by cluster 
	/// @param[in] img original color image 
	/// @param[in] clusterNum cluster number
	//////////////////////////////////////////////////////////////////////////
	void colorClusterCv(IplImage* img, int clusterNum);

	//////////////////////////////////////////////////////////////////////////
	/// @brief get color model of image, which has been restricted by skin color
	/// @param[in] image original image
	/// @param[out] model result skin color model
	//////////////////////////////////////////////////////////////////////////
	void getColorModel(IplImage* image, ColorModel& model);

	//////////////////////////////////////////////////////////////////////////
	/// @brief get connexe region of one image
	/// @param[in] image original image
	/// @param[out] nMaxrect the number of connexe regions
	/// @param[out] theCent center of connexe regions
	/// @param[out] theBox corners of connexe regions
	/// @param[in] nThreshold minimum pixel numbers  of connexe region
	//////////////////////////////////////////////////////////////////////////
	void getConnexeCenterBox(IplImage* image, int& nMaxRect, int* &theCent, int* &theBox, int nThreshold);

	//////////////////////////////////////////////////////////////////////////
	/// @brief get image without background
	/// @param[in] colorImage original colorImage
	/// @param[in] binaryImage binary image by depth and skin color restriction
	/// @param[out] color image without background and non-skin region
	/////////////////////////////////////////////////////////////////////////
	void getForeImage(IplImage* colorImage, IplImage* binaryImage);

	//////////////////////////////////////////////////////////////////////////
	/// @brief get the max connexe region of input image
	/// @param[in] image original image
	/// @return IplImage* connexe image
	//////////////////////////////////////////////////////////////////////////
	IplImage* getConnexeImage(IplImage* image);

	//////////////////////////////////////////////////////////////////////////
	/// @brief get skin color region of face and neck
	/// @param[in] rgbMat rgb mat data
	/// @param[in] depthMat depth mat data
	//////////////////////////////////////////////////////////////////////////
	void getFaceNeckRegion(Mat rgbMat, Mat depthMat);

	//////////////////////////////////////////////////////////////////////////
	/// @brief if point is in the rect
	/// @param[in] point CvPoint
	/// @param[in] rect CRect
	/// @return bool
	//////////////////////////////////////////////////////////////////////////
	bool judgePointInRect(CvPoint point, CRect rect);

	//////////////////////////////////////////////////////////////////////////
	/// @brief judge whether two images(left and right hand postures) have union pixels
	/// @param[in] leftImg left hand posture
	/// @param[in] leftRect left hand CRect in original image(640*480)
	/// @param[in] rightImg right hand posture
	/// @param[in] rightRect right hand CRect in original image(640*480)
	/// @return bool 
	//////////////////////////////////////////////////////////////////////////
	bool judgeTwoImageHaveUnion(IplImage* leftImg, CRect leftRect, IplImage* rightImg, CRect rightRect);

	//////////////////////////////////////////////////////////////////////////
	/// @brief copy structrue vector<Posture>
	/// @param[in] src source vector
	/// @param[out] dst des vector
	//////////////////////////////////////////////////////////////////////////
	void postureVectorCopy(vector<Posture> src, vector<Posture> &dst);

	//////////////////////////////////////////////////////////////////////////
	/// @brief clear vector<Posture>
	/// @param[out] vec clear object
	//////////////////////////////////////////////////////////////////////////
	void clearPostureVector(vector<Posture> &vec);

	//////////////////////////////////////////////////////////////////////////
	/// @brief turn depth data to color depth data
	/// @param[in] depthMat original depth data
	/// @return Mat color depth data
	//////////////////////////////////////////////////////////////////////////
	Mat retrieveColorDepth(Mat depthMat);

	//////////////////////////////////////////////////////////////////////////
	/// @brief copy depth mat
	/// @param[in] depthMat original depth mat
	//////////////////////////////////////////////////////////////////////////
	void copyDepthMat(Mat depthMat);

	IplImage *m_pHeadImage;			// head image by face detection

	void getHogFeature(IplImage *img1, IplImage *img2, double *hogFeas);
	bool bBlackImg(IplImage *img);
private:
	ColorModel faceModel;           //skin color model  of face
	Mat m_depthCopyMat;             //depth copy of first frame 
	unsigned short m_faceDepthMin;  //minimum depth value of face
	IplImage *m_pFaceNeckBiImg;     //binary image of face and neck
	CRect m_faceRect;               //face rect
	CRect m_faceNeckRect;           //rect of face and neck

	CvMat *pcaMat;
public:
	void kickHandsAll(IplImage* rgbImg, Mat mDepth, CvPoint leftPoint, CvPoint rightPoint, Posture & posture,CvRect &leftHand,CvRect &rightHand);
	IplImage* kickOneHandAll(IplImage* rgbImg, Mat mDepth, CvPoint point,CvRect &HandRegion);
};

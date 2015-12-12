#ifndef DEEP_PYRAMID_H
#define DEEP_PYRAMID_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <caffe/caffe.hpp>
#include <caffe/common.hpp>
#include <vector>
#include <utility>

#include <stdio.h>
#define TIMER_START(name) int64 t_##name = getTickCount()
#define TIMER_END(name) printf("TIMER_" #name ":\t%6.2fms\n", \
    1000.f * ((getTickCount() - t_##name) / getTickFrequency()))

#define OBJECT 1
#define NOT_OBJECT -1

double IOU(const cv::Rect& r1, const cv::Rect& r2);

class ObjectBox
{
public:
    double confidence;
    int level;
    cv::Rect norm5Box;
    cv::Rect originalImageBox;
    bool operator< (ObjectBox object)
    {
        return confidence<object.confidence;
    }
};

//friend class DeepPyramid
class DeepPyramidConfiguration
{
public:
    std::string model_file;
    std::string trained_net_file;

    int numLevels;

    cv::Scalar objectRectangleColor;

    std::string svm_trained_file;
    cv::Size filterSize;

    int stride;

    DeepPyramidConfiguration(cv::FileStorage& configFile);
};

class DeepPyramid
{
public:
    DeepPyramid(cv::FileStorage& configFile);

    void extractFeatureVectors(const cv::Mat& img, const int& filterIdx,const std::vector<cv::Rect>& objectsRect, cv::Mat& features, cv::Mat& labels);

    DeepPyramidConfiguration config;

    void detect(const cv::Mat& img, std::vector<ObjectBox>& objects);

    void getNegFeatureVector(int levelIndx, const cv::Rect& rect, cv::Mat& feature);

    int chooseLevel(const cv::Size& filterSize, const cv::Rect& boundBox, const cv::Size& imgSize);

    void getPosFeatureVector(const cv::Rect& rect, const cv::Size& size, cv::Mat& feature, const cv::Size& imgSize);

    std::vector<cv::Mat> imagePyramid;
    std::vector< std::vector<cv::Mat> > max5;
    std::vector< std::vector<cv::Mat> > norm5;
    std::vector<std::pair<cv::Size, CvSVM*> > rootFilter;

    caffe::shared_ptr<caffe::Net<float> > net;

    //Image Pyramid
    cv::Size imageSizeAtPyramidLevel(const cv::Mat& img, const int& level);
    void createImageAtPyramidLevel(const cv::Mat& img, const int& level, cv::Mat& dst);
    void createImagePyramid(const cv::Mat& img);

    //NeuralNet
    void fillNeuralNetInput(int level);

    void getNeuralNetOutput(std::vector<cv::Mat>& netOutput);
    void calculateImageRepresentation();
    void calculateImageRepresentationAtLevel(const int& level);

    //Max5
    void createMax5AtLevel(const int& level,  std::vector<cv::Mat>& max5);
    void createMax5Pyramid();

    //Norm5
    void createNorm5AtLevel(const int& level, std::vector<cv::Mat>& norm5);
    void createNorm5Pyramid();
    void calculateToNorm5(const cv::Mat& img);

    //Root-Filter sliding window
    void rootFilterAtLevel(int rootFilterIndx, int levelIdx, std::vector<ObjectBox>& detectedObjects);
    //rename private: detect()
    void rootFilterConvolution(std::vector<ObjectBox>& detectedObjects);

    //Rectangle transform
    cv::Rect originalRect2Norm5(const cv::Rect& originalRect, int level, const cv::Size& imgSize);
    //rename
    cv::Rect norm5Rect2Original(const cv::Rect& norm5Rect, int level, const cv::Size& imgSize);
    void calculateOriginalRectangle(std::vector<ObjectBox>& detectedObjects, const cv::Size& imgSize);
    void groupOriginalRectangle(std::vector<ObjectBox>& detectedObjects);

    //Rectangle transform ARTICLE
    cv::Rect getRectByNorm5Pixel_ARTICLE(cv::Point point);
    cv::Rect getRectByNorm5Rect_ARTICLE(cv::Rect rect);
    cv::Rect getNorm5RectByOriginal_ARTICLE(cv::Rect originalRect);
    int centerConformity;
    int boxSideConformity;
    void clear();


};
#endif // DEEP_PYRAMID_H

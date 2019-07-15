//
// Created by zhoumohan on 2019/7/15.
//

#ifndef LIVEPALYER_FACETRACK_H
#define LIVEPALYER_FACETRACK_H

#include <opencv2/objdetect.hpp>

using namespace std;
using namespace cv;
class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector) :
            IDetector(),
            Detector(detector) {
    }

//检测到人脸  调用  Mat == Bitmap
    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                   maxObjSize);
    }

    virtual ~CascadeDetectorAdapter() {
    }

private:
    CascadeDetectorAdapter();

    cv::Ptr<cv::CascadeClassifier> Detector;
};

class FaceTrack{
public:
    FaceTrack(const char *path);

    void startTracing();

private:
    DetectionBasedTracker *tracker = 0;
};

#endif //LIVEPALYER_FACETRACK_H

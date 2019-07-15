//
// Created by zhoumohan on 2019/7/15.
//

#include "FaceTrack.h"

FaceTrack::FaceTrack(const char *path) {
    //智能指针
    Ptr<CascadeClassifier> classifier = makePtr<CascadeClassifier>(path);
    //创建检测器
    Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(classifier);

    //跟踪器
    //智能指针
    Ptr<CascadeClassifier> classifier1 = makePtr<CascadeClassifier>(path);
    //创建检测器
    Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(classifier1);

    DetectionBasedTracker::Parameters DetectorParams;

    tracker = makePtr(mainDetector, trackingDetector, DetectorParams);
}

//开始检测
void FaceTrack::startTracing() {
    tracker->run();
}

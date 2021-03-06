#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <opencv2/opencv.hpp>
#include <set>
#include <string>
#include <utility>

class VideoSource {

public :

    virtual cv::Mat getColorFeed(){}
    virtual cv::Mat getDepthFeed(){}
    virtual cv::Mat getOriginalDepth(){}
    virtual cv::Mat getMappedFeed(){}
    virtual void update(){}
    virtual bool isRunning() {return true;}
    virtual ~VideoSource() =default;

    virtual bool hasDepthSource() {}
    virtual std::string getTimeStamp(){}
    virtual int getTimePosition(){}
    virtual double getExactTimePosition(){}
    virtual void resetTime(){}
    virtual std::set<std::string> findLastName() const{}
    virtual bool startRecording(std::string folderLoc){}
    virtual bool stopRecording(std::string folderLoc, bool extractFrames){}
    virtual void extractVideos(){}

};


#endif //VIDEOSOURCE_H

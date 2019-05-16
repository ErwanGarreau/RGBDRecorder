#ifndef REALSENSE_H
#define REALSENSE_H
#undef Q_FOREACH
#include "VideoSource.h"
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#if __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#endif
#include <string>
#include <sstream>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

using namespace rs2;



class RealSense  : public VideoSource{
public:
    RealSense();
    virtual ~RealSense();


    cv::Mat getColorFeed();
    cv::Mat getDepthFeed();
    cv::Mat getMappedFeed();
    void update();
    bool startRecording(std::string folderLoc = "b");
    bool stopRecording(std::string folderLoc = "b",bool extractFrames = false);


    cv::Mat getOriginalDepth(){return depthFeed;}
    bool hasDepthSource() {return true;}
    std::string getTimeStamp() {return "<font color=\"#CD2034\">&#149;</font> En direct";}
    int getTimePosition() {return (int)((std::clock() - startTime)/(double) CLOCKS_PER_SEC);}
    double getExactTimePosition() {return (std::clock() - startTime)/(double) CLOCKS_PER_SEC;}
    std::pair<int,int> getScreenSize(){return std::pair<int,int>(1280,720);}
    void resetTime() {startTime = std::clock();}
    std::set<std::string> findLastName() const;
    void extractVideos();

private:
    cv::Mat frame_to_mat(const rs2::frame& f);
    cv::Mat depth_frame_to_meters(const rs2::pipeline& pipe, const rs2::depth_frame& f);
    void saveVideo(std::string bagLoc);
    filesystem::path createDirectories(std::string bagLoc);
    int extractFrame(filesystem::path directory);



    cv::Mat colorFeed;
    cv::Mat depthFeed;
    cv::Mat depthMeters;
    rs2::pipeline_profile pipeline_profile;
    rs2::colorizer color_map;
    std::shared_ptr<rs2::pipeline> pipe;
    rs2::frameset data;
    rs2::align align_to;
    int last_frame_number;
    double startTime;
    double endTime;
    filesystem::path bag_file;
    std::string homedir;
    filesystem::path directory;
    std::vector<int32_t> params;


};
#endif

#include "RealSense.h"


RealSense::RealSense(): align_to(RS2_STREAM_COLOR){

    rs2::config cfg;
    pipe = std::make_shared<rs2::pipeline>();
    startTime = std::clock();
    //Add desired streams to configuration
    cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH,1280,720,RS2_FORMAT_Z16);
    pipe->start(cfg);
    last_frame_number = 0;
    for(int i = 0; i < 30; i++)
    {
        //Wait for all configured streams to produce a frame, otherwise the images sent will be a bit weird.
        data = pipe->wait_for_frames();
    }
    params = { cv::IMWRITE_JPEG_QUALITY, 99 };

    struct passwd *pw = getpwuid(getuid());

    const char *dir = pw->pw_dir;
    homedir = dir;
    homedir = homedir + "/Pictures/RGBDRec";
    filesystem::path fs = homedir;
    if (!filesystem::exists(fs))
        filesystem::create_directories(homedir);
    homedir = homedir + "/";
}

RealSense::~RealSense() {
    pipe->stop();
}

bool RealSense::startRecording(std::string folderLoc) {
    endTime = std::clock();
    pipe->stop();
    pipe = std::make_shared<rs2::pipeline>();
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_RGB8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH,1280,720,RS2_FORMAT_Z16);

    cfg.enable_record_to_file(homedir + folderLoc+".bag");
    pipe->start(cfg);
    for(int i = 0; i < 30; i++)
    {
        //Wait for all configured streams to produce a frame, otherwise the images sent will be a bit weird.
        data = pipe->wait_for_frames();
    }
}

bool RealSense::stopRecording(std::string folderLoc,bool extractFrames) {
    rs2::config cfg;
    startTime = std::clock();
    pipe->stop();

    if (extractFrames)
        this->saveVideo(homedir + folderLoc);

    pipe = std::make_shared<rs2::pipeline>();

    cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH,1280,720,RS2_FORMAT_Z16);
    pipe->start(cfg);
    last_frame_number = 0;

    for(int i = 0; i < 30; i++)
    {

        data = pipe->wait_for_frames();
    }

}

/**
 * @brief RealSense::saveVideo
 * @param bagLoc
 * This is used to convert the video to image file for the training phase.
 * Modify the code here if you wish to save/delete the intermediate bag files.
 */
void RealSense::saveVideo(std::string bagLoc) {

    rs2::config config;
    rs2::context context;
    const rs2::playback playback = context.load_device(bagLoc);
    const std::vector<rs2::sensor> sensors = playback.query_sensors();
    for( const rs2::sensor& sensor : sensors ){
        const std::vector<rs2::stream_profile> stream_profiles = sensor.get_stream_profiles();
        for( const rs2::stream_profile& stream_profile : stream_profiles ){
            config.enable_stream( stream_profile.stream_type(), stream_profile.stream_index() );
        }
    }
    pipe = std::make_shared<rs2::pipeline>();

    // Start Pipeline
    config.enable_device_from_file( playback.file_name() );
    pipeline_profile = pipe->start( config );

    // Set Non Real Time Playback to make sure that no frame is lost.
    // Update : Non real time is broken right now on the software side. Changed to true until a patch
    auto profile = pipeline_profile.get_device().as<rs2::playback>();
    profile.set_real_time(true);

    auto dir = this->createDirectories(bagLoc);
    pipe->wait_for_frames();
    int maxFrame = -1;
    int frame = -1;
    while (frame >= maxFrame) {
        frame = this->extractFrame(dir);
        maxFrame = std::max(maxFrame,frame);
    }
    pipe->stop();

    std::cout << "check";
}


filesystem::path RealSense::createDirectories(std::string bagLoc)
{
    bagLoc = bagLoc.substr(0, bagLoc.length()-4);
    filesystem::path directory = bagLoc;

    if (!filesystem::exists(directory)) {
        if( !filesystem::create_directories( directory ) ){
            throw std::runtime_error( "Folder n'a pu etre cree." );
        }
    }
    else
        return directory;

    const std::vector<rs2::stream_profile> stream_profiles = pipeline_profile.get_streams();
    for( const rs2::stream_profile stream_profile : stream_profiles ){
        filesystem::path sub_directory = directory.generic_string() + "/" + stream_profile.stream_name();
        filesystem::create_directories( sub_directory );
    }
    return directory;
}


void RealSense::extractVideos() {

    pipe->stop();
    for(filesystem::directory_entry p: filesystem::directory_iterator(homedir))
    {
        if (filesystem::is_regular_file(p.symlink_status())){
            std::string filename = p.path().u8string();
            std::string ext = p.path().extension().u8string();
            if (ext == ".bag") {
                this->saveVideo(filename);
            }
        }
    }

}

std::set<std::string> RealSense::findLastName() const{

    std::set<std::string> name;
    for(filesystem::directory_entry p: filesystem::directory_iterator(homedir))
    {
        if (filesystem::is_regular_file(p.symlink_status())){
            std::string filename = p.path().stem().u8string();
            name.insert(filename);
        }
    }
    return name;
}
/*
int RealSense::findLastName(){

    int name = 0;
    for(filesystem::directory_entry p: filesystem::directory_iterator(homedir))
    {
        if (filesystem::is_regular_file(p.symlink_status())){
            std::string filename = p.path().stem().u8string();
            char* p;
            strtol(filename.c_str(), &p, 10);
            if (!*p) {
               int pos = std::stoi(filename);
               name = std::max(pos,name);
            }
        }
    }
    return name;
}*/

/**
 * @brief RealSense::extractFrame
 * @param directory
 * @return the frame number in the entire data frame set.
 * Extracts the color and depth frame. Modify here is you wish to save the infrared frame too.
 */
int RealSense::extractFrame(filesystem::path directory) {
    data = pipe->wait_for_frames();

    auto color_frame = data.get_color_frame();
    if( !color_frame ){
        return 0;
    }

    auto color_width = color_frame.as<rs2::video_frame>().get_width();
    auto color_height = color_frame.as<rs2::video_frame>().get_height();

    auto color_mat = frame_to_mat(color_frame);


    // Create the color frame name to save it.
    std::ostringstream oss;
    oss << directory.generic_string() << "/Color/";
    oss << std::setfill( '0' ) << std::setw( 6 ) << color_frame.get_frame_number() << ".jpg";

    // Save the Color Image
    cv::imwrite( oss.str(), color_mat, params );

    auto depth_frame = data.get_depth_frame();
    if( !depth_frame ){
        return color_frame.get_frame_number();
    }

    auto depth_width = depth_frame.as<rs2::video_frame>().get_width();
    auto depth_height = depth_frame.as<rs2::video_frame>().get_height();

    auto scale_mat = frame_to_mat(depth_frame);

    // Create the depth frame name to save it.
    oss.clear();
    oss.str("");
    oss << directory.generic_string() << "/Depth/";
    oss << std::setfill( '0' ) << std::setw( 6 ) << depth_frame.get_frame_number() << ".png";

    //cv::Mat scale_mat = depth_mat;
    //if( scaling ){
    //   depth_mat.convertTo( scale_mat, CV_8U, -255.0 / 10000.0, 255.0 ); // 0-10000 -> 255(white)-0(black)
    //}

    // Save the Color Image
    cv::imwrite( oss.str(), scale_mat );

    return std::max(depth_frame.get_frame_number(),color_frame.get_frame_number());


}



cv::Mat RealSense::getColorFeed() {
    return this->colorFeed;
}

cv::Mat RealSense::getDepthFeed() {
    return this->depthFeed;
}

cv::Mat RealSense::getMappedFeed() {
    return this->depthMeters;
}

void RealSense::update() {

    data = pipe->wait_for_frames(); // Wait for next set of frames from the camera
    data = align_to.process(data);

    auto color_frame = data.get_color_frame();
    //rs2::frame depth = color_map.colorize(data.get_depth_frame());

    if (color_frame.get_frame_number() == last_frame_number) return;
    last_frame_number = color_frame.get_frame_number();

    //colorFeed.release();
    colorFeed = this->frame_to_mat(color_frame);


}

/**
 * @brief RealSense::frame_to_mat
 * @param f
 * @return Image matrix
 * Convert a Realsense frame to an Opencv image.
 */
cv::Mat RealSense::frame_to_mat(const rs2::frame& f)
{
    using namespace cv;
    using namespace rs2;


    auto vf = f.as<video_frame>();
    const int w = 1280;//vf.get_width();
    const int h = 720;//vf.get_height();

    if (f.get_profile().format() == RS2_FORMAT_BGR8)
    {
        return Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), Mat::AUTO_STEP);
    }
    else if (f.get_profile().format() == RS2_FORMAT_RGB8)
    {
        auto r = Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), Mat::AUTO_STEP);
        cv::cvtColor(r, r, cv::COLOR_BGR2RGB);
        return r;
    }
    else if (f.get_profile().format() == RS2_FORMAT_Z16)
    {
        return Mat(Size(w, h), CV_16UC1, (void*)f.get_data(), Mat::AUTO_STEP);
    }
    else if (f.get_profile().format() == RS2_FORMAT_Y8)
    {
        return Mat(Size(w, h), CV_8UC1, (void*)f.get_data(), Mat::AUTO_STEP);;
    }

    throw std::runtime_error("Frame format is not supported yet!");
}

/**
 * @brief RealSense::depth_frame_to_meters
 * @param pipe
 * @param f
 * @return
 * Converts depth frame to a matrix of doubles with distances in meters.
 * Legacy code.
 */
cv::Mat RealSense::depth_frame_to_meters(const rs2::pipeline& pipe, const rs2::depth_frame& f)
{
    using namespace cv;
    using namespace rs2;

    Mat dm = frame_to_mat(f);
    dm.convertTo(dm, CV_64F);
    auto depth_scale = pipe.get_active_profile()
            .get_device()
            .first<depth_sensor>()
            .get_depth_scale();
    dm = dm * depth_scale;



    return dm;
}






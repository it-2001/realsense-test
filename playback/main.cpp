#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
// #include <openpose/headers.hpp>
// #include <openpose/flags.hpp>

#include <iostream>
#include <string>

// program pro zpracovani bag souboru
//
// momentalne: precte bag soubor a posle color frame do openpose
//             pro odhadnuti postavy. Zaroven vypisuje pozici 
//             nejvzdalenejsiho bodu + vzdalenost a index snimku.
//             S vygenerovanou postavou se zatim nic nedela
// 
// Z testovacich duvodu vytvori video "video.avi" s obsahem color bufferu
// 
// problem:Rrealsense se snazi podavat snimky v realnem case, 
//          to vetsinou neni problem, ale obcas se stane, ze
//          preskoci jeden nebo vice snimku, protoze zpracovani
//          minuleho trvalo prilis dlouho
// 
// reseni: momentalne zadne, nutnost detailnejsiho vyzkumu
// 
// Daniel Antos
//


int main() {
    // op::Wrapper opWrapper{ op::ThreadManagerMode::Asynchronous };
    // opWrapper.disableMultiThreading();
    // opWrapper.start();
    // Declare the pipeline and pipeline profile
    rs2::pipeline pipe;
    rs2::pipeline_profile profile;

    // Specify the path to the bag file
    std::string bagFilePath = "../../bgr.bag";
    std::string winName = "imaaag";
    std::string frames_dir = "../frames/";

    // Create a configuration for the pipeline
    rs2::config cfg;

    // Enable all streams from the bag file
    cfg.enable_device_from_file(bagFilePath, false);

    // Start the pipeline with the configuration and get the pipeline profile
    profile = pipe.start(cfg);
    rs2::video_stream_profile color_profile = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
    int color_width = color_profile.width();
    int color_height = color_profile.height();
    //rs2::video_stream_profile color_profile = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
    //int color_width = color_profile.width();
    //int color_height = color_profile.height();
    int fps = color_profile.fps();
    
    auto device = pipe.get_active_profile().get_device();
    auto playback = device.as<rs2::playback>();
    playback.set_real_time(false);
    

    cv::Mat image;
    cv::Mat depth_image;
    cv::VideoWriter video;
    std::string outputVideo = "video.avi";
    video.open(outputVideo, cv::VideoWriter::fourcc('X','V','I','D'), fps/2, cv::Size(color_width, color_height));
    
    // cv::namedWindow(winName);

    // Main loop
    while (true) {
        // Get frames from the bag file
        rs2::frameset frameset;
        if (!pipe.try_wait_for_frames(&frameset)) {
            break;
        }
        // Get the depth frame
        rs2::depth_frame depth_frame = frameset.get_depth_frame();
        if (!depth_frame) {
            break;
        }
        rs2::frame color_frame = frameset.get_color_frame();
        if (!color_frame) {
            break;
        }
        auto timestamp = frameset.get_frame_timestamp_domain();

        // Get the width and height of the depth frame
        int width = depth_frame.get_width();
        int height = depth_frame.get_height();

        // Print the distance for the furthest pixel


        auto imageData = color_frame.get_data();
        auto deptData = depth_frame.get_data();
        
        auto fnum = depth_frame.get_frame_number();

        
        image = cv::Mat(cv::Size(color_width, color_height), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
        if (image.empty()) {
            break;
        }
        depth_image = cv::Mat(cv::Size(color_width, color_height), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);


        video.write(image);
        imwrite(frames_dir + "f" + std::to_string((int)fnum) + ".png", image);
        imwrite(frames_dir + "d" + std::to_string((int)fnum) + ".tif", depth_image);
        // imshow(winName, image);


        // openpose matrix for pose estimation
        // const op::Matrix imageToProcess = OP_CV2OPCONSTMAT(image);

        // estimated pose
        // auto datumProcessed = opWrapper.emplaceAndPop(imageToProcess);


        // TODO: do something with the pose
        // ...
            
    }
    

    // Stop the pipeline
    video.release();
    pipe.stop();
    // cv::destroyAllWindows();

    return 0;
}



// timestamp
// encoding
// confidence
// syncer
 

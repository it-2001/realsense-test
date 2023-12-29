#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <openpose/headers.hpp>
#include <openpose/flags.hpp>

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
    op::Wrapper opWrapper{ op::ThreadManagerMode::Asynchronous };
    opWrapper.disableMultiThreading();
    opWrapper.start();
    // Declare the pipeline and pipeline profile
    rs2::pipeline pipe;
    rs2::pipeline_profile profile;

    // Specify the path to the bag file
    std::string bagFilePath = "file.bag";

    // Create a configuration for the pipeline
    rs2::config cfg;

    // Enable all streams from the bag file
    cfg.enable_device_from_file(bagFilePath);

    // Start the pipeline with the configuration and get the pipeline profile
    profile = pipe.start(cfg);
    rs2::video_stream_profile color_profile = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
    int color_width = color_profile.width();
    int color_height = color_profile.height();
    int fps = color_profile.fps();
    rs2::frameset frameset;
    pipe.try_wait_for_frames(&frameset);
    rs2::depth_frame depth_frame = frameset.get_depth_frame();

    // search here: [[DUPLICATE_FRAMES]]
    auto first_frame = depth_frame.get_frame_number();
    auto last_frame = depth_frame.get_frame_number();

    std::cout << "first frame: " << first_frame << std::endl;

    cv::Mat image;
    cv::VideoWriter video;
    std::string outputVideo = "video.avi";
    video.open(outputVideo, cv::VideoWriter::fourcc('X','V','I','D'), fps, cv::Size(color_width, color_height));

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

        // Get the width and height of the depth frame
        int width = depth_frame.get_width();
        int height = depth_frame.get_height();

        // Initialize variables to store the furthest pixel coordinates and distance
        int furthest_x = 0;
        int furthest_y = 0;
        float furthest_distance = 0.0f;

        // [[DUPLICATE_FRAMES]]
        // sometimes realsense returns duplicate frames
        // maybe this is intentional, needs further research
        // TODO: find better method of getting frames
        auto fnum = depth_frame.get_frame_number();
        // end of file
        // may fail to break if pipe skips first frame on next iteration
        if (fnum == first_frame) {
            break;
        }
        // duplicate frame
        else if (fnum == last_frame) {
            continue;
        }
        last_frame = fnum;


        // Iterate through all pixels in the depth frame
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Get the distance for the current pixel
                float distance = depth_frame.get_distance(x, y);

                // Check if the current pixel is further than the previous furthest pixel
                if (distance > furthest_distance) {
                    furthest_x = x;
                    furthest_y = y;
                    furthest_distance = distance;
                }
            }
        }

        // Print the distance for the furthest pixel
        std::cout << "Furthest pixel coordinates: (" << furthest_x << ", " << furthest_y << ")"
            << " Distance: " << furthest_distance << " meters, frame: " << fnum << std::endl;


        auto imageData = color_frame.get_data();

        
        image = cv::Mat(cv::Size(color_width, color_height), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
        if (image.empty()) {
            break;
        }


        video.write(image);


        // openpose matrix for pose estimation
        const op::Matrix imageToProcess = OP_CV2OPCONSTMAT(image);

        // estimated pose
        auto datumProcessed = opWrapper.emplaceAndPop(imageToProcess);


        // TODO: do something with the pose
        // ...
            
    }

    // Stop the pipeline
    video.release();
    pipe.stop();
    cv::destroyAllWindows();

    return 0;
}
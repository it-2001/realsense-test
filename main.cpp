#include <librealsense2/rs.hpp>
#include <iostream>

int main() {
    // Declare the pipeline and pipeline profile
    rs2::pipeline pipe;
    rs2::pipeline_profile profile;

    // Specify the path to the bag file
    std::string bagFilePath = "../file.bag";

    // Create a configuration for the pipeline
    rs2::config cfg;

    // Enable all streams from the bag file
    cfg.enable_device_from_file(bagFilePath);

    // Start the pipeline with the configuration and get the pipeline profile
    profile = pipe.start(cfg);
    rs2::frameset frameset;
    pipe.try_wait_for_frames(&frameset);
    rs2::depth_frame depth_frame = frameset.get_depth_frame();

    // Get the width and height of the depth frame
    auto first_frame = depth_frame.get_frame_number();
    auto last_frame = depth_frame.get_frame_number();
    
    std::cout << "first frame: " << first_frame << std::endl;
    

    // Main loop
    while (true) {
        // Get frames from the bag file
        rs2::frameset frameset;
        if (pipe.try_wait_for_frames(&frameset)) {
            // Get the depth frame
            rs2::depth_frame depth_frame = frameset.get_depth_frame();

            // Get the width and height of the depth frame
            int width = depth_frame.get_width();
            int height = depth_frame.get_height();

            // Initialize variables to store the furthest pixel coordinates and distance
            int furthest_x = 0;
            int furthest_y = 0;
            float furthest_distance = 0.0f;
            
            auto fnum = depth_frame.get_frame_number();
                    
            if (fnum == first_frame) {
                break;
            }else if (fnum == last_frame){
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

        }
    }

    // Stop the pipeline
    pipe.stop();

    return 0;
}

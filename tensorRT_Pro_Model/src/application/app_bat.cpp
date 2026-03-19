#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <boost/format.hpp>
#include "app_tracker/BAT.hpp"

namespace fs = std::filesystem;

cv::Mat get_x_frame(const std::string& color_path, const std::string& depth_path, 
                    const std::string& dtype = "rgbcolormap", bool depth_clip = false) {
    
    cv::Mat rgb, dp;    
    // Read color image
    if (!color_path.empty()) {
        rgb = cv::imread(color_path);
        if (!rgb.empty()) {
            cv::cvtColor(rgb, rgb, cv::COLOR_BGR2RGB);
        }
    }    
    // Read depth image
    if (!depth_path.empty()) {
        dp = cv::imread(depth_path, -1); // Read with unchanged depth
        if (!dp.empty() && depth_clip) {
            // Convert to float for processing
            cv::Mat dp_float;
            dp.convertTo(dp_float, CV_32F);            
            // Calculate median
            cv::Mat flat = dp_float.reshape(1, dp_float.total());
            cv::Mat sorted;
            cv::sort(flat, sorted, cv::SORT_EVERY_COLUMN);
            double median = sorted.at<float>(sorted.rows / 2, 0);            
            // Calculate max_depth = min(median * 3, 10000)
            float max_depth = std::min(static_cast<float>(median * 3), 10000.0f);            
            // Clip values
            cv::Mat mask = dp_float > max_depth;
            dp_float.setTo(max_depth, mask);            
            // Convert back to original type
            dp_float.convertTo(dp, dp.type());
        }
    }    
    // Check if images were loaded
    if (rgb.empty() || dp.empty()) {
        std::cout << "***** " << color_path << std::endl;
        std::cout << "***** " << depth_path << std::endl;
    }    
    cv::Mat img;    
    if (dtype == "color") {
        img = rgb.clone();
    }
    else if (dtype == "rgbrgb") {
        // Convert depth to RGB if it's grayscale
        if (dp.channels() == 1) {
            cv::cvtColor(dp, dp, cv::COLOR_GRAY2RGB);
        } else if (dp.channels() == 3) {
            cv::cvtColor(dp, dp, cv::COLOR_BGR2RGB);
        }        
        // Merge RGB and depth (converted to RGB)
        std::vector<cv::Mat> channels;
        cv::split(rgb, channels);        
        std::vector<cv::Mat> dp_channels;
        cv::split(dp, dp_channels);
        
        // Add depth channels to the vector
        channels.insert(channels.end(), dp_channels.begin(), dp_channels.end());        
        cv::merge(channels, img);
    }
    else {
        std::cout << "No such dtype !!! " << std::endl;
        img = cv::Mat();
    }   
    return img;
}

void save_matrix_to_file(const std::string& filename, const cv::Mat& matrix) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return;
    }   
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            if (matrix.type() == CV_32F) {
                file << matrix.at<float>(i, j);
            } else if (matrix.type() == CV_64F) {
                file << matrix.at<double>(i, j);
            } else if (matrix.type() == CV_32S) {
                file << matrix.at<int>(i, j);
            }
            
            if (j < matrix.cols - 1) {
                file << " ";  // Space delimiter (or use "," for comma)
            }
        }
        file << std::endl;
    }    
    file.close();
}

std::tuple<std::vector<std::string>, std::vector<std::string>, 
           cv::Mat, cv::Mat> genConfig(const std::string& seq_path, const std::string& set_type) {
    
    std::vector<std::string> RGB_img_list, T_img_list;
    cv::Mat RGB_gt, T_gt;    
    if (set_type == "RGBT234" || set_type == "LasHeR") {
        // Get visible (RGB) image list
        std::string visible_path = seq_path + "/visible/";
        if (fs::exists(visible_path)) {
            for (const auto& entry : fs::directory_iterator(visible_path)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    if (ext == ".jpg" || ext == ".JPG") {
                        RGB_img_list.push_back(entry.path().string());
                    }
                }
            }
            std::sort(RGB_img_list.begin(), RGB_img_list.end());
        }       
        // Get infrared (T) image list
        std::string infrared_path = seq_path + "/infrared/";
        if (fs::exists(infrared_path)) {
            for (const auto& entry : fs::directory_iterator(infrared_path)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    if (ext == ".jpg" || ext == ".JPG") {
                        T_img_list.push_back(entry.path().string());
                    }
                }
            }
            std::sort(T_img_list.begin(), T_img_list.end());
        }        
        // Load ground truth files
        auto load_gt = [](const std::string& filename) -> cv::Mat {
            std::ifstream file(filename);
            std::vector<std::vector<float>> data;
            std::string line;           
            if (file.is_open()) {
                while (std::getline(file, line)) {
                    std::vector<float> row;
                    std::stringstream ss(line);
                    std::string value;
                    
                    while (std::getline(ss, value, ',')) {
                        try {
                            row.push_back(std::stof(value));
                        } catch (const std::exception& e) {
                            std::cerr << "Error parsing value: " << value << std::endl;
                        }
                    }
                    if (!row.empty()) {
                        data.push_back(row);
                    }
                }
                file.close();
            }            
            if (data.empty()) {
                return cv::Mat();
            }            
            // Convert to cv::Mat
            int rows = data.size();
            int cols = data[0].size();
            cv::Mat gt(rows, cols, CV_32F);
            
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    gt.at<float>(i, j) = data[i][j];
                }
            }           
            return gt;
        };
        
        RGB_gt = load_gt(seq_path + "/visible.txt");
        T_gt = load_gt(seq_path + "/infrared.txt");
    }
    return std::make_tuple(RGB_img_list, T_img_list, RGB_gt, T_gt);
}

void run_sequence(const std::string& engine_path, 
                    const std::string& seq_home, 
                    const std::string& seq_name,
                    const std::string& dataset_name,
                    const std::string& yaml_name = "default"
                  ) {
        
        std::string save_name = yaml_name;
        std::string base_path = "./RGBT_workspace/results/" + dataset_name + "/";
        std::string save_folder = base_path + save_name;
        std::string save_path = save_folder + "/" + seq_name + ".txt";
        
        // Check if save_folder exists, if not create it
        try {
            if (!fs::exists(save_folder)) {
                std::cout << "Creating directory: " << save_folder << std::endl;
                fs::create_directories(save_folder);  // Creates parent directories if needed
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error creating directory: " << e.what() << std::endl;
            return;
        }        
        // Check if save_path already exists
        if (fs::exists(save_path)) {
            std::cout << "-1 " << seq_name << std::endl;
            return;  // Early return if file already exists
        }        
        // Create tracker
        auto tracker = VOT::create_tracker(engine_path);
        if(tracker == nullptr){
            printf("tracker is nullptr.\n");
            return;
        }
        
        // Construct sequence path
        std::string seq_path = seq_home + "/" + seq_name;        
        // Print processing message
        std::cout << "—————————— Process sequence: " << seq_name << " ——————————————" << std::endl;
        
        auto [rgb_list, t_list, rgb_gt, t_gt] = genConfig(seq_path, dataset_name);
        cv::Mat result;       
        // Check if number of images matches ground truth
        if (rgb_list.size() == static_cast<size_t>(rgb_gt.rows)) {
            // Create result matrix with same size as ground truth
            result = cv::Mat::zeros(rgb_gt.size(), rgb_gt.type());
        } else {
            // Create result matrix with image count rows and 4 columns
            result = cv::Mat::zeros(static_cast<int>(rgb_list.size()), 4, rgb_gt.type());
        }        
        // Copy first ground truth frame to result
        if (rgb_gt.rows > 0) {
            rgb_gt.row(0).copyTo(result.row(0));
        }
        double toc = 0.0;
        size_t frame_idx = 0;   
        // Process each frame
        for (frame_idx = 0; frame_idx < std::min(rgb_list.size(), t_list.size()); ++frame_idx) {
            auto tic = std::chrono::high_resolution_clock::now();            
            // Get XTYPE from config (default to "rgbrgb")
            std::string xtype = "rgbrgb";            
            if (frame_idx == 0) {
                // Initialization frame
                cv::Mat image = get_x_frame(rgb_list[frame_idx], 
                                        t_list[frame_idx], 
                                        xtype);                
                // Convert ground truth to vector
                std::vector<float> init_bbox(4);
                for (int i = 0; i < 4; ++i) {
                    init_bbox[i] = rgb_gt.at<float>(0, i);  // Assuming float type, adjust if needed
                }                
                tracker->init(image, init_bbox);                
            } else if (frame_idx > 0) {
                // Tracking frames
                cv::Mat image = get_x_frame(rgb_list[frame_idx], 
                                        t_list[frame_idx], 
                                        xtype);
               
                cv::Rect region = tracker->track(image);                
                // Store result
                result.at<float>(frame_idx, 0) = region.x;
                result.at<float>(frame_idx, 1) = region.y;
                result.at<float>(frame_idx, 2) = region.width;
                result.at<float>(frame_idx, 3) = region.height;
            }            
            auto toc_duration = std::chrono::high_resolution_clock::now() - tic;
            toc += std::chrono::duration<double>(toc_duration).count();
        }
        // Calculate FPS
        double fps = frame_idx / toc;       
        // Save results if not in debug mode
        if (!save_path.empty()) {
            save_matrix_to_file(save_path, result);
        }        
        std::cout << seq_name << " , fps:" << fps << std::endl;
        std::cout << "-1 " << seq_name << std::endl;
    }


int main(int argc, char* argv[]) {
    
    std::string engine_path = (argc > 1) ? argv[1] : "";
    std::string dataset_name = (argc > 2) ? argv[2] : "RGBT234";
    std::string seq_home;
    
    if(dataset_name == "RGBT234") {
        seq_home = "/media/Datasets/Tracking/DATASET_TEST/RGBT234";
    } else {
        std::cerr << "Unknown dataset: " << dataset_name << std::endl;
        return 1;
    }    
    std::vector<std::string> seq_list;
    if (!fs::exists(seq_home) || !fs::is_directory(seq_home)) {
            std::cerr << "Directory does not exist: " << seq_home << std::endl;
            return 1;
    }    
    // Iterate through directory entries
    for (const auto& entry : fs::directory_iterator(seq_home)) {
        if (entry.is_directory()) {
            // Get the directory name (last part of path)
            std::string dir_name = entry.path().filename().string();
            seq_list.push_back(dir_name);
        }
    }       
    // Sort the list alphabetically
    std::sort(seq_list.begin(), seq_list.end());    
    for(auto &seq : seq_list) {
        run_sequence(engine_path, seq_home, seq, dataset_name);
    }    
    return 0;
}
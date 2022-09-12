#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <chrono>


int main() {
    //input
    const cv::Mat orig_image = cv::imread("../front_upper_dust_mono_image.png", cv::IMREAD_GRAYSCALE);
    if (orig_image.empty()) {
        std::cout << "empty" << std::endl;
        return -1;
    }
//    //binary_entropy(histogram)
//    const int channels[] = {0};
//    std::vector<uint> mono_plane;
//    const float range[] = {0, 256};
//    const float *ranges[] = {range};
//    cv::Mat hist;
//    const int bins = 256;
//    const int hdims[] = {bins};
//    const int ch_height = 256;
//    const int ch_width = 256;
////    cv::Mat hist_img(cv::Size(ch_width,ch_height),CV_8UC3,cv::Scalar::all(255));
//    double max_val;
//    cv::calcHist(&orig_image, 1, 0, cv::Mat(), hist, 1, hdims, ranges);
    //binary_entropy(histogram)

    //画像の領域分割(参照のみで可
    auto start_time = std::chrono::system_clock::now();
    uint vertical_cut_num = 6;//縦方向の切る回数
    uint horizontal_cut_num = 6;//横方向の切る回数

    std::vector<cv::Point> edge_points;
    cv::Point p;
    uint cut_width = orig_image.cols / vertical_cut_num;
    uint cut_height = orig_image.rows / horizontal_cut_num;
    std::cout << "orig_x " << orig_image.cols << "\t orig_y " << orig_image.rows << std::endl;

    for (uint i = 0; i < vertical_cut_num; i++) {//ちゃんと任意の切断数で動くようにする.
        p = {i * cut_width, 0};
        edge_points.emplace_back(p);
    }
    for (int i = 0; i < horizontal_cut_num; i++) {
        edge_points.at(i).y = i * cut_height;
    }

    std::vector<cv::Mat> rois;
    for (int i = 0; i < horizontal_cut_num; ++i) {
        for (int j = 0; j < vertical_cut_num; ++j) {
            cv::Mat var_roi;
            var_roi = orig_image(cv::Rect(j * cut_width, i * cut_height, cut_width, cut_height));
            rois.emplace_back(var_roi);
        }
    }

    std::cout << "cut image num is " << rois.size() << std::endl;


    // calc binary binary_entropy
    std::vector<double> binary_entropies;
    for (int i = 0; i < rois.size(); i++) {
        int num_of_zero_pixel = 0;
        int num_of_non_zero_pixels = 0;
        double prob_of_zero = 0.0;
        double prob_of_nonzero = 0.0;
        double binary_entropy = 0.0;
        for (int ite_row = 0; ite_row < rois.at(i).rows; ite_row++) {
            for (int ite_col = 0; ite_col < rois.at(i).cols; ite_col++) {
                if (rois.at(i).at<unsigned short>(ite_row, ite_col) == 0) {
                    num_of_zero_pixel++;
                } else {
                    num_of_non_zero_pixels++;
                }
            }
        }
        std::cout << "rows " << rois.at(i).rows << std::endl;
        std::cout << "cols " << rois.at(i).cols << std::endl;
        std::cout << "rows*cols is " << rois.at(i).rows * rois.at(i).cols << std::endl;
        std::cout << "ROI" << i << " zero num is " << num_of_zero_pixel << "non zero num is " << num_of_non_zero_pixels
                  << std::endl;
        prob_of_zero = (double) num_of_zero_pixel / rois.at(i).cols / rois.at(i).rows;
        prob_of_nonzero = (double) num_of_non_zero_pixels / rois.at(i).cols / rois.at(i).rows;
        std::cout << "ROI" << i << " prob zero  is " << prob_of_zero << "prob non zero is " << prob_of_nonzero
                  << std::endl;
        if ((prob_of_nonzero != 0) & (prob_of_zero != 0)) {
            binary_entropy = (prob_of_zero * log2(prob_of_zero) + prob_of_nonzero * log2(prob_of_nonzero)) * (-1);

        } else {
            binary_entropy = 0.0;
        }
        binary_entropies.emplace_back(binary_entropy);

    }
    /// calc binary binary_entropy

    auto end_time = std::chrono::system_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();


    for (int i = 0; i < rois.size(); i++) {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(3) << i;
        cv::imwrite("../outputs/entropy/roi_" + oss.str() + ".png", rois.at(i));
        std::cout << "../outputs/entropy/roi_" + oss.str() + ".png is writed! binary_entropy is"
                  << binary_entropies.at(i) << std::endl;
    }
    std::cout << "calc entropy time is " << elapsed_time << "[ms]" << std::endl;

//    for (auto &prob: probabilities) {
//        if (prob != 0) {
//            binary_entropy += prob * log2(prob);
//        }
//    }
//    RCLCPP_WARN(get_logger(), "binary_entropy is %lf", binary_entropy);


    std::cout << "Process end!" << std::endl;


    return 0;
}

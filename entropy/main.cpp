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

    //画像の領域分割(参照のみで可
    auto start_time = std::chrono::system_clock::now();
    uint vertical_cut_num = 16;//縦方向の切る回数
    uint horizontal_cut_num = 6;//横方向の切る回数

    std::vector<cv::Point> edge_points;
    cv::Point p;
    uint cut_width = orig_image.cols / vertical_cut_num;
    uint cut_height = orig_image.rows / horizontal_cut_num;
    std::cout << "orig_x " << orig_image.cols << "\t orig_y " << orig_image.rows << std::endl;

    std::vector<cv::Mat> rois;
    for (int i = 0; i < horizontal_cut_num + 1; ++i) {//ちゃんと任意の切断数で動くようにする.
        for (int j = 0; j < vertical_cut_num + 1; ++j) {
            p = {j * cut_width, i * cut_height}; //左上のはず
            edge_points.emplace_back(p);
            cv::Mat var_roi;
            if (((orig_image.cols - j * cut_width) < cut_width) &&
                ((orig_image.rows - i * cut_height) < cut_height)) {
                std::cout << "Pt.d" << std::endl;
                var_roi = orig_image(
                        cv::Rect(p.x, p.y, orig_image.cols - j * cut_width, orig_image.rows - i * cut_height));
            } else if ((orig_image.cols - j * cut_width) < cut_width) {
                std::cout << "Pt.c" << std::endl;
                var_roi = orig_image(cv::Rect(p.x, p.y, orig_image.cols - j * cut_width, cut_height));
            } else if ((orig_image.rows - i * cut_height) < cut_height) {
                std::cout << "Pt.b" << std::endl;
                var_roi = orig_image(
                        cv::Rect(p.x, p.y, cut_width, orig_image.rows - i * cut_height));
            } else {
                std::cout << "Pt.a" << std::endl;
                var_roi = orig_image(cv::Rect(p.x, p.y, cut_width, cut_height));
            }
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

    //エントロピーのしきい値を超えたら色塗り
    cv::Mat result_color_img = orig_image.clone();
    cv::applyColorMap(result_color_img, result_color_img, cv::COLORMAP_JET);

    cv::Mat result_bin_img(cv::Size(orig_image.cols, orig_image.rows), CV_8U, cv::Scalar(255)); //for mask？

    std::cout << "edge_points.size() is" << edge_points.size() << std::endl;
    std::cout << "binary_entropies.size() is" << binary_entropies.size() << std::endl;

    for (int it = 0; it < binary_entropies.size(); ++it) {
        if (binary_entropies.at(it) > 0.93) {
            cv::rectangle(result_color_img, edge_points.at(it),
                          cv::Point(edge_points.at(it).x + cut_width, edge_points.at(it).y + cut_height),
                          cv::Scalar(255), 2);
        }
    }


    cv::imwrite("../outputs/entropy/result_bin.png", result_bin_img);
    cv::imwrite("../outputs/entropy/result_colored.png", result_color_img);


    std::cout << "Process end!" << std::endl;


    return 0;
}

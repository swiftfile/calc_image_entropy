#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <chrono>

using namespace cv;

int main() {
    //input
    const cv::Mat orig_image = cv::imread("../front_upper_dust_mono_image.png", cv::IMREAD_GRAYSCALE);
    if (orig_image.empty()) {
        std::cout << "empty" << std::endl;
        return -1;
    }

    //make rois
    uint vertial_cut_num = 6;//縦方向の切る回数
    uint horizontal_cut_num = 6;//横方向の切る回数
    cv::Point p;
    std::vector<cv::Point> edge_points;
    int cut_width = orig_image.cols / vertial_cut_num;
    int cut_height = orig_image.rows / horizontal_cut_num;

    for (int i = 0; i < vertial_cut_num; i++) {
        p = {i * cut_width, 0};
        edge_points.emplace_back(p);
    }
    for (int i = 0; i < horizontal_cut_num; i++) {
        edge_points.at(i).y = i * cut_height;
    }
//    std::vector<cv::Mat> rois;
//    for (int i = 0; i < horizontal_cut_num; ++i) {
//        for (int j = 0; j < vertial_cut_num; ++j) {
//            cv::Mat var_roi;
//            var_roi = orig_image(cv::Rect(j * cut_width, i * cut_height, cut_width, cut_height));
//            rois.emplace_back(var_roi);
//        }
//    }
//    std::cout << "cut image num is " << rois.size() << std::endl;
    auto start_time = std::chrono::system_clock::now();
    //calc fft
    //    for (int ite = 0; ite < rois.size(); ite++) {
    Mat var_img = orig_image.clone();
    Mat padded;
    int m = getOptimalDFTSize(var_img.rows);
    int n = getOptimalDFTSize(var_img.cols);
    copyMakeBorder(var_img, padded, 0, m - var_img.rows, 0, n - var_img.cols, BORDER_CONSTANT,
                   Scalar::all(0));
    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);
    split(complexI, planes);
    magnitude(planes[0], planes[1], planes[0]);
    Mat magI = planes[0];
    magI += Scalar::all(1);
    log(magI, magI);
//    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
    int cx = magI.cols / 2;
    int cy = magI.rows / 2;
    Mat q0(magI, Rect(0, 0, cx, cy));
    Mat q1(magI, Rect(cx, 0, cx, cy));
    Mat q2(magI, Rect(0, cy, cx, cy));
    Mat q3(magI, Rect(cx, cy, cx, cy));
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
    normalize(magI, magI, 0, 1, NORM_MINMAX);
    resize(var_img, var_img, cv::Size(), 3, 3);
    resize(magI, magI, cv::Size(), 3, 3);
    imshow("Input", var_img);
    imshow("spectrum magnitude", magI);
    cv::waitKey();
    Mat result_img = magI.clone();
    result_img.convertTo(result_img, CV_8UC1, 255);
    std::ostringstream oss;
//        oss << std::setfill('0') << std::setw(3) << ite;
    cv::imwrite("../outputs/fft/result.png", result_img);
//        cv::imwrite("../outputs/fft/result_roi_" + oss.str() + ".png", result_img);
    std::cout << "image is saved!" << std::endl;
//    }
    auto end_time = std::chrono::system_clock::now();
    double elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "elapsed time is " << elapsed_time << "[ms]" << std::endl;
    std::cout << "Process end!" << std::endl;
    return 0;
}

#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <chrono>

using namespace cv;


int main() {
    //input
    cv::Mat orig_img = cv::imread("../front_upper_dust_mono_image.png", cv::IMREAD_GRAYSCALE);
//    cv::Mat orig_img = cv::imread("../soccer.jpg", cv::IMREAD_GRAYSCALE);
    if (orig_img.empty()) {
        std::cout << "empty" << std::endl;
        return -1;
    }
    std::cout << orig_img.type() << std::endl;


    //make rois
    auto start_time = std::chrono::system_clock::now();
    Mat var_img = orig_img.clone();


    resize(orig_img, orig_img, cv::Size(), 3, 3);
//    resize(sobeled_img, sobeled_img, cv::Size(), 3, 3);

//    namedWindow("mag_img");
//    imshow("mag_img", magnitude_img);
//    namedWindow("IDFT");
//    imshow("IDFT", idft_img);
//    namedWindow("binary");
//    imshow("binary", binary_img);
//    namedWindow("Sobel");
//    imshow("Sobel", sobeled_img);
    cv::Mat dist;
    Mat bw;
//    cvtColor(orig_img,bw,COLOR_BGR2GRAY);
    bw = orig_img.clone();
    threshold(bw, bw, 1, 255, THRESH_BINARY | cv::THRESH_OTSU);
    cv::distanceTransform(bw, dist, DIST_L2, 3);
    normalize(dist, dist, 0, 1.0, NORM_MINMAX);


    auto end_time = std::chrono::system_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    std::cout << "calc distance time is " << elapsed_time << "[us]" << std::endl;
    namedWindow("original");
    imshow("original", orig_img);
    imshow("binary image", bw);

    imshow("Distanceimg", dist);

    cv::waitKey();

    std::ostringstream oss;
//        oss << std::setfill('0') << std::setw(3) << ite;
//    cv::imwrite("../outputs/fft-sobel/result.png", result_img);
//    cv::imwrite("../outputs/fft-sobel/sobel.png", sobeled_img);
//        cv::imwrite("../outputs/fft-sobel/result_roi_" + oss.str() + ".png", result_img);
    std::cout << "img is saved!" << std::endl;
//    }

    return 0;
}

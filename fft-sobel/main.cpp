#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <chrono>

using namespace cv;

void shiftDft(Mat &src_img, Mat &dst_img) {
    Mat tmp;
    int cx = src_img.cols / 2;
    int cy = src_img.rows / 2;

    for (int i = 0; i <= cx; i += cx) {
        Mat qs(src_img, Rect(i ^ cx, 0, cx, cy));
        Mat qd(dst_img, Rect(i, cy, cx, cy));
        qs.copyTo(tmp);
        qd.copyTo(qs);
        tmp.copyTo(qd);
    }
}


void createCompleximgForDft(const Mat &complex_img, Mat &magnitude_img) {

    Mat real_img;

    copyMakeBorder(complex_img, real_img, 0, getOptimalDFTSize(complex_img.rows) - complex_img.rows, 0,
                   getOptimalDFTSize(complex_img.cols) - complex_img.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat planes[] = {Mat_<float>(real_img), Mat::zeros(real_img.size(), CV_32F)};
    merge(planes, 2, magnitude_img);

}

void createFourierMagnitude(const Mat &complex_img, Mat &dst_img) {
    Mat planes[2];
    split(complex_img, planes);

    magnitude(planes[0], planes[1], dst_img);

    dst_img += Scalar::all(1);
    log(dst_img, dst_img);

    dst_img = dst_img(Rect(0, 0, dst_img.cols & -2, dst_img.rows & -2));

    const int half_width = dst_img.cols / 2;
    const int half_height = dst_img.rows / 2;

    Mat tmp;
    int cx = dst_img.cols / 2;
    int cy = dst_img.rows / 2;
    Mat q0(dst_img, Rect(0, 0, cx, cy));
    Mat q1(dst_img, Rect(cx, 0, cx, cy));
    Mat q2(dst_img, Rect(0, cy, cx, cy));
    Mat q3(dst_img, Rect(cx, cy, cx, cy));
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    normalize(dst_img, dst_img, 0, 1, NORM_MINMAX);
}

void createInverseFrourierImg(const Mat &mag_img, unsigned int orig_cols, unsigned int orig_rows, cv::Mat &dst_img) {

    Mat splitted_img[2];
    split(mag_img, splitted_img);
    splitted_img[0](cv::Rect(0, 0, orig_cols, orig_rows)).copyTo(dst_img);
    cv::normalize(dst_img, dst_img, 0, 1, NORM_MINMAX);

}


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

    //make binary img
    Mat binary_img = orig_img.clone();
    inRange(binary_img, 0, 1, binary_img);


//    Mat complex_img;
//    createCompleximgForDft(orig_img, complex_img);
//    cv::dft(complex_img, complex_img);
//    cv::Mat magnitude_img;
//    createFourierMagnitude(complex_img, magnitude_img);
//
//    idft(complex_img, complex_img);

    Mat sobeled_img;
    Sobel(binary_img, sobeled_img, CV_8UC1, 1, 0, 3);
    Sobel(sobeled_img, sobeled_img, CV_8UC1, 0, 1, 3);

    //仮で窓関数作ってみる
//    Mat window_img(complex_img.rows,complex_img.cols,CV_32F);
//    cv::bitwise_and()

    //sobelしたやつにモルフォロジーかける
//    Mat molpho_img;
//    uint erode_kernel = 3;
//    Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(erode_kernel + 1, 2 * erode_kernel + 1),
//                                        Point(erode_kernel, erode_kernel));

//    Mat idft_img;

    auto end_time = std::chrono::system_clock::now();
    double elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "elapsed time is " << elapsed_time << "[us]" << std::endl;
    std::cout << "Process end!" << std::endl;

//    createInverseFrourierImg(complex_img, orig_img.cols, orig_img.rows, idft_img);

    resize(orig_img, orig_img, cv::Size(), 3, 3);
    resize(sobeled_img, sobeled_img, cv::Size(), 3, 3);
    namedWindow("original");
    imshow("original", orig_img);

//    namedWindow("mag_img");
//    imshow("mag_img", magnitude_img);
//    namedWindow("IDFT");
//    imshow("IDFT", idft_img);
//    namedWindow("binary");
//    imshow("binary", binary_img);
    namedWindow("Sobel");
    imshow("Sobel", sobeled_img);

//    resize(inversed_img, inversed_img, cv::Size(), 3, 3);
//    imshow("Input", var_img);
//    imshow("spectrum magnitude", magI);
//    imshow("decoded img", inversed_img);
    cv::waitKey();
//    result_img.convertTo(result_img, CV_8UC1, 255);
    std::ostringstream oss;
//        oss << std::setfill('0') << std::setw(3) << ite;
//    cv::imwrite("../outputs/fft-sobel/result.png", result_img);
    cv::imwrite("../outputs/fft-sobel/sobel.png", sobeled_img);
//        cv::imwrite("../outputs/fft-sobel/result_roi_" + oss.str() + ".png", result_img);
    std::cout << "img is saved!" << std::endl;
//    }

    return 0;
}

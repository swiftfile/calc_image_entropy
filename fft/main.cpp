#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <chrono>

using namespace cv;

int main() {
    //input
    const cv::Mat input_image = cv::imread("../front_upper_dust_mono_image.png", cv::IMREAD_GRAYSCALE);
    if (input_image.empty()) {
        std::cout << "empty" << std::endl;
        return -1;
    }

    //画像の領域分割(参照のみで可
    auto start_time = std::chrono::system_clock::now();
    Mat padded;
    int m = getOptimalDFTSize(input_image.rows);
    int n = getOptimalDFTSize(input_image.cols);
    copyMakeBorder(input_image, padded, 0, m - input_image.rows, 0, n - input_image.cols, BORDER_CONSTANT,
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

    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

    int cx = magI.cols / 2;
    int cy = magI.rows / 2;

    Mat q0(magI, Rect(0, 0, cx, cy));
    Mat q1(magI, Rect(cx, 0, cx, cy));
    Mat q2(magI, Rect(cx, cy, cx, cy));
    Mat q3(magI, Rect(cx, cy, cx, cy));

    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    normalize(magI, magI, 0, 1, NORM_MINMAX);

    imshow("Input", input_image);
    imshow("spectrum magnitude", magI);
    Mat result_img = magI.clone();
    imshow("spectrum magnitude2", result_img);
    waitKey();

//    auto end_time = std::chrono::system_clock::now();
//    double elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
//    std::cout << "elapsed time is " << elapsed_time << "[ms]" << std::endl;
    result_img.convertTo(result_img,CV_8UC1,255);
    cv::imwrite("../outputs/fft/result.png", result_img);
    std::cout << "image is saved!" << std::endl;
    std::cout << "Process end!" << std::endl;
    return 0;
}

#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
// opencv
#include <opencv2/core/version.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/freetype.hpp"

using namespace cv;
using namespace std;

void overlayImage_PNG(const cv::Mat &background, const cv::Mat &foreground,
                      cv::Mat &output, cv::Point2i location)
{
    background.copyTo(output);

    // start at the row indicated by location, or at row 0 if location.y is negative.
    for (int y = std::max(location.y, 0); y < background.rows; ++y)
    {
        int fY = y - location.y; // because of the translation

        // we are done of we have processed all rows of the foreground image.
        if (fY >= foreground.rows)
            break;

        // start at the column indicated by location,

        // or at column 0 if location.x is negative.
        for (int x = std::max(location.x, 0); x < background.cols; ++x)
        {
            int fX = x - location.x; // because of the translation.

            // we are done with this row if the column is outside of the foreground image.
            if (fX >= foreground.cols)
                break;

            // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
            double opacity =
                ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

                / 255.;

            // and now combine the background and foreground pixel, using the opacity,

            // but only if opacity > 0.
            // opacity = 0.7;
            for (int c = 0; opacity > 0 && c < output.channels(); ++c)
            {
                unsigned char foregroundPx =
                    foreground.data[fY * foreground.step + fX * foreground.channels() + c];
                unsigned char backgroundPx =
                    background.data[y * background.step + x * background.channels() + c];
                output.data[y * output.step + output.channels() * x + c] =
                    backgroundPx * (1. - opacity) + foregroundPx * opacity;
            }
        }
    }
}

int main()
{
    Mat in_mat = imread("/works/cpp_pngwriter/png/room_sample.jpg", IMREAD_UNCHANGED);
    Mat png_background = imread("/works/cpp_pngwriter/png/room_info.png", IMREAD_UNCHANGED);
    Mat png_roomnumber = imread("/works/cpp_pngwriter/png/room6.png", IMREAD_UNCHANGED);

    static Ptr<freetype::FreeType2> ft2;
    ft2 = freetype::createFreeType2();
    ft2->loadFontData("/works/cpp_pngwriter/font/SCDream5.otf", 0);

    static Ptr<freetype::FreeType2> ft3;
    ft3 = freetype::createFreeType2();
    ft3->loadFontData("/works/cpp_pngwriter/font/SCDream3.otf", 0);

    Mat text_area(Size(200, 350), CV_8UC3, Scalar(0, 0, 0));
    // ft2->putText(text_area, "9", Point(110, 0), 60, Scalar(255, 255, 255), -1, LINE_AA, false);
    // ft2->putText(text_area, "30", Point(75, 0), 60, Scalar(255, 255, 255), -1, LINE_AA, false);
    // ft2->putText(text_area, "1", Point(105, 130), 60, Scalar(255, 255, 255), -1, LINE_AA, false);
    // ft2->putText(text_area, "18", Point(65, 130), 60, Scalar(255, 255, 255), -1, LINE_AA, false);
    // ft2->putText(text_area, "180", Point(28, 130), 60, Scalar(255, 255, 255), -1, LINE_AA, false);
    ft2->putText(text_area, "4", Point(130, 260), 60, Scalar(255, 255, 255), -1, LINE_AA, false);
    // ft2->putText(text_area, "45", Point(95, 260), 60, Scalar(255, 255, 255), -1, LINE_AA, false);
    Mat gray_area;
    cv::cvtColor(text_area, gray_area, cv::COLOR_RGB2GRAY);
    cv::Mat srcImg[] = {text_area, gray_area};

    cv::Mat abgr(text_area.rows, text_area.cols, CV_8UC4);
    int from_to[] = {0, 3, 1, 1, 2, 2, 3, 0};
    cv::mixChannels(srcImg, 2, &abgr, 1, from_to, 4);

    overlayImage_PNG(png_background, abgr, png_background, cv::Point(200, 29)); // 9kg

    overlayImage_PNG(in_mat, png_background, in_mat, cv::Point(14, 685));
    overlayImage_PNG(in_mat, png_roomnumber, in_mat, cv::Point(0, 0));

    imwrite("output.png", in_mat);

    return 0;
}
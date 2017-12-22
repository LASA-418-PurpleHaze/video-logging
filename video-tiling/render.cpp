#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace cv;

int main(int argc, char const *argv[]) {
  VideoCapture my_video = VideoCapture();
  std::cout << argv[0];
  my_video.open( argv[0] );
  Mat outp;
  namedWindow("window", CV_WINDOW_AUTOSIZE);
  while (true) {
    my_video >> outp;
    if (!outp.empty()) {
      imshow("window", outp);
    }
  }
  my_video.release();
  return 0;
}

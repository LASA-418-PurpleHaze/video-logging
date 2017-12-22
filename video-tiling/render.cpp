#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace cv;

int main(int argc, char const *argv[]) {
  VideoCapture my_video = VideoCapture( argv[0] );
  Mat outp(100,100,CV_8UC3);
  namedWindow("window", CV_WINDOW_AUTOSIZE);
  while (true) {
    my_video.read(outp);
    if (!outp.empty()) {
      imshow("window", outp);
    }
  }
  my_video.release();
  return 0;
}

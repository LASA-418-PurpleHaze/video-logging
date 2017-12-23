#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace cv;

int main(int argc, char const *argv[]) {
<<<<<<< HEAD
  VideoCapture my_video = VideoCapture( argv[0] );
  Mat outp(100,100,CV_8UC3);
  namedWindow("window", CV_WINDOW_AUTOSIZE);
  while (true) {
    my_video.read(outp);
    if (!outp.empty()) {
      imshow("window", outp);
=======
  VideoCapture cap( argv[1] );
  VideoCapture videoDos( argv[2] );
  
  Mat outframe;
  namedWindow("Viewer",1);
  for(;;)
  {
    Mat frame;
    Mat frame2;
    cap >> frame;
    videoDos >> frame2;
    if (!frame.empty() && !frame2.empty()) {
      hconcat(frame, frame2, outframe);
      imshow("Viewer", outframe);
>>>>>>> d6c1fc78d06f65c6d0ffcac331b9f09450f86a7e
    }
    else if(frame.empty() && !frame2.empty()) {
      imshow("Viewer", frame2);
    }
    else if(frame2.empty() && !frame.empty()) {
      imshow("Viewer", frame);
    }
    else {
      std::clog << "Video ended" << '\n';
      break;
    }
    if(waitKey(1) == 27) break;
  }
  return 0;
}

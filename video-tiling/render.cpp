#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace cv;

int main(int argc, char const *argv[]) {
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

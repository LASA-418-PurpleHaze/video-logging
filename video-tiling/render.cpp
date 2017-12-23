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
  Mat liveFrame, liveFrame2;
  for(;;)
  {
    Mat frame, frame2;
    cap >> frame, videoDos >> frame2;
    if(frame.empty() && frame2.empty()) {
      std::clog << "Video ended" << '\n';
      break;
    }
    else if(frame2.empty()) {
      liveFrame = frame.clone();
      liveFrame2.setTo(Scalar(0,0,255));
    }
    else if(frame.empty()) {
      liveFrame2 = frame2.clone();
      liveFrame.setTo(Scalar(0,0,255));
    }
    else {
      liveFrame = frame.clone();
      liveFrame2 = frame2.clone();
    }
    liveFrame2.resize(liveFrame.rows, Scalar(0,255,0));
    hconcat(liveFrame, liveFrame2, outframe);
    imshow("Viewer", outframe);
    if(waitKey(30) == 27) break;
  }
  return 0;
}

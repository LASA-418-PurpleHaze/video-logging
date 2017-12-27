#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace cv;

struct line_item {
  long timestamp;
  std::string ns;
  std::string key;
  std::string val;
  std::string toString() {
    return std::to_string(this->timestamp) + " " + this->ns + " " + this->key + " " + this->val;
  }
};

line_item read_in_line(std::ifstream &my_stream) {
  std::string beans, broccoli, carrots, celery;
  getline(my_stream, beans, ','), getline(my_stream, broccoli, ','), getline(my_stream, carrots, ','), getline(my_stream, celery);
  if (beans == "") {
    return line_item();
  }
  return { stol(beans), broccoli, carrots, celery };
}

void overlay_variables(Mat &manip, std::map<std::string, std::string> vars) {
  int i = 1;
  for(auto it = vars.cbegin(); it != vars.cend(); ++it)
  {
      std::string textAssembly = it->first + " - " + it->second;
      putText(manip, textAssembly, Point(manip.cols - 200,manip.rows - 100 + 15*i), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 2);
      i++;
  }
}

int main(int argc, char const *argv[]) {
  VideoCapture cap( argv[1] );
  VideoCapture videoDos( argv[2] );
  VideoWriter outvid = VideoWriter();
  Mat jeff = imread( "/home/cmk/Downloads/lr.png" );
  Mat jeff2;
  resize(jeff, jeff2, Size(60,60));
  
  std::ifstream my_data ( argv[3] );
  std::string throwaway;
  getline(my_data, throwaway);

  Mat outframe;
  namedWindow("Viewer",1);
  Mat liveFrame, liveFrame2, liveFrame2bordered;
  cap >> liveFrame, videoDos >> liveFrame2;
  copyMakeBorder(liveFrame2,liveFrame2bordered,3,3,3,3,BORDER_CONSTANT, Scalar(0,0,0));
  liveFrame2bordered.resize(liveFrame.rows, Scalar(0,255,0));
  hconcat(liveFrame, liveFrame2bordered, outframe);
  double fps_1 = cap.get(CAP_PROP_FPS);
  double fps_2 = videoDos.get(CAP_PROP_FPS);
  double my_fps = fps_1 >= fps_2 ? fps_1 : fps_2;
  bool fpsvid1 = (fps_1 == my_fps);
  line_item control = read_in_line(my_data);
  long start_point = control.timestamp;
  std::cout << control.toString() << '\n';
  outvid.open("video.avi", CV_FOURCC('M','J','P','G'), (int)(my_fps + 0.5), outframe.size());
  outvid.write(outframe);
  Mat frame, frame2;
  std::map<std::string, std::string> variables;
  line_item cur_event = read_in_line(my_data);
  long cur_timestamp;
  for(int i = 0;;i++)
  {
    double vid_loc1 = cap.get(CAP_PROP_POS_FRAMES)/cap.get(CAP_PROP_FPS) * 1000;
    double vid_loc2 = videoDos.get(CAP_PROP_POS_FRAMES)/videoDos.get(CAP_PROP_FPS) * 1000;
    if (fpsvid1) {
      cap >> frame;
      if(vid_loc1 >= vid_loc2) {
        videoDos >> frame2;
      }
      cur_timestamp = cur_event.timestamp - start_point;
      while (cur_timestamp < vid_loc1 && cur_event.key != "") {
        variables[cur_event.ns +":"+ cur_event.key] = cur_event.val;
        variables["cur_ts"] = std::to_string(cur_timestamp);
        cur_event = read_in_line(my_data);
        cur_timestamp = cur_event.timestamp - start_point;
      }
    }
    else {
      videoDos >> frame2;
      if(vid_loc2 >= vid_loc1) {
        cap >> frame;
      }
      cur_timestamp = cur_event.timestamp - start_point;
      while (cur_timestamp < vid_loc2 && cur_event.key != "") {
        variables[cur_event.key] = cur_event.val;
        variables["cur_ts"] = std::to_string(cur_timestamp);
        cur_event = read_in_line(my_data);
        cur_timestamp = cur_event.timestamp - start_point;
      }
    }
    if(frame.empty() && frame2.empty()) {
      std::clog << "Video ended" << '\n';
      break;
    }
    else if(frame2.empty()) {
      liveFrame = frame.clone();
      putText(liveFrame, std::to_string(cap.get(CAP_PROP_POS_FRAMES)/cap.get(CAP_PROP_FPS) * 1000), Point(50, liveFrame.rows - 20), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,255,255),5);
      liveFrame2.setTo(Scalar(0,0,255));
    }
    else if(frame.empty()) {
      liveFrame2 = frame2.clone();
      putText(liveFrame2, std::to_string(videoDos.get(CAP_PROP_POS_FRAMES)/videoDos.get(CAP_PROP_FPS) * 1000), Point(0, liveFrame2.rows - 20), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,255,255),5);
      liveFrame.setTo(Scalar(0,0,255));
    }
    else {
      liveFrame = frame.clone();
      putText(liveFrame, std::to_string(vid_loc1), Point(50, liveFrame.rows - 20), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,255,255),5);
      liveFrame2 = frame2.clone();
      putText(liveFrame2, std::to_string(vid_loc2), Point(0, liveFrame2.rows - 20), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,255,255),5);
    }
    Mat liveFrame2bordered;
    copyMakeBorder(liveFrame2,liveFrame2bordered,3,3,3,3,BORDER_CONSTANT, Scalar(0,0,0));
    liveFrame2bordered.resize(liveFrame.rows, Scalar(0,255,0));
    hconcat(liveFrame, liveFrame2bordered, outframe);
    Mat bottomLeft = outframe(Rect(outframe.cols - 60, outframe.rows - 60, 60, 60));
    double number = cap.get(CAP_PROP_POS_AVI_RATIO) * 100;
    std::cout << ">>>> " << number << "%" << '\r' << std::flush;
    bottomLeft = bottomLeft * .8 + jeff2 * .2;
    overlay_variables(outframe, variables);
    putText(outframe, std::to_string(cur_event.timestamp), Point(outframe.cols - 250, outframe.rows - 200), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 2);
    outvid.write(outframe);
  }
  outvid.release();
  return 0;
}

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace cv;

/// Global variables

Mat left_side, right_side;

/** @function main */
int main( int argc, char** argv )
{
  /// Load an image
  left_side = imread( argv[1] );
  right_side = left_side;
  if( !left_side.data )
  {std::cout << "**error** provide sample image" << std::endl; return -1; }

  /// Convert the image to grayscale
  cvtColor( left_side, right_side, CV_BGR2GRAY );

  ///now they aren't on the same channel type, can't concat,
  ///convert the right_side gray to bgr, will still be gray
	///but it'll have 3 channels, just like the left side
  cvtColor(right_side, right_side, CV_GRAY2BGR);

  /// Create a window
  namedWindow( "my window", CV_WINDOW_AUTOSIZE );
  ///create a matrix that'll hold the tiled images
  Mat combined;
  Mat combined_flipped;
  ///hconcat takes in input array 1 and 2, then the output array
  hconcat(left_side, right_side, combined);
  hconcat(right_side, left_side, combined_flipped);

  imshow("my window", combined);

  Mat resized_combined(500, 1000, CV_8UC3);
  Mat resized_combined_flipped(500, 1000, CV_8UC3);

  resize(combined, resized_combined, resized_combined.size(), 0,0,CV_INTER_LINEAR);
  resize(combined_flipped, resized_combined_flipped, resized_combined_flipped.size(), 0, 0, CV_INTER_LINEAR);

  imshow("my window", resized_combined);	
 
  Mat quad_combined;
  
  vconcat(resized_combined, resized_combined_flipped, quad_combined);

  imshow("my other window", quad_combined);

  Mat big_boi(1000, 1000, CV_8UC3);
  resize(left_side, big_boi, big_boi.size(), 0, 0, CV_INTER_LINEAR);
  cvtColor(big_boi, big_boi, CV_BGR2HSV);
  Mat mega_big_boi(1000, 2000, CV_8UC3);
  hconcat(big_boi, quad_combined, mega_big_boi);

  imshow("mega big boi window", mega_big_boi);  
  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <memory>

#include "Processor.h"
#include "CombinedProcessor.h"

using namespace cv;

int main(int argc,  char** argv) {
  if(argc < 2) {
    std::cout << "Usage: main_pi [input_image] [output_image]" << std::endl;
    return 0;
  }
	FrameProcessor* processor = new Processor();
  
  //std::cout << "Reading input from " << argv[1] << std::endl;
	Mat frame = imread(argv[1]);
  if (!frame.empty())
    processor->Process(frame);
  else {
    std::cout << "Error reading file " << argv[1] << std::endl;
    return 1;
  }

  //std::cout << "Writing output to " << argv[2] << std::endl;
  char name[1000];
  sprintf(name, "output\\%s.out.jpg", argv[1]);
  imwrite(name, processor->get_img());
	return 0;
}

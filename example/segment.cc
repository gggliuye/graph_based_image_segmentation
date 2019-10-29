#include "graphBasedSegmentation.h"


// /home/viki/Documents/ImageSegmentation/indoor1.jpg

int main(int argc, char* argv[])   
{   
    if (argc != 2) {
      fprintf(stderr, "usage: %s input(image)\n", argv[0]);
      return 1;
    }

    cv::Mat image = cv::imread(argv[1]);
    if(image.empty()){
        fprintf(stderr,"Image not found. \n");
        return 1;
    }
    
    int num_class;
    GraphBasedSegmentation graphBasedSegmentation(image,0.5,600,20);


    return 0;   
}

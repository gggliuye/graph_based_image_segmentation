#include "graph.h"
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

// ./run_cut /home/viki/Documents/ImageSegmentation/indoor1.jpg

template <class T>
T square(T x)
{
    return x*x;
}

int TestOffical();

void graphCut(cv::Mat image);

int main(int argc, char* argv[]) 
{
    if (argc != 2) {
      fprintf(stderr, "usage: %s input(image)\n", argv[0]);
      return 1;
    }

    TestOffical();

    cv::Mat image = cv::imread(argv[1]);
    if(image.empty()){
        fprintf(stderr,"Image not found. \n");
        return 1;
    }

    graphCut(image);

    return 0;
}

bool useHSV = true;

float calculateWeight(cv::Mat &r,cv::Mat &g,cv::Mat &b, int i, int j , int u, int v)
{
    float weight;
    if(useHSV){
        weight = square(r.at<float>(i,j) - r.at<float>(u,v))*2
                     + square(g.at<float>(i,j) - g.at<float>(u,v))*0.5
                     + square(b.at<float>(i,j) - b.at<float>(u,v))*0.5;
    }else{
        weight = square(r.at<float>(i,j) - r.at<float>(u,v))
                     + square(g.at<float>(i,j) - g.at<float>(u,v))
                     + square(b.at<float>(i,j) - b.at<float>(u,v));
    }
    return exp( - weight/1.4);

}

void graphCut(cv::Mat image)
{
    int cols = image.cols;
    int rows = image.rows;

    int numNodes = cols * rows;
    int numEdges = (cols-2) * (rows - 2) * 4 // for the center part
                   + (3+2)*(cols-1) + 1      // for the upper and down two rows
                   + (4+1) * (rows - 2);     // for the left and right two cols

    printf("There are %d nodes and %d edges.\n", numNodes, numEdges );

    cv::Mat imageOrigin;
    // convert image to float32
    image.convertTo(imageOrigin, CV_32F, 1, 0);

    if(useHSV){
        cvtColor(imageOrigin,imageOrigin,CV_BGR2HSV);
    }

    std::vector<cv::Mat> channels;
    cv::split(imageOrigin, channels);
    cv::Mat imageB = channels.at(0);
    cv::Mat imageG = channels.at(1);
    cv::Mat imageR = channels.at(2);


    // initialize the graph
    typedef Graph<float,float,float> GraphType;
    GraphType *g = new GraphType(/*estimated # of nodes*/ numNodes, /*estimated # of edges*/ numEdges); 

    // add the nodes
    for(int i = 0; i < rows; i ++){
        for(int j = 0; j < cols; j++){
            g -> add_node(); 
        }
    }

    // add the edges
    int centerRange = 40;
    int outerRange = 150;
    int a1 = rows/2 - centerRange - 20;
    int a2 = rows/2 + centerRange + 80;
    int b1 = cols/2 - centerRange + 20;
    int b2 = cols/2 + centerRange;

    int c1 = rows/2 - outerRange;
    int c2 = rows/2 + outerRange;
    int d1 = cols/2 - outerRange;
    int d2 = cols/2 + outerRange;
    printf("%d, %d, %d, %d.  %d * %d \n", a1, a2, b1, b2,rows, cols);
    for(int i = 0; i < rows; i ++){
        for(int j = 0; j < cols; j++){

            int index = cols * i + j;

            if(a1 < i && i < a2 && b1 < j && j < b2)
	            g -> add_tweights( index,   /* capacities */  1000000, 0 );
            else if( d1 > j || j > d2 || c1 > i || i > c2)
                g -> add_tweights( index,   /* capacities */  0, 1000000 );
            else
                g -> add_tweights( index,   /* capacities */  0, 0 );

            if(j < cols - 1){
                int index2 = cols * i + j + 1;
                float weight = calculateWeight(imageR,imageG,imageB,i,j ,i,j+1);
                g -> add_edge( index, index2,    /* capacities */  weight, weight );
                //printf("weight %f \n", weight);
            }

            if(i < rows - 1){
                int index2 = cols * (i + 1) + j;
                float weight = calculateWeight(imageR,imageG,imageB,i,j ,i+1,j);
                g -> add_edge( index, index2,    /* capacities */  weight, weight );
            }

            if((j < cols - 1) && (i < rows - 1) ){
                int index2 = cols * (i+1) + j + 1;
                float weight = calculateWeight(imageR,imageG,imageB,i,j ,i+1,j+1);
                g -> add_edge( index, index2,    /* capacities */  weight, weight );
            }

            if((j < cols - 1) && (i > 0) ){
                int index2 = cols * (i-1) + j + 1;
                float weight = calculateWeight(imageR,imageG,imageB,i,j ,i-1,j+1);
                g -> add_edge( index, index2,    /* capacities */  weight, weight );
            }
        }
    }

    int flow = g -> maxflow();

    printf("Flow = %d\n", flow);
	printf("Minimum cut:\n");
    int backcount = 0;
    // retrive the nodes
    for(int i = 0; i < rows; i ++){
        for(int j = 0; j < cols; j++){
            int index = cols * i + j;
            if (g->what_segment(index) == GraphType::SOURCE){
                //gray.at<float>(i,j) = 255;
            } else {
                backcount++;
                //gray.at<float>(i,j) = 0;
                image.at<cv::Vec3b>(i,j)[0] = 0;
                image.at<cv::Vec3b>(i,j)[1] = 0;
                image.at<cv::Vec3b>(i,j)[2] = 0;
            }
        }
    }

	delete g;

    printf("There are %d background pixels. \n", backcount);

    cv::imwrite("/home/viki/Documents/ImageSegmentation/cuthsv.png", image);

    cv::line(image, cv::Point2f(b1,a1), cv::Point2f(b1,a2), cv::Scalar( 0, 255, 0), 2 );
    cv::line(image, cv::Point2f(b1,a1), cv::Point2f(b2,a1), cv::Scalar( 0, 255, 0), 2 );
    cv::line(image, cv::Point2f(b2,a2), cv::Point2f(b1,a2), cv::Scalar( 0, 255, 0), 2 );
    cv::line(image, cv::Point2f(b2,a2), cv::Point2f(b2,a1), cv::Scalar( 0, 255, 0), 2 );

    cv::line(image, cv::Point2f(d1,c1), cv::Point2f(d1,c2), cv::Scalar( 0, 0, 255), 2 );
    cv::line(image, cv::Point2f(d1,c1), cv::Point2f(d2,c1), cv::Scalar( 0, 0, 255), 2 );
    cv::line(image, cv::Point2f(d2,c2), cv::Point2f(d1,c2), cv::Scalar( 0, 0, 255), 2 );
    cv::line(image, cv::Point2f(d2,c2), cv::Point2f(d2,c1), cv::Scalar( 0, 0, 255), 2 );

    cv::imwrite("/home/viki/Documents/ImageSegmentation/cuthsv_guide.png", image);

    cv::imshow("result", image);
    cv::waitKey(0);

}

/*
		        SOURCE
		       /       \
		     2/         \8
		     /      1    \
		   node0 -----> node1
		     |   <-----   |
		     |      2     |
		     \            /
		     5\          /6
		       \        /
		          SINK

*/
int TestOffical()
{
	typedef Graph<int,int,int> GraphType;
	GraphType *g = new GraphType(/*estimated # of nodes*/ 2, /*estimated # of edges*/ 1); 

	g -> add_node(); 
	g -> add_node(); 

	g -> add_tweights( 0,   /* capacities */  2, 5 );
	g -> add_tweights( 1,   /* capacities */  8, 6 );
	g -> add_edge( 0, 1,    /* capacities */  1, 2 );

	int flow = g -> maxflow();

	printf("Flow = %d\n", flow);
	printf("Minimum cut:\n");
	if (g->what_segment(0) == GraphType::SOURCE)
		printf("node0 is in the SOURCE set\n");
	else
		printf("node0 is in the SINK set\n");
	if (g->what_segment(1) == GraphType::SOURCE)
		printf("node1 is in the SOURCE set\n");
	else
		printf("node1 is in the SINK set\n");

	delete g;

	return 0;
}


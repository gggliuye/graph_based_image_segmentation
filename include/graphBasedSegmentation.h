#ifndef GRAPH_BASED_SEGMENTATION_H
#define GRAPH_BASED_SEGMENTATION_H

#include <vector>

#include <opencv2/opencv.hpp>
#include <highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <types.hpp>

class GraphBasedSegmentation
{
public:
    GraphBasedSegmentation(cv::Mat &image, float sigma_, float c_, int min_size_);
    ~GraphBasedSegmentation();

public:
    void buildSegmentationGraph();

    void buildSegmentationGraphKNN();

    void assignEdgeWeight(edge *edge_i);

    void segmentGraph();

    void postProcessComponents();

    void drawOutput();

    void saveOutput(char *path);

private:
    bool isGray;
    bool useHSV = true;
    cv::Mat imageOrigin;
    cv::Mat imageOutput;

    // used when it is a color image, empty if input a gray image
    cv::Mat imageB, imageG, imageR;

    float sigma;
    float c;
    int min_size;
    int num_class;
    int cols, rows;

    edge *edges;
    int edge_count;
    ComponentTree *componentTree;

};



#endif // #ifndef GRAPH_BASED_SEGMENTATION_H

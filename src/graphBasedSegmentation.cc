#include "graphBasedSegmentation.h"

template <class T>
T square(T x)
{
    return x*x;
}

GraphBasedSegmentation::GraphBasedSegmentation(cv::Mat &image, 
                  float sigma_, float c_, int min_size_)
{
    if(image.channels() == 3){
        fprintf(stderr," Input color image. \n");
        isGray = false;
    } else if(image.channels() == 1){
        fprintf(stderr," Input gray image. \n");
        isGray = true;
    } else{
        fprintf(stderr,"Image error, cannot decode type. \n");
        return;
    }

    sigma = sigma_;
    c = c_;
    min_size = min_size_;

    cv::GaussianBlur(image, imageOrigin, cv::Size(3, 3), sigma, sigma);
    if(!isGray && useHSV){
        cvtColor(imageOrigin,imageOrigin,CV_BGR2HSV);
    }

    cols = imageOrigin.cols;
    rows = imageOrigin.rows;

    // convert image to float32
    imageOrigin.convertTo(imageOrigin, CV_32F, 1, 0);
 
    if(!isGray){
        std::vector<cv::Mat> channels;
        cv::split(imageOrigin, channels);
        imageB = channels.at(0);
        imageG = channels.at(1);
        imageR = channels.at(2);
    }

    // imageOrigin type is CV_8UC3
    fprintf(stderr," Image size : %d * %d. \n", imageOrigin.rows, imageOrigin.cols);
  
    // build the component tree
    componentTree = new ComponentTree(imageOrigin.cols * imageOrigin.rows, c);

    buildSegmentationGraph();

    segmentGraph();

    postProcessComponents();

    drawOutput();
}

GraphBasedSegmentation::~GraphBasedSegmentation() {
    delete [] edges;
    delete componentTree;
}
#if 0
void GraphBasedSegmentation::assignEdgeWeight(edge *edge_i)
{
    float weight;
    if(isGray){
        weight = square(imageOrigin.at<uchar>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                             -imageOrigin.at<uchar>(edge_i->pixel_j.i, edge_i->pixel_j.j));
    } else{
        weight = square(imageR.at<uchar>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                             -imageR.at<uchar>(edge_i->pixel_j.i, edge_i->pixel_j.j));
        weight += square(imageG.at<uchar>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                        -imageG.at<uchar>(edge_i->pixel_j.i, edge_i->pixel_j.j));
        weight += square(imageB.at<uchar>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                        -imageB.at<uchar>(edge_i->pixel_j.i, edge_i->pixel_j.j));

    }
    edge_i->weight = sqrt(weight);
}
#endif
void GraphBasedSegmentation::assignEdgeWeight(edge *edge_i)
{
    float weight;
    if(isGray){
        weight = square(imageOrigin.at<float>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                       -imageOrigin.at<float>(edge_i->pixel_j.i, edge_i->pixel_j.j));
    } else{
        if(useHSV){
            weight = square((imageR.at<float>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                           -imageR.at<float>(edge_i->pixel_j.i, edge_i->pixel_j.j))*2);
            weight += square((imageG.at<float>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                            -imageG.at<float>(edge_i->pixel_j.i, edge_i->pixel_j.j))*0.3);
            weight += square((imageB.at<float>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                            -imageB.at<float>(edge_i->pixel_j.i, edge_i->pixel_j.j))*0.3);
        } else{
            weight = square(imageR.at<float>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                           -imageR.at<float>(edge_i->pixel_j.i, edge_i->pixel_j.j));
            weight += square(imageG.at<float>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                            -imageG.at<float>(edge_i->pixel_j.i, edge_i->pixel_j.j));
            weight += square(imageB.at<float>(edge_i->pixel_i.i, edge_i->pixel_i.j)
                            -imageB.at<float>(edge_i->pixel_j.i, edge_i->pixel_j.j));
        }
    }
    edge_i->weight = sqrt(weight);
    //fprintf(stderr, "%f \n", edge_i->weight);
}


void GraphBasedSegmentation::buildSegmentationGraph()
{
    int pixelsize = imageOrigin.cols * imageOrigin.rows;
    int initsize = pixelsize * 6;
    edges = new edge[initsize];

    int count = 0;
    int interval = 1;
    //int interval2 = 1;
    for(int i = 0; i < imageOrigin.rows; i++){
        for(int j = 0; j < imageOrigin.cols; j++){
/*
            if(j < imageOrigin.cols - interval2){
                edges[count].pixel_i.i = i;
                edges[count].pixel_i.j = j;
                edges[count].pixel_j.i = i;
                edges[count].pixel_j.j = j+interval2;
                assignEdgeWeight(&edges[count]);
                count++;
            }

            if(i < imageOrigin.rows - interval2){
                edges[count].pixel_i.i = i;
                edges[count].pixel_i.j = j;
                edges[count].pixel_j.i = i+interval2;
                edges[count].pixel_j.j = j;
                assignEdgeWeight(&edges[count]);
                count++;
            }
*/
            if(j < imageOrigin.cols - interval){
                edges[count].pixel_i.i = i;
                edges[count].pixel_i.j = j;
                edges[count].pixel_j.i = i;
                edges[count].pixel_j.j = j+interval;
                assignEdgeWeight(&edges[count]);
                count++;
            }

            if(i < imageOrigin.rows - interval){
                edges[count].pixel_i.i = i;
                edges[count].pixel_i.j = j;
                edges[count].pixel_j.i = i+interval;
                edges[count].pixel_j.j = j;
                assignEdgeWeight(&edges[count]);
                count++;
            }

            if((j < imageOrigin.cols - interval) && (i < imageOrigin.rows - interval) ){
                edges[count].pixel_i.i = i;
                edges[count].pixel_i.j = j;
                edges[count].pixel_j.i = i+interval;
                edges[count].pixel_j.j = j+interval;
                assignEdgeWeight(&edges[count]);
                count++;
            }

            if((j < imageOrigin.cols - interval) && (i > interval-1) ){
                edges[count].pixel_i.i = i;
                edges[count].pixel_i.j = j;
                edges[count].pixel_j.i = i-interval;
                edges[count].pixel_j.j = j+interval;
                assignEdgeWeight(&edges[count]);
                count++;
            }

        }
    }

    edge_count = count;
    fprintf(stderr, " Build graph -> Edge size : %d, Count : %d \n", initsize, count);
}


void GraphBasedSegmentation::segmentGraph()
{
    // sort edges by weight
    std::sort(edges, edges + edge_count);

    // for each edge, in non-decreasing weight order...
    for (int i = 0; i < edge_count; i++) {
        edge *edge_i = &edges[i];
    
        // components conected by this edge
        int idx_a = edge_i->pixel_i.i * cols + edge_i->pixel_i.j;
        int idx_b = edge_i->pixel_j.i * cols + edge_i->pixel_j.j;
        int a = componentTree->findParent(idx_a);
        int b = componentTree->findParent(idx_b);
        if (a != b) {
            if((edge_i->weight <= componentTree->getMInt(a)) &&
	       (edge_i->weight <= componentTree->getMInt(b))) {
	        componentTree->join(a, b, edge_i->weight);
            }
        }
    }

    fprintf(stderr, " Finish Graph Based Image Segmentation. Totally %d components. \n", componentTree->size());
}

void GraphBasedSegmentation::postProcessComponents()
{
    for (int i = 0; i < edge_count; i++) {
        edge *edge_i = &edges[i];
        int idx_a = edge_i->pixel_i.i * cols + edge_i->pixel_i.j;
        int idx_b = edge_i->pixel_j.i * cols + edge_i->pixel_j.j;
        int a = componentTree->findParent(idx_a);
        int b = componentTree->findParent(idx_b);
        if ((a != b) && ((componentTree->sizeOfComponent(a) < min_size) 
          || (componentTree->sizeOfComponent(b) < min_size)))
            componentTree->join(a, b, -1);
    }
    //delete [] edges;
    num_class = componentTree->size();
    fprintf(stderr, " Remove small components. Totally %d components. \n", componentTree->size());
}


void GraphBasedSegmentation::drawOutput()
{
    cv::Mat imageOutput = cv::Mat(rows,cols, CV_8UC3);

    // initial with random color
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            imageOutput.at<cv::Vec3b>(i,j)[0] = (uchar)random();
            imageOutput.at<cv::Vec3b>(i,j)[1] = (uchar)random();
            imageOutput.at<cv::Vec3b>(i,j)[2] = (uchar)random();
        }
    }
    // assign each pixel with the same color as its parent
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int comp = componentTree->findParent(i * cols + j);
            int x = comp/cols;
            int y = comp - x*cols;
            imageOutput.at<cv::Vec3b>(i,j)[0] = imageOutput.at<cv::Vec3b>(x,y)[0];
            imageOutput.at<cv::Vec3b>(i,j)[1] = imageOutput.at<cv::Vec3b>(x,y)[1];
            imageOutput.at<cv::Vec3b>(i,j)[2] = imageOutput.at<cv::Vec3b>(x,y)[2];
        }
    } 
    cv::imwrite("/home/viki/Documents/ImageSegmentation/indoor_res3.jpg", imageOutput);
    cv::imshow("test",imageOutput);
    cv::waitKey(0);
}

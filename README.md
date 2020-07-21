# graph_based_image_segmentation
graph_based_image_segmentation

Implementation of the segmentation algorithm described in:

Efficient Graph-Based Image Segmentation
Pedro F. Felzenszwalb and Daniel P. Huttenlocher
International Journal of Computer Vision, 59(2) September 2004.

And graph cut image background/foreground segmentation. Using maxflow by "http://pub.ist.ac.at/~vnk/software.html".

## dependence

need Opencv

## Build in linux

1) mkidr build

2) cd build

3) cmake ..

4) make

## Note
Implement the grid graph, didn't make Implementation of Nearest Neighbor Graph version.

## Example

 ./bin/run_seg /path/to/image.jpg
 
![image original](https://github.com/gggliuye/graph_based_image_segmentation/blob/master/images/indoor1.jpg)

result using RGB distance

![image original](https://github.com/gggliuye/graph_based_image_segmentation/blob/master/images/indoor_res1.jpg)

result using HSV space

![image original](https://github.com/gggliuye/graph_based_image_segmentation/blob/master/images/indoor_res3.jpg)


Graph cut result

![image original](https://github.com/gggliuye/graph_based_image_segmentation/blob/master/images/graphcutresult.jpg)

Matlab based L1 image filling

![image original](Matlab_l1/depth_result.jpg)


```latex
@inproceedings{graph_based_image_segmentation,
  author={Felzenszwalb P F, Huttenlocher D P. },
  booktitle={International journal of computer vision},
  title={Efficient graph-based image segmentation},
  year={2004}
}
```

#include <opencv2/core/core.hpp>

using namespace cv;

void createPOI(const Mat& src, const Mat& polyMat, Mat& dst)
{
    vector<Mat> array(3);

    for (int i=0; i<3; i++)
    {
        extractChannel(src, array[i], i);
        bitwise_and(array[i], polyMat, array[i]);
    }

    merge(array, dst);
}

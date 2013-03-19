/* Research Project 2012
 * by Dominique Hightower
 * trainer.cpp
 * purpose: to learn characteristics of sign types, given a csv file with all image names of that unique traffic sign
 *
 */

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace cv;

//square globals //, numPts = 3;
const char* sqrWnd = "Square Detection";
const char* triWnd = "Triangle Detection";
const char* cirWnd = "Circle Detection";
const char* octWnd = "Octagon Detection";

/// Global variables
std::vector<int> shape_count;
std::vector<std::vector<int> > color_count;

//// Color Fcns ////

/* Multiplies each color image to the saturation image & normalizes */
int normColor( const Mat& image );
/* Generates 4 color biased images */
void hueConversion( const Mat& image, vector<Mat> & colorImg, Mat& colorImgR, Mat& colorImgB, Mat& colorImgY );
/* Generates one saturation biased images */
void satConversion( const Mat& image, Mat& satImg );

/// Function headers
int identifySign(string imgName);
int normSignImg(string imgName, int color, int shape);

/// Find Shape Fcn
/* Returns the number for the shape */
int findShape(const Mat& image, int color);

///Square functions
double angle( Point pt1, Point pt2, Point pt0 );
int findPoly(const Mat image, vector<vector<Point> >& squares, int numPts, float ang, int check);
int findSquares( const Mat image, vector<vector<Point> >& squares, int numPts, float ang, int check );
void drawSquares( Mat image, const vector<vector<Point> >& squares, int numPts );

///Circle Funtions
int findCircles(const Mat image, vector<Vec3f>& circles );
void drawCircles(Mat image, const vector<Vec3f>& circles );

///Print Functions
void printUsage() {
    std::cerr << "Usage: ./trainer dataFile.csv " << std::endl;
}

/**
 * @function main
 */
int main( int argc, char** argv )
{
    if (argc != 2) {
        printUsage();
        exit(EXIT_FAILURE);
    }
    else
    {
        /// get file path
        std::string rawPath = argv[1];
        int lastindex = rawPath.find_last_of("/");
        std::string path = rawPath.substr(0, lastindex) + "/";
        //**//std::cout << "File Path: " << path << std::endl;
        std::ifstream filename(argv[1]);
        
        //// init color count
        for (int i=0; i<5; i++) {
            
            std::vector<int> shape_c;
            //// init shape count
            for (int j=0; j<5; j++) {
                shape_c.push_back(0);
            }
            color_count.push_back(shape_c);
        }
        //// init shape count
        for (int i=0; i<5; i++) {
            shape_count.push_back(0);
        }
        
        const char* k_col[] = {"red","blue","yellow", "black", "unknown"};
        const char* k_sha[] = {"square","octagon", "triangle", "circle", "unknown"};
        
        std::vector<std::string> known_colors(k_col, k_col + sizeof(k_col)/sizeof(k_col[0]));
        std::vector<std::string> known_shapes(k_sha, k_sha + sizeof(k_sha)/sizeof(k_sha[0]));
        std::vector<std::string> imgNames;
        if (filename.is_open()){
            
            //**//std::cout << "File opened correctly" << std::endl;
            std::string lineData;
            int numF=0, n=0;
            int MAX_FIELDS_PER_IMG = 8;
            std::vector<std::string> data(MAX_FIELDS_PER_IMG); //initialize to 0
            
            while (filename.good() )
            {
                //get line
                getline(filename, lineData);
                //cout << "Line " << numF << ": " << lineData << endl;
                
                /*** PARSE STRING ***/
                int i=0;
                std::stringstream ss(lineData);
                while (getline(ss, data[i], ';')) {
                    //std::cout << data[i] << std::endl;
                    i++;
                }
                /** STORE DATA **/
                if (numF!=0 /*&& numF < 50 && numF%2 == 0*/){ //16 for error checking
                    //init variables
                    if (data[0].size() > 4){
                        std::string imgName = path + data[0]; //jpg
                        imgNames.push_back(imgName);
                        //**//std::cout << imgName << std::endl;
                        int retVal;
                        retVal = identifySign(imgName);
                        if (retVal < 0){
                            //sign detection error
                            //**//std::cout << "error detecting sign or not able to classify sign" << std::endl;
                            int t = 0; //useless
                        }
                        else
                            //waitKey(0);
                            int t = 0; //useless
                    }
                }
                numF++; //counts the number of lines
            }
            filename.close();
            
            //find max color
            int maxCol = 0, maxSha = 0, maxCnt=0;
            
            for (std::vector< std::vector<int> >::size_type u = 0; u < color_count.size()-1; u++) {
                for (std::vector<int>::size_type v = 0; v < color_count[u].size()-1; v++) {
                    if (color_count[u][v] > maxCnt) {
                        maxCnt = color_count[u][v];
                        maxCol = u;
                        maxSha = v;
                    }
                    //std::cout << "[" << color_count[u][v] << "]";
                }
                //std::cout << std::endl;
            }
            
            // ** Print Results ** 
            std::cout << known_colors[maxCol] << "," << known_shapes[maxSha] << std::endl; //
            
            //std::cout << "\nRESULTS" << std::endl;
            //std::cout << "-color: " << known_colors[ maxCol ] << std::endl;
            //std::cout << "-shape: " << known_shapes[ maxSha ] << std::endl;
            //std::cout << maxCol << "," << maxSha << std::endl;
            /*float tot = numF - 1;
            std::cout << "\nCOLOR" << std::endl;
            
            for (int j=0; j<5; j++) {
                if (color_count[j] != 0){
                    std::cout << known_colors[j] << ": " << color_count[j]  << std::endl;
                }
                else {
                    std::cout << known_colors[j] << ": 0.00%" << std::endl;
                }
            }
            std::cout << "\nSHAPE" << std::endl;
            for (int j=0; j<5; j++) {
                if (shape_count[j] != 0 ){
                    std::cout << known_shapes[j] << ": " << shape_count[j] << std::endl;
                }
                else {
                    std::cout << known_shapes[j] << ": 0.00" << std::endl;
                    
                }
            } */ /*
            for (int j=0; j<imgNames.size(); j++) {
                //std::cout<<imgNames[j]<<std::endl; //for error checking
                normSignImg(imgNames[j], maxCol, maxSha); //fcn call
            } */
        }
        else {
            std::cerr << "Unable to open file" << std::endl;
            return(-1);
        }
    
    }
    
    return 0;
}


//Determine Sign
int identifySign(string imgName){
    //**//std::cout <<  "/ID SIGN/" << std::endl;
    Mat src, img_hsv; //
    /// Load an image
    src = imread( imgName, CV_LOAD_IMAGE_COLOR);
    if(! src.data ) // Check for invalid input
    {
        //**//std::cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }
    
    /* CONVERT TO HSV */
    cvtColor( src, img_hsv, CV_BGR2HSV);
    
    
    return normColor(img_hsv);
    
}

/*/ the function creates a new cropped and normalized image of the sign feature
    each sign is resized to 50 x 50 x 50

/*/
int normSignImg(string imgName, int color, int shape){
    
    return 0;
}

/*/ the function creates a new color biased image for each known color by multiplying a converted hue and saturation image to each and normalizing the result to the max value 255.
 /*/
int normColor( const Mat& image ){
    //**//std::cout <<  "/NORM SIGN/" << std::endl;
    //normalize parameters
    double alpha=0, beta=255;
    int norm_type=NORM_MINMAX, dtype=-1;
    //InputArray mask=noArray(); no input mask; 
    
    //create image containers
    Mat satImg,imgR, imgB, imgY;
    vector<Mat> normImg;
    vector<Mat> colorImg;
    vector<Mat> imgCh;
    
    //split channels
    split(image, imgCh); //three channels: 0-2
    Mat r_blankTemp = Mat::zeros(imgCh[0].rows, imgCh[0].cols, CV_8UC1);
    Mat b_blankTemp = Mat::zeros(imgCh[0].rows, imgCh[0].cols, CV_8UC1);
    Mat y_blankTemp = Mat::zeros(imgCh[0].rows, imgCh[0].cols, CV_8UC1);
    
    
    //size images
    imgR = r_blankTemp;
    imgB = b_blankTemp;
    imgY = y_blankTemp;
    
    //norm image template
    Mat nR_blankTemp = Mat::zeros(imgCh[0].rows, imgCh[0].cols, CV_8UC1);
    Mat nB_blankTemp = Mat::zeros(imgCh[0].rows, imgCh[0].cols, CV_8UC1);
    Mat nY_blankTemp = Mat::zeros(imgCh[0].rows, imgCh[0].cols, CV_8UC1);
    Mat nBl_blankTemp = Mat::zeros(imgCh[0].rows, imgCh[0].cols, CV_8UC1);
    
    //Save four copies of the hue channel to norm for each known color
    
    for (int i = 0; i<4; i++) {
        colorImg.push_back(imgCh[0]);
        //normImg.push_back(imgCh[0]);
    } 
    
    normImg.push_back(nR_blankTemp);
    normImg.push_back(nB_blankTemp);
    normImg.push_back(nY_blankTemp);
    normImg.push_back(nBl_blankTemp);
    
    //create images for each known color except black
    hueConversion(image, colorImg, imgR, imgB, imgY);

    //**//std::cout <<  "/NORM SIGN 2/" << std::endl;
    // set images from fcn to color iamges //
    colorImg[0] = imgR;
    colorImg[1] = imgB;
    colorImg[2] = imgY;
    
    //**//std::cout <<  "/NORM SIGN 3/" << std::endl;
    // create black image (value channel reversed)
    Mat temp_255(imgCh[2].rows, imgCh[2].cols, CV_8UC1, Scalar(255));
    colorImg[3] = temp_255 - imgCh[2];
    /*colorImg[3] = imgCh[2]; //.push_back(imgCh[2]);
    int threshVal = 190;
    threshold(colorImg[3], colorImg[3], threshVal, 255, 4);
    */
    satConversion(image, satImg);
    
    //**//std::cout <<  "/NORM SIGN 4/" << std::endl;
    for (int i = 0; i<4; i++) {
        normImg[i] = satImg.mul(colorImg[i]);
        //When the norm is minmax, cv::normalize normalizes _src in such a way that the min value of dst is alpha and max value of dst is beta
        //alternatively could use dtype=CV_8UC1, for an 8 bit unsigned single channel
        normalize( normImg[i], normImg[i], alpha, beta, norm_type/*, dtype*/);
    }
    
    //show normalized images
    
    namedWindow( "IMG - Red", CV_WINDOW_NORMAL );
    imshow( "IMG - Red", normImg[0]);
    
    namedWindow( "IMG - Blue", CV_WINDOW_NORMAL );
    imshow( "IMG - Blue", normImg[1]);
    
    namedWindow( "IMG - Yellow", CV_WINDOW_NORMAL );
    imshow( "IMG - Yellow", normImg[2]);
    
    namedWindow( "IMG - Black", CV_WINDOW_NORMAL );
    imshow( "IMG - Black", normImg[3]);
    
    //namedWindow( "IMG - Saturation", CV_WINDOW_NORMAL );
    //imshow( "IMG - Saturation", satImg);
    
    //find
    
    //waitKey(0);
    /*
    destroyWindow("IMG - Red");
    destroyWindow("IMG - Blue");
    destroyWindow("IMG - Yellow");
    destroyWindow("IMG - Black"); */
    //destroyWindow("IMG - Saturation");
    
    
    int colorFound=0; //if a shape if found for this image
    int retVal = 0;
    for (int i=0; i<4; i++) {
        //**/std::cout <<  "Which Color: " << i <<  std::endl;
        retVal = findShape(normImg[i], i);
        if (retVal > 1) {
            colorFound ++;
            //color_count[i][retVal] ++;
        }
    } /*
    if (colorFound < 1) { //unknown

        color_count[4] ++;
    } */
    
    return colorFound;
}


/*/ the function creates an image for each known color from the hue channel of the original image, for black, use reverse value channel
 Known colors: red, blue, yellow, black = 1,2,3,4
 /*/
void hueConversion( const Mat& image, vector<Mat> & colorImg, Mat& colorImgR, Mat& colorImgB, Mat& colorImgY )
{
    //**//std::cout <<  "/HUE CONV/" << std::endl;
    
    //split channels
    vector<Mat> imgCh;
    vector<Mat> knownCols;
    split(image, imgCh);
    Mat blankTemp = Mat::zeros(imgCh[0].rows, imgCh[0].cols, CV_8UC1);
    
    //create knownCols vec with push back
    for (int i = 0; i<3; i++) {
        //colorImg.push_back(imgCh[0]);
        knownCols.push_back(imgCh[0]);
    }
    /*
    std::cout <<  "KNOWN" << std::endl; //
    std::cout <<  "red image val: " << (int)imgCh[0].at<uchar>(1,1) << std::endl;
    std::cout <<  "blu image val: " << (int)imgCh[0].at<uchar>(1,1) << std::endl;
    std::cout <<  "yel image val: " << (int)imgCh[0].at<uchar>(1,1) << std::endl;
    std::cout <<  "END" << std::endl;
    */
    
    //constants
    int hue_range = 360; //in opencv hue range is 180
    
    float red_LB_1 = 0 * 180 / hue_range;
    float red_UB_1 = 10 * 180 / hue_range;
    
    float yel_LB_1 = 20 * 180 / hue_range;
    float yel_UB_1 = 80 * 180 / hue_range;
    
    float blu_LB_1 = 170 * 180 / hue_range;
    float blu_UB_1 = 240 * 180 / hue_range;
    
    float red_LB_2 = 338 * 180 / hue_range;
    float red_UB_2 = 359 * 180 / hue_range;
    
    //for each pixel in img
    for(int row = 0; row < imgCh[0].rows; ++row) {
        for(int col = 0; col < imgCh[0].cols; col++) {
            int hsvPixel = imgCh[0].at<uchar>(row,col);
            
            // create red image
            if ( hsvPixel > red_LB_1 && hsvPixel < red_UB_1) { /*Red*/
                //knownCols[0].at<uchar>(row,col) = 255*((red_UB_1-hsvPixel)/red_UB_1);
                colorImgR.at<uchar>(row,col) = 255*((red_UB_1-hsvPixel)/red_UB_1);
            } else if (hsvPixel > red_LB_2 && hsvPixel < red_UB_2) {
                //knownCols[0].at<uchar>(row,col) = 255*((hsvPixel-red_UB_2)/red_UB_2);
                colorImgR.at<uchar>(row,col) = 255*((hsvPixel-red_UB_2)/red_UB_2);
            } else {
                //knownCols[0].at<uchar>(row,col) = 0;
                colorImgR.at<uchar>(row,col) = 0;
            }
            
            // create blue image
            if (hsvPixel > blu_LB_1 && hsvPixel < (blu_LB_1+(blu_UB_1-blu_LB_1)/blu_LB_1) ) { /*Blue*/
                //knownCols[1].at<uchar>(row,col) = 255*((hsvPixel-blu_LB_1)/blu_LB_1);
                colorImgB.at<uchar>(row,col) = 255*((hsvPixel-blu_LB_1)/blu_LB_1);
            } else if (hsvPixel > (blu_LB_1+(blu_UB_1-blu_LB_1)/blu_LB_1) && hsvPixel < blu_UB_1) {
                //knownCols[1].at<uchar>(row,col) = 255*((hsvPixel-blu_UB_1)/blu_UB_1);
                colorImgB.at<uchar>(row,col) = 255*((hsvPixel-blu_UB_1)/blu_UB_1);
            } else {
                //knownCols[1].at<uchar>(row,col) = 0;
                colorImgB.at<uchar>(row,col) = 0;
            }
            
            // create yellow image
            if (hsvPixel > yel_LB_1 && hsvPixel < (yel_LB_1+(yel_UB_1-yel_LB_1)/yel_LB_1) ) { /*Yellow*/
                //knownCols[2].at<uchar>(row,col) = 255*((hsvPixel-yel_LB_1)/yel_LB_1);
                colorImgY.at<uchar>(row,col) = 255*((hsvPixel-yel_LB_1)/yel_LB_1);
            } else if (hsvPixel > (yel_LB_1+(yel_UB_1-yel_LB_1)/yel_LB_1) && hsvPixel < yel_UB_1) {
                //knownCols[2].at<uchar>(row,col) = 255*((hsvPixel-yel_UB_1)/yel_UB_1);
                colorImgY.at<uchar>(row,col) = 255*((hsvPixel-yel_UB_1)/yel_UB_1);
            } else {
                //knownCols[2].at<uchar>(row,col) = 0;
                colorImgY.at<uchar>(row,col) = 0;
            }
            /*
            if (row < 3 && col < 3){
                std::cout <<  "hue value: " << (int)hsvPixel << std::endl;
                std::cout <<  "red image val: " << (int)colorImgR.at<uchar>(row,col) << std::endl;
                std::cout <<  "blu image val: " << (int)colorImgB.at<uchar>(row,col) << std::endl;
                std::cout <<  "yel image val: " << (int)colorImgY.at<uchar>(row,col) << std::endl;
            } */
            
        } // end col
    } //end row loop
    
    //**//std::cout <<  "/HUE CONV 2/" << std::endl;
    //set colorImg to knownCols
    /* NOT NEEDED
    colorImg[0] = colorImgR;
    colorImg[1] = colorImgB;
    colorImg[2] = colorImgY;

    
    for (int i = 0; i<3; i++) {
        colorImg[i] = knownCols[i]; //.push_back(knownCols[i]);
    } */
    
    //show the images for
    /*
    namedWindow( "IMG - Red", CV_WINDOW_NORMAL );
    imshow( "IMG - Red", colorImgR);
    namedWindow( "IMG - Blue", CV_WINDOW_NORMAL );
    imshow( "IMG - Blue", colorImgB);
    namedWindow( "IMG - Yellow", CV_WINDOW_NORMAL );
    imshow( "IMG - Yellow", colorImgY);
    */
    return;
}

/*/ the function creates a new saturation image from the saturation channel of the original image
 /*/
void satConversion( const Mat& image, Mat& satImg ) {

    //**//std::cout <<  "/SAT CONV/" << std::endl;
    int sMin = 130;
    
    //split channels
    vector<Mat> imgCh;
    split(image, imgCh);
    
    imgCh[1].copyTo(satImg);
    
    //for each pixel in img
    for(int row = 0; row < imgCh[1].rows; ++row) {
        for(int col = 0; col < imgCh[1].cols; col++) {
            int satPixel = imgCh[1].at<uchar>(row,col);
            
            // convert saturation of image
            if (satPixel < sMin) {
                satImg.at<uchar>(row,col) = satPixel;
            } else {
                satImg.at<uchar>(row,col) = 255;
            }
            
        } // end col loop
    } // end row loop
    
    return;
}

int findShape(const Mat& image, int color){
    
    //**//std::cout << "Shape Fcn" << std::endl;
    /****DETECT SHAPE****/
    int num_SQ=0, numOCT=0, numTRI=0, numCir=0;
    
    vector<vector<Point> > squares;
    vector<Vec3f> circles;
    
    Mat img_sqr, img_oct, img_tri, img_cir, img_shape;
    image.copyTo(img_sqr);
    image.copyTo(img_oct);
    image.copyTo(img_tri);
    image.copyTo(img_cir);
    
    //**//std::cout << "Shape Fcn 1" << std::endl;
    num_SQ = findPoly(img_sqr, squares, 4, 1/*.3*/, 1);
    //**//std::cout << "Shape Fcn 2" << std::endl;
    numOCT = findPoly(img_oct, squares, 8, 1, 0);
    //**//std::cout << "Shape Fcn 3" << std::endl;
    numTRI = findPoly(img_tri, squares, 3, 1, 0);
    //**//std::cout << "Shape Fcn 4" << std::endl;
    numCir = findCircles(img_cir, circles);
    
    if (num_SQ > 0) {
        //detected squares
        //**/std::cout << "Shape: SQR" << std::endl;
        shape_count[0] ++;
        color_count[color][0] ++;
        //drawSquares(img_sqr, squares, 4);
    } else {
        
    }
    if (numOCT > 0) {
        //detect octagons
        //**/std::cout << "Shape: OCT" << std::endl;
        shape_count[1] ++;
        color_count[color][1] ++;
        //drawSquares(img_oct, squares, 8);
    } else {
        
    }
    if (numTRI) {
        //detect triangles
        //**/std::cout << "Shape: TRI" << std::endl;
        shape_count[2] ++;
        color_count[color][2] ++;
        //drawSquares(img_tri, squares, 3);
    } else {
        
    }
    if (numCir) {
        //detect circles
        //**/std::cout << "Shape: CIR" << std::endl;
        shape_count[3] ++;
        color_count[color][3] ++;
        //drawCircles(img_cir, circles);
    } else {
        
    }
    if (num_SQ ==0 && numOCT ==0 && numTRI ==0 && numCir==0) {
        //detect no shapes
        //**/std::cout << "Shape: UNK" << std::endl;
        shape_count[4] ++;
        color_count[color][4] ++;
        return -1;
    }
    
    return 1;
}

double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int findPoly(const Mat image, vector<vector<Point> >& squares, int numPts, float ang, int check)
{
    squares.clear();
    Mat gray;
    image.copyTo(gray);
    vector<vector<Point> > contours;
    
    //ƒwhichstd::cout << "Poly Find 1" << std::endl;
    // find contours and store them all as a list
    findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    //std::cout << "Poly Find 2" << std::endl;
    vector<Point> approx;
    //std::cout << "Poly Find 2.3, size: " << contours.size() << std::endl;
    
    if (contours.size() > 0){
        vector<Rect> boundRect( contours.size() );
        // test each contour
        for( size_t i = 0; i < contours.size(); i++ )
        {   // approximate contour with accuracy proportional
            // to the contour perimeter
            //std::cout << "Poly Find 2.5" << std::endl;
            approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
            boundRect[i] = boundingRect( Mat(approx) );
            
            int shapeArea = boundRect[i].width * boundRect[i].height;
            
            //std::cout <<  "shape size: " << shapeArea << std::endl;
            
            // square contours should have 4 vertices after approximation
            // relatively large area (to filter out noisy contours)
            // and be convex.
            // Note: absolute value of an area is used because
            // area may be positive or negative - in accordance with the
            // contour orientation
            //std::cout << "Poly Find 3" << std::endl;
            if( approx.size() == numPts /* &&
               fabs(contourArea(Mat(approx))) > 1000 */ && 
               isContourConvex(Mat(approx)) && shapeArea < (float)image.rows*image.cols*.75 && shapeArea > (float)image.rows*image.cols*.2) //shape area between 20 and 75 percent of image area
            {
                double maxCosine = 0;
                for( int j = 2; j < numPts+1; j++ )
                {  // find the maximum cosine of the angle between joint edges
                    double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                    maxCosine = MAX(maxCosine, cosine);
                }
                // if cosines of all angles are small
                // (all angles are ~90 degree) then write quandrange
                // vertices to resultant sequence
                if (check == 1) {
                    if( maxCosine < ang ) // .3 for sq
                        squares.push_back(approx);
                } else {
                    squares.push_back(approx);
                }
            }   //else {std::cout << "Number of points: " << approx.size() << std::endl;}
        }
        
        
        //**//std::cout <<  "img size: " << (float)image.rows*image.cols*.75 << std::endl;
    }
    
    return squares.size();
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
int findSquares( const Mat image, vector<vector<Point> >& squares, int numPts, float ang, int check )
{
    
    int thresh = 50, N = 11;
    squares.clear();
    
    Mat pyr, timg, gray0(image.size(), CV_8U), gray;
    
    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;
    
        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {   // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            } else
            {// apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }
            // find contours and store them all as a list
            findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
            vector<Point> approx;
            
            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {   // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
                
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == numPts &&
                   fabs(contourArea(Mat(approx))) > 1000 &&
                   isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;
                    for( int j = 2; j < numPts+1; j++ )
                    {  // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }
                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if (check == 1) {
                        if( maxCosine < ang ) // .3 for sq
                            squares.push_back(approx);
                    } else {
                        squares.push_back(approx);
                    }
                }   //else {std::cout << "Number of points: " << approx.size() << std::endl;}
            }
        }
    
    return squares.size();
}
// the function draws all the squares in the image
void drawSquares( Mat image, const vector<vector<Point> >& squares, int numPts )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, CV_AA);
    }
    if (numPts == 3) {
        namedWindow( triWnd, CV_WINDOW_NORMAL );
        imshow(triWnd, image);
        waitKey(0);
        destroyWindow(triWnd);
    } else if (numPts == 4) {
        namedWindow( sqrWnd, CV_WINDOW_NORMAL );
        imshow(sqrWnd, image);
        waitKey(0);
        destroyWindow(sqrWnd);
    } else if (numPts == 8) {
        namedWindow( octWnd, CV_WINDOW_NORMAL );
        imshow(octWnd, image);
        waitKey(0);
        destroyWindow(octWnd);
    }
    
    return;
}
int findCircles(const Mat image, vector<Vec3f>& circles ){

    circles.clear();
    /// Convert it to gray
    Mat img_gray;
    image.copyTo(img_gray);
    //cvtColor( image, img_gray, CV_BGR2GRAY );
    
    /// Reduce the noise so we avoid false circle detection
    GaussianBlur( img_gray, img_gray, Size(9, 9), 2, 2 );
    HoughCircles( img_gray, circles, CV_HOUGH_GRADIENT, 1, img_gray.rows/2, 200, 30, img_gray.rows/4, img_gray.rows/2);
    //HoughCircles( img_gray, circles, CV_HOUGH_GRADIENT, 1, img_gray.rows/2, 200, 30, 1, 200);
    return circles.size();
}
void drawCircles(Mat image, const vector<Vec3f>& circles ){
    
    /// Draw the circles detected
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        circle( image, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( image, center, radius, Scalar(0,0,255), 3, 8, 0 );
    }
    /// Show your results
    namedWindow( cirWnd, CV_WINDOW_NORMAL );
    imshow( cirWnd, image );
    waitKey(0);
    destroyWindow(octWnd);
    return;
}

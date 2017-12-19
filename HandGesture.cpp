#include "HandGesture.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

HandGesture::HandGesture() {

}


double HandGesture::getAngle(Point s, Point e, Point f) {

	double v1[2],v2[2];
	v1[0] = s.x - f.x;
	v1[1] = s.y - f.y;

	v2[0] = e.x - f.x;
	v2[1] = e.y - f.y;

	double ang1 = atan2(v1[1],v1[0]);
	double ang2 = atan2(v2[1],v2[0]);

	double angle = ang1 - ang2;
	if (angle > CV_PI) angle -= 2 * CV_PI;
	if (angle < -CV_PI) angle += 2 * CV_PI;
	return (angle * 180.0/CV_PI);
}
void HandGesture::FeaturesDetection(Mat mask, Mat output_img) {

	vector<vector<Point> > contours;
	Mat temp_mask;
	mask.copyTo(temp_mask);
	int index = -1;

  // CODIGO 3.1
  // detección del contorno de la mano y selección del contorno más largo
  //...

	findContours(temp_mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	vector<Point> maslargo = contours[0];
	index = 0;
	for (int i = 1; i < contours.size(); i++)
	{
		if (contours[i].size() > maslargo.size())
		{
			maslargo = contours[i];
			index = i;
		}
	}

	Rect rect = boundingRect(contours[index]);
	Point pt1, pt2;
	pt1.x = rect.x;
	pt1.y = rect.y;
	pt2.x = rect.x + rect.width;
	pt2.y = rect.y + rect.height;

	rectangle(output_img, pt1, pt2, CV_RGB(255,255,0),1);

  // pintar el contorno
  //...

	drawContours(output_img, contours, index, cv::Scalar(255,0,0), 2, 8,
		vector<Vec4i>(), 0, Point());

	//obtener el convex hull
	vector<int> hull;
	convexHull(contours[index],hull);

	// pintar el convex hull
	Point pt0 = contours[index][hull[hull.size()-1]];
	for (int i = 0; i < hull.size(); i++)
	{
		Point pt = contours[index][hull[i]];
		line(output_img, pt0, pt, Scalar(0, 0, 255), 2, CV_AA);
		pt0 = pt;
	}

  //obtener los defectos de convexidad
	vector<Vec4i> defects;
	convexityDefects(contours[index], hull, defects);


	int cont = 0;
	vector<Point> puntos;
	Point s,e,f;

	for (int i = 0; i < defects.size(); i++) {
		s = contours[index][defects[i][0]];
		e = contours[index][defects[i][1]];
		f = contours[index][defects[i][2]];
		float depth = (float)defects[i][3] / 256.0;
		double angle = getAngle(s, e, f);

    // CODIGO 3.2
    // filtrar y mostrar los defectos de convexidad
		if (angle <= 90){
    		circle(output_img,f,5,Scalar(0,255,0),3);
		cont++;
		}

		/*if(depth > 40 && depth < 200)
    {
    	line( output_img, s, f, CV_RGB(0,255,0), 2 );
      	line( output_img, e, f, CV_RGB(0,255,0), 2 );
      	circle( output_img, s,   4, Scalar(100,0,255), 2 );
    }*/
  }

	// Cuenta dedos
	int dedos;
	stringstream ss;
	if (cont > 0)
		dedos = cont+1;
	else
		dedos = 0;
	ss << dedos;
	string num_dedos = ss.str();
	putText(output_img, num_dedos, Point2f(100,100), FONT_HERSHEY_PLAIN, 2,  cvScalar(0,0,0), 2);

	// dibujar
	switch (dedos) {
		case 0: exit(0);break;
		case 1: break;
		case 2: break;
		case 3: break;
		
		case 4: break;
		case 5: break;
	}	
}

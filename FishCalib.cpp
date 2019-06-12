#include "FishCalib.h"
#include "StlGlDefines.h"

inline void sphere_tp_erect( double x_dest,double  y_dest,
	double* x_src, double* y_src,
	int img_width, int img_height,
	double dbfov,
	double x_dbscale,
	double y_dbscale,
	double x_offset,
	double y_offset)
{
	double distance = (double)img_width/dbfov;
	register double phi, theta, r,s;
	double v[3];

	x_dbscale = 1/x_dbscale;
	y_dbscale = 1/y_dbscale;
	x_dest = (double) (x_dest-img_width/2)*x_dbscale;
	y_dest = (double) (y_dest-img_height/2)*y_dbscale;

	phi 	= x_dest / distance;
	theta 	=  - y_dest / distance  + PI / 2;
	if(theta < 0)
	{
		theta = - theta;
		phi += PI;
	}
	if(theta > PI)
	{
		theta = PI - (theta - PI);
		phi += PI;
	}
	s = sin( theta );
	v[0] =  s * sin( phi );	//  y' -> x
	v[1] =  cos( theta );				//  z' -> y
	r = sqrt( v[1]*v[1] + v[0]*v[0]);
	theta = distance * atan2( r , s * cos( phi ) );

	*x_src =  theta * v[0] / r + img_width/2 + 0.5 + x_offset;
	*y_src =  theta * v[1] / r + img_height/2 + 0.5 + y_offset;
}

void get_img(Mat &inIMG, Mat &outIMG)
{
	static cv::Mat mapX(outIMG.rows, outIMG.cols, CV_32F);
	static cv::Mat mapY(outIMG.rows, outIMG.cols, CV_32F);
	static bool once=false;

	if(!once)
	{
		Size imageSize = inIMG.size();
		for(int row = 0; row < outIMG.rows; row++)
		{
			for(int col = 0; col < outIMG.cols; col++)
			{
				double x_dist = (double) (col);
				double y_dist = (double) (row);
				double x_src, y_src;

				sphere_tp_erect(x_dist, y_dist, &x_src, &y_src, outIMG.cols, outIMG.rows, PI*140/180, 1.2, 1.05, 0.0, 0.0);

				int xsrc = (floor)(x_src);
				int ysrc = (floor)(y_src);
				if(xsrc >=0 && xsrc < inIMG.cols && ysrc >=0 && ysrc < inIMG.rows){
					mapX.at<float>(row, col) = (float)(x_src);
					mapY.at<float>(row, col) =(float)( y_src);
				}else{
					mapX.at<float>(row, col) = (float)0.0;
					mapY.at<float>(row, col) = (float)0.0;
				}
			}
		}
		once = true;
	}
	remap(inIMG, outIMG, mapX, mapY, cv::INTER_LINEAR);
}

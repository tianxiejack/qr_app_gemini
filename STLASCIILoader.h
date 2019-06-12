/*
 * STLASCIILoader.h
 *
 *  Created on: Nov 1, 2016
 *      Author: hoover
 */

#ifndef STLASCIILOADER_H_
#define STLASCIILOADER_H_
#include <stdio.h>
#include <opencv2/core/core.hpp>
using namespace std;
using namespace cv;

class STLASCIILoader{
public :
	STLASCIILoader();
	~STLASCIILoader();
	void ParseSTLAscii(const char *filename);
	int GetMemSize(){return mem_size;};
	inline float GetZ_Depth(){return Z_Depth;};
	inline float GetBig_Extent(){return Big_Extent;};
	inline float Getextent_neg_x(){return extent_neg_x;};
	inline float Getextent_neg_y(){return extent_neg_y;};
	inline float Getextent_neg_z(){return extent_neg_z;};
	inline float Getextent_pos_x(){return extent_pos_x;};
	inline float Getextent_pos_y(){return extent_pos_y;};
	inline float Getextent_pos_z(){return extent_pos_z;};
	inline int Getpoly_count(){return poly_count;};
	inline float GetScan_pos(){return scan_pos;};
	void SetScan_pos(float now_scan_distance){scan_pos=now_scan_distance;};
//	inline float *Getpoly_list(){return poly_list;};
	 void cpyl1l2();
	inline vector<cv::Point3f> *Getpoly_vector(){return &list;};
	inline vector<cv::Point3f> *Getpoly_vector2(){return &list2;};
	void ResetPolyList();
	void PrintExtents();
private:
	void CollectPolygons(FILE *filein);
	void setScale(float scale);
	void FindExtents();
	void TransformToOrigin();
	int getMaxExtents();
	inline void ResetExtents(){
		extent_pos_x = extent_pos_y = extent_pos_z = -100000.0f;
		extent_neg_x = extent_neg_y = extent_neg_z = 100000.0f;
	};
	vector<cv::Point3f> list;
	vector<cv::Point3f> list2;
	int poly_count;
	float scan_pos;
	float extent_pos_x, extent_pos_y , extent_pos_z ;
	float extent_neg_x , extent_neg_y , extent_neg_z ;
	float Z_Depth , Big_Extent;
	int mem_size;
};

#endif /* STLASCIILOADER_H_ */

/*
 * STLASCIILoader.cpp
 *
 *  Created on: Nov 1, 2016
 *      Author: hoover
 */

#include "STLASCIILoader.h"
#include <stdlib.h>
#include <string.h>    /* for strcpy and relatives */
#include <math.h>
#include "StlGlDefines.h"
#include "GLRender.h"
#include <iostream>
//#include "common.h"
#include "main.h"
//using namespace std;

/* Declarations ------------------------------------- */



STLASCIILoader::STLASCIILoader():
		poly_count(0),
		Z_Depth(-5),Big_Extent(10),mem_size(0)
{
	ResetExtents();
}

STLASCIILoader::~STLASCIILoader()
{
	if(!list.empty()){
		list.clear();
	}
}
/* This function puts all the polygons it finds into the global array poly_list */
/* it uses the global variable poly_count to index this array poly_count is used */
/* elsewhere so it needs to be left alone once this finishes */
#define XX (t1)
#define YY t2
#define ZZ (t3)
void STLASCIILoader::CollectPolygons(FILE *filein)
{
	 char oneline[255];
	 char arg1[16],arg2[16];
	 static const char *poly_end = "endfacet";
	 static const char *poly_normal = "facet";
	 static const char *poly_vertex = "vertex";
	 float t1, t2, t3;
	 poly_count = 0;
	 while ( !feof(filein) )
	 {
		  memset(oneline,0,sizeof(oneline));
		  memset(arg1,0,sizeof(arg1));
		  fgets(oneline, 255, filein);
		  sscanf(oneline, "%s", arg1);
		  if (strcasecmp(poly_normal, arg1) == 0)  /* Is this a normal ?? */
		  {
			  sscanf(oneline, "%s %s %f %f %f", arg1, arg2, &t1, &t2, &t3);
			  list.push_back(cv::Point3f(XX,YY,ZZ));
		  }

		  if (strcasecmp(poly_vertex, arg1) == 0)  /* Is it a vertex ?  */
		  {
			   sscanf(oneline, "%s %f %f %f", arg1, &t1, &t2, &t3);
			   list.push_back(cv::Point3f(XX,YY,ZZ));

			   fgets(oneline, 255, filein);  /* Next two lines vertices also!  */
			   sscanf(oneline, "%s %f %f %f", arg1, &t1, &t2, &t3);
			   list.push_back(cv::Point3f(XX,YY,ZZ));

			   fgets(oneline, 255, filein);
			   sscanf(oneline, "%s %f %f %f", arg1, &t1, &t2, &t3);
			   list.push_back(cv::Point3f(XX,YY,ZZ));
		   }
	}

	if (common.isVerbose())
		printf("\n");
}


/* This function reads through the array of polygons (poly_list) to find the */
/* largest and smallest vertices in the model.  This data will be used by the */
/* transform_model function to center the part at the origin for rotation */
/* and easy autoscale */
void STLASCIILoader::FindExtents()
{
	ResetExtents();
    for(int i=0; i<list.size(); i++)
    {
    	if(i%4 == 0)
    		continue;

    	if(list[i].x > extent_pos_x)
    		extent_pos_x = list[i].x;
    	if(list[i].y > extent_pos_y)
    	    extent_pos_y = list[i].y;
    	if(list[i].z > extent_pos_z)
    	    extent_pos_z = list[i].z;

    	if(list[i].x < extent_neg_x)
    	    extent_neg_x = list[i].x;
    	if(list[i].y < extent_neg_y)
    	    extent_neg_y = list[i].y;
    	if(list[i].z < extent_neg_z)
    	    extent_neg_z = list[i].z;
    }
/* Find geometry extents */
}


/* This translates the center of the rectangular bounding box surrounding */
/* the model to the origin.  Makes for good rotation.  Also it does a quick */
/* Z depth calculation that will be used bring the model into view (mostly) */

void STLASCIILoader::TransformToOrigin()
{
	float LongerSide, ViewAngle;

	/* first transform into positive quadrant */

	for(int i=0; i<list.size(); i++)
	{
		if(i%4 == 0)
			continue;
		list[i].x -= extent_neg_x;
		list[i].y -= extent_neg_y;
		list[i].z -= extent_neg_z;
	}
	FindExtents();
	/* Do quick Z_Depth calculation while part resides in ++ quadrant */
	/* Convert Field of view to radians */
	ViewAngle = ((FOV / 2) * (PI / 180));

	LongerSide = MAX(extent_pos_x, extent_pos_y);

	/* Put the result where the main drawing function can see it */
	Z_Depth = ((LongerSide / 2) / tanf(ViewAngle));
	Z_Depth = Z_Depth * -1;

	/* Do another calculation for clip planes */
	/* Take biggest part dimension and use it to size the planes */

	Big_Extent = getMaxExtents();
	/* Then calculate center and put it back to origin */

	for(int i=0; i<list.size(); i++)
	{
		if(i%4 == 0)
			continue;
		list[i].x -= extent_pos_x/2;
		list[i].y -= extent_pos_y/2;
	}
}
int STLASCIILoader::getMaxExtents()
{
	return MAX(extent_pos_z,MAX(extent_pos_x,extent_pos_y));
}
void STLASCIILoader::setScale(float scale)
{
	for(int i=0; i<list.size(); i++)
	{
		if(i%4 == 0)
			continue;
		list[i].x *= scale;
		list[i].y *= scale;
		list[i].z *= scale;
	}
}
void STLASCIILoader::ParseSTLAscii(const char *filename)
{
	/* Begin parsing command args.  Lame, but it works :)  */
	printf("  StlTextures: \n");
//	filein; // File handle for the STL file to be viewed
	FILE *filein = fopen(filename, "r");
	if (filein == NULL)
	{
		  printf("This is how you invoke the viewer... \n");
		  printf("           Usage:  StlTexturex [Scenefile] [VehicleFile] [PanelFile] (-o or -p) (-f or -v)\n");
		  printf("           Valid Options are: -o (Ortho View EXPEREMENTAL)\n");
		  printf("                              -p (Perspective View)\n");
		  printf("                              -f (Redraw only on view change)\n");
		  printf("                              -v (Report debug info to STDOUT)\n");
		  exit(1);
	}

	/* Read through the file to get number of polygons so that we can malloc */
	/* The right amount of ram plus a little :)  */

	/* Ask our friendly OS to give us a place to put the polygons for a while */
	/* This does not work on win32.  Seems it does not know how to deal with */
	/* the sizeof thing...  Have to just plug in a value (4) damn...  */

	/* reset the poly counter so that it is also an index for the data */

	CollectPolygons(filein);
//	setScale(1.0*4);
	poly_count = list.size()/4;
	mem_size = poly_count*sizeof(cv::Point3f);
	if (common.isVerbose())
	{
		 printf("           %i bytes allocated!\n", mem_size);
		 printf("           Reading");
	}

	FindExtents();
	printf("   %s:%d        Part extents are: x, y, z\n",filename,poly_count);
	printf("           %f, %f, %f\n", extent_pos_x, extent_pos_y, extent_pos_z);
	printf("           %f, %f, %f\n", extent_neg_x, extent_neg_y, extent_neg_z);

	/* Print the result of the extent calc */
	if (common.isVerbose())
	{
		printf("           Part extents are: x, y, z\n");
		printf("           %f, %f, %f\n", extent_pos_x, extent_pos_y, extent_pos_z);
		printf("           %f, %f, %f\n", extent_neg_x, extent_neg_y, extent_neg_z);
	}

	TransformToOrigin();

	fclose(filein);

	if (common.isVerbose())
		printf("           File Processed\n");

}

void STLASCIILoader::ResetPolyList()
{
	if(!list.empty()){
		list.clear();
	}
}

void STLASCIILoader::PrintExtents()
{
	cout<<"STL neg_x, y, z = "<<extent_neg_x<<", "<<extent_neg_y<<", "<<extent_neg_z<<endl;
	cout<<"STL pos_x, y, z = "<<extent_pos_x<<", "<<extent_pos_y<<", "<<extent_pos_z<<endl;
}



void STLASCIILoader::cpyl1l2(){
//	vector<cv::Point3f>::iterator iter;
	list2.clear();
	list2.assign(list.begin(),list.end());
//	int a=0,b=0;
//	for(iter=list.begin();iter!=list.end();iter++)
	{
//		a++;
		//cout<<*iter<<endl;
	}
//	for(iter=list2.begin();iter!=list2.end();iter++)
	{
	//	b++;
	//	cout<<*iter<<endl;
	}
//	printf("numa=%d\n,b=%d\n",a,b);
//	sleep(5);
};


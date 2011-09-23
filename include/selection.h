#ifndef SELECTION_H
#define SELECTION_H

#include "CImg.h"
#include <iostream>

using namespace cimg_library;
using namespace std; 

class Selection {

	public:
		int getxsize();
		int getysize();
		int getx1();
		int getx2();
		int gety1();
		int gety2();
		float getm();
		float getc();

		void selectline(char* filename);
		bool selmade;
		
	private:
		int x1,x2,y1,y2;
		int xsize, ysize; 
		float m,c;	
			
};

#endif

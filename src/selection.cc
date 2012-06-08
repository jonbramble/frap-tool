/*
 * selection.cc
 * Copyright (C) Jonathan Bramble 2011
 * 
frap-tool is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * frap-tool is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../include/selection.h"

namespace FrapTool {

int Selection::getxsize(){
		return xsize;
	}

int Selection::getysize(){
		return ysize;
	}

int Selection::getx1(){
		return x1;
	}

int Selection::getx2(){
		return x2;
	}

int Selection::gety1(){
		return y1;
	}

int Selection::gety2(){
		return y2;
	}

float Selection::getm(){
		return m;
	}

float Selection::getc(){
		return c;
	}

void Selection::selectline(std::string _filename){
	int currentx, currenty;
	bool setone,settwo;
	const unsigned char color[] = { 0,0,0 };

	char *cstr;
	cstr = new char [_filename.size()+1];
    	strcpy (cstr, _filename.c_str());

	cimg_library::CImg<float> image(cstr), fresh(cstr);
	cimg_library::CImgDisplay main_display(image,cstr);

	x1=0;
	setone = false;
	settwo = false;
	
	while (!main_display.is_closed()) {
		main_display.wait();

		if(main_display.button()>=0){
			currentx = main_display.mouse_x(); //get x and y points
			currenty = main_display.mouse_y();

			if(x1!=0){
				image.draw_line(x1,y1,currentx,currenty,color).display(main_display);	
				image = fresh; //reload image with a clean one
			}
			
			if(main_display.button()==1){
				setone = false;
				x1 = currentx;  // left click to set starting point
				y1 = currenty;
				image = fresh; 		
				main_display.set_button(0);  // breaks out of this sequence if mouse is held down	
				setone = true;	// know that the point has been set
			}			

			if(main_display.button()==2){
				settwo = false;
				x2 = main_display.mouse_x();
				y2 = main_display.mouse_y();
				settwo = true;
				main_display.close(); //right click to close
				}
			}
		}

	if(setone && settwo){
		selmade = true;
		xsize = x2-x1;
		ysize = y2-y1;	
	}
	else
	{	
		std::cout << "boundaries not set correctly" << std::endl;

	}
}


} // FrapTool

/*void Selection::selectline(char* filename){
	int currentx, currenty;
	bool setone,settwo;
	const unsigned char color[] = { 0,0,0 };
	const unsigned char white[] = { 255,255,255 };
	std::string text;

	CImg<float> image(filename), fresh(filename);
	image.normalize(0,255);
	fresh.normalize(0,255);
	CImgDisplay main_display(image,filename);

	x1=0;
	setone = false;
	settwo = false;
	
	while (!main_display.is_closed()) {
		main_display.wait();

		if(main_display.button()>=0){
			currentx = main_display.mouse_x(); //get x and y points
			currenty = main_display.mouse_y();

			if(x1!=0){
				image.draw_line(x1,y1,currentx,currenty,white);
				//bounding lines
				image.draw_line(x1,y1,x1,currenty,white);
				image.draw_line(x1,y1,x1,currenty,white);
				image.draw_line(x1,y1,currentx,y1,white);
				image.draw_line(currentx,y1,currentx,currenty,white);
				image.draw_line(x1,currenty,currentx,currenty,white);
				xsize = x2-x1;
				ysize = y2-y1;
				m = (float)ysize/xsize;			
		
				image.draw_text(20,20,white,color,1.0,"Ratio %f",m).display(main_display);
				
				image = fresh; //reload image with a clean one
			}
			
			if(main_display.button()==1){
				setone = false;
				x1 = currentx;  // left click to set starting point
				y1 = currenty;
				image = fresh; 		
				main_display.set_button(0);  // breaks out of this sequence if mouse is held down	
				setone = true;	// know that the point has been set
			}			

			if(main_display.button()==2){
				settwo = false;
				x2 = main_display.mouse_x();
				y2 = main_display.mouse_y();
				settwo = true;
				main_display.close(); //right click to close
				}
			}
		}

	if(setone && settwo){
		selmade = true;
		xsize = x2-x1;
		ysize = y2-y1;
		m = (float)ysize/xsize;
		c = (float)y2-m*(float)x2;	
	}
	else
	{	
		cout << "boundaries not set correctly" << endl;

	}

}*/

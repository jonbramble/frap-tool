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

#include "selection.h"

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

void Selection::selectline(std::string _prima, std::string _closed, std::string _first){
	int currentx, currenty, k, a;
	bool setone,settwo;
	const int npoints = 450;

	const unsigned char black[] = { 0,0,0 };
	const unsigned char red[] = { 255,0,0 };

	x1=0;
	a=0;
	setone = false;
	settwo = false;

	char *closed, *first, *prima;
	closed = new char [_closed.size()+1];
	first = new char [_first.size()+1];
	prima = new char [_prima.size()+1];
    	strcpy (closed, _closed.c_str());
	strcpy (first, _first.c_str());
	strcpy (prima, _prima.c_str());

	cimg_library::CImg<float> image(closed), fresh(closed), visu(npoints,320,1,3,0);
	cimg_library::CImgDisplay main_display(image,closed), draw_display(visu,"Intensity Profile");
	cimg_library::CImg<float> graph_values(npoints,1,1,1);
	cimg_library::CImg<float> first_image(first);
	cimg_library::CImg<float> prima_image(prima);
	cimg_library::CImg<float> baseline_image; 

	baseline_image = prima_image-first_image;		//this is the 'wrong' way to get +ve image
	baseline_image.blur(2.5);

	float max_val = baseline_image.max();
	float min_val = baseline_image.min();
	float pix_val,xk,yk, xstep, ystep, len, x_size, y_size;

	delete [] closed;
	delete [] first;
	delete [] prima;

	while (!main_display.is_closed()) {
		main_display.wait();

		if(main_display.button()>=0){
			currentx = main_display.mouse_x(); //get x and y points
			currenty = main_display.mouse_y();

			if(x1!=0){
				std::cout << a ; 
				image.draw_line(x1,y1,currentx,currenty,black).display(main_display);	
				 
				x_size = currentx-x1;
				y_size = currenty-y1;

				len = hypot(xsize,ysize);
				if(len == 0.0) {len = 1.0;} 
				xstep = x_size/len; // should be length of line?
				ystep = y_size/len;

				for(k=0;k<npoints;k++){
					xk=x1+k*xstep; // calc x and y values
                			yk=y1+k*ystep;
					pix_val = baseline_image.cubic_atXY(xk,yk);  // out of range here
					graph_values.set_linear_atXY(pix_val,k);
				}

				//max_val = graph_values.max();
				//min_val = graph_values.min();
				visu.fill(0).draw_graph(graph_values,red,1,1,0,min_val,max_val).display(draw_display);
				std::cout << "..end" << std::endl;
				a++;
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


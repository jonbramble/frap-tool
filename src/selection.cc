#include "../include/selection.h"

int Selection::getxsize(){
		return abs(xsize);
	}

int Selection::getysize(){
		return abs(ysize);
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

void Selection::selectline(char* filename){
	int currentx, currenty;
	bool setone,settwo;
	const unsigned char color[] = { 0,0,0 };

	CImg<float> image(filename), fresh(filename);
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
		m = (float)ysize/xsize;
		c = (float)y2-m*(float)x2;	
	}
	else
	{	
		cout << "boundaries not set correctly" << endl;

	}
}

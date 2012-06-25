/*
 * selection.h
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

#ifndef SELECTION_H
#define SELECTION_H

#include "CImg.h"
#include <string>
#include <iostream>

namespace FrapTool {

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

		void selectline(std::string, std::string);
		bool selmade;
		
	private:
		int x1,x2,y1,y2;
		int xsize, ysize; 
		float m,c;	
			
};

}

#endif

/*
 * tiffile.h
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


#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <tiffio.h>

class Tiffile {
	
	public:
	Tiffile(char* filename);
        int getimagewidth();  //can add more if needed
        int getimageheight();
	char* getfilename();
	tm gettm();
	double gettime(){return seconds;};
	
	bool operator<(Tiffile rhs) const { return seconds < rhs.seconds; }
	
	private:
	char* imagefilename;
        int imagewidth;
        int imageheight;
	struct tm imagetime;
	int ms;	
	double seconds;
};

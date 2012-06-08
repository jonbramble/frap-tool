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
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <tiffio.h>

namespace FrapTool {

class Tiffile {
	
	public:
	Tiffile(std::string _filename);
        int getimagewidth();  //can add more if needed
        int getimageheight();
	std::string getfilename();
	tm gettm();
	double gettime(){return d_seconds;};
	long int get_long_time(){return l_seconds;};

	protected:
	long int l_seconds;
	double d_seconds;
	
	private:
	std::string filename;
        int imagewidth;
        int imageheight;
	struct tm imagetime;
	int ms;	
};

}

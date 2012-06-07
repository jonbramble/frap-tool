/*
 * frapimage.h
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

#ifndef FRAPIMAGE_H
#define FRAPIMAGE_H

#include <string>
#include "tiffile.h"

namespace FrapTool {

class Frapimage {
	public:
	 Frapimage(std::string _filename);
	 ~Frapimage();

	double A, lambda, lambda_2, mu, b, time_s, A_err, lambda_err, lambda_err_2, mu_err, b_err;

	private:
  	 std::string filename;
	 Tiffile *tiffile;
	 char *cstr;
};

} // FrapTool

#endif

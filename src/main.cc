/*
 * main.cc
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


#include "../include/frap.h"

int main(int argc, char* argv[]) {
	int c, index;
	char* closed; 
	char* prima;
	char* appname = argv[0];
	char* prefix;
	vector<std::string> ifiles;
	string file_name;
	bool verbose = false;
	
	prefix = "frap-data";
	int one = 1, fd;
	int multiplefiles = (argc > 1);
	
	while((c = getopt(argc, argv, "vp:c:f:")) != -1){			//taken from gnu example
		switch(c){
		case 'f':
			prefix = optarg;
		case 'v':
			verbose = true;
		case 'p':
			prima = optarg;
			break;
		case 'c':
			closed = optarg;
			break;
		default:
			fprintf(stderr, "usage: %s [-p background] [-c closed] others.tif ...\n", appname);
			exit(-1);
			
		}
	}

	printf ("prima = %s, closed = %s\n ", prima, closed);

	for (index = optind; index < argc; index++){
        	//printf ("Image files %s\n", argv[index]);
		file_name = argv[index];
		ifiles.push_back(file_name);
	}

	FrapTool::Frap *experiment;

	experiment = new FrapTool::Frap(prima,closed,verbose);
			
	experiment->doselection();
	experiment->setimagenames(ifiles);		
	experiment->processdata();

	//experiment->plplot_chart(prefix);
	experiment->print_data();
	experiment->save_data_file(prefix);

	delete experiment;
	
	return 0;
}

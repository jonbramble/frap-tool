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
	vector<char*> ifiles;

	int one = 1, fd;
	int multiplefiles = (argc > 1);
	
	while((c = getopt(argc, argv, "p:c:")) != -1){			//taken from gnu example
		switch(c){
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
		ifiles.push_back(argv[index]);
	}

	Frap *experiment;

	experiment = new Frap(prima,closed);
	
	experiment->start();				// starts do selection
	experiment->setimagenames(ifiles);		// but can do processing on tiff files in the meantime
	experiment->join();				// now we join the thread to continue with dataprocessing

	experiment->processdata();
	
	//FILE * f = fopen ("alldata.dat", "w");
       	//gsl_matrix_fprintf (f,  experiment.getdata(), "%.5g");
        //fclose (f);

	experiment->plplot_chart();

	delete experiment;
	
	return 0;
}

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

	experiment->plot_graph();

	delete experiment;
	
	return 0;
}

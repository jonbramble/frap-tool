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

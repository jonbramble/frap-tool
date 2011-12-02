#include "tiffio.h"
int main()
{
   	uint16 count;
	void *data;

 	TIFF* tif = TIFFOpen("prima.tif", "r");
	TIFFGetField(tif, 306, &count, &data);
	printf("Tag %d: %s, count %d0\n", 306, (char *)data, count);
	TIFFClose(tif);
    	return 0;
}

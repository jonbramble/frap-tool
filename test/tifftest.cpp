#include "tiffio.h"
main()
{
    TIFF* tif = TIFFOpen("horse.tif", "r");
    if (tif) {
        tdata_t buf;
        ttile_t tile;
        
        buf = _TIFFmalloc(TIFFTileSize(tif));
        for (tile = 0; tile < tiffnumberoftiles(tif); tile++)
            tiffreadencodedtile(tif, tile, buf, (tsize_t) -1);
        _tifffree(buf);
        tiffclose(tif);
    }
}

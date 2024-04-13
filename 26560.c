static int readContigStripsIntoBuffer (TIFF* in, uint8* buf)
 {
         uint8* bufp = buf;
         int32  bytes_read = 0;
        uint16 strip, nstrips   = TIFFNumberOfStrips(in);
         uint32 stripsize = TIFFStripSize(in);
         uint32 rows = 0;
         uint32 rps = TIFFGetFieldDefaulted(in, TIFFTAG_ROWSPERSTRIP, &rps);
        tsize_t scanline_size = TIFFScanlineSize(in);

        if (scanline_size == 0) {
                TIFFError("", "TIFF scanline size is zero!");    
                return 0;
        }

        for (strip = 0; strip < nstrips; strip++) {
                bytes_read = TIFFReadEncodedStrip (in, strip, bufp, -1);
                rows = bytes_read / scanline_size;
                if ((strip < (nstrips - 1)) && (bytes_read != (int32)stripsize))
                        TIFFError("", "Strip %d: read %lu bytes, strip size %lu",
                                  (int)strip + 1, (unsigned long) bytes_read,
                                  (unsigned long)stripsize);

                if (bytes_read < 0 && !ignore) {
                        TIFFError("", "Error reading strip %lu after %lu rows",
                                  (unsigned long) strip, (unsigned long)rows);
                        return 0;
                }
                bufp += bytes_read;
        }

        return 1;
} /* end readContigStripsIntoBuffer */

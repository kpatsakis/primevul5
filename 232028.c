int ImagingLibTiffMergeFieldInfo(ImagingCodecState state, TIFFDataType field_type, int key, int is_var_length){
    // Refer to libtiff docs (http://www.simplesystems.org/libtiff/addingtags.html)
    TIFFSTATE *clientstate = (TIFFSTATE *)state->context;
    char field_name[10];
    uint32 n;
    int status = 0;

    // custom fields added with ImagingLibTiffMergeFieldInfo are only used for
    // decoding, ignore readcount;
    int readcount = 0;
    // we support writing a single value, or a variable number of values
    int writecount = 1;
    // whether the first value should encode the number of values.
    int passcount = 0;

    TIFFFieldInfo info[] = {
        { key, readcount, writecount, field_type, FIELD_CUSTOM, 1, passcount, field_name }
    };

    if (is_var_length) {
        info[0].field_writecount = -1;
    }

    if (is_var_length && field_type != TIFF_ASCII) {
        info[0].field_passcount = 1;
    }

    n = sizeof(info) / sizeof(info[0]);

    // Test for libtiff 4.0 or later, excluding libtiff 3.9.6 and 3.9.7
#if TIFFLIB_VERSION >= 20111221 && TIFFLIB_VERSION != 20120218 && TIFFLIB_VERSION != 20120922
    status = TIFFMergeFieldInfo(clientstate->tiff, info, n);
#else
    TIFFMergeFieldInfo(clientstate->tiff, info, n);
#endif
    return status;
}
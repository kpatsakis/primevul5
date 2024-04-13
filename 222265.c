getTiffCompressedFormat(l_uint16  tiffcomp)
{
l_int32  comptype;

    switch (tiffcomp)
    {
    case COMPRESSION_CCITTFAX4:
        comptype = IFF_TIFF_G4;
        break;
    case COMPRESSION_CCITTFAX3:
        comptype = IFF_TIFF_G3;
        break;
    case COMPRESSION_CCITTRLE:
        comptype = IFF_TIFF_RLE;
        break;
    case COMPRESSION_PACKBITS:
        comptype = IFF_TIFF_PACKBITS;
        break;
    case COMPRESSION_LZW:
        comptype = IFF_TIFF_LZW;
        break;
    case COMPRESSION_ADOBE_DEFLATE:
        comptype = IFF_TIFF_ZIP;
        break;
    case COMPRESSION_JPEG:
        comptype = IFF_TIFF_JPEG;
        break;
    default:
        comptype = IFF_TIFF;
        break;
    }
    return comptype;
}
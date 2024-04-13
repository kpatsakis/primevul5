DwaCompressor::uncompressTile
    (const char *inPtr,
     int inSize,
     IMATH_NAMESPACE::Box2i range,
     const char *&outPtr)
{
    return uncompress (inPtr, inSize, range, outPtr);
}
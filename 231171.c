DwaCompressor::compressTile
    (const char             *inPtr,
     int                    inSize,
     IMATH_NAMESPACE::Box2i range,
     const char             *&outPtr)
{
    return compress (inPtr, inSize, range, outPtr);
}
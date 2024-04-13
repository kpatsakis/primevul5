DwaCompressor::compress
    (const char *inPtr,
     int inSize,
     int minY,
     const char *&outPtr)
{
    return compress
        (inPtr,
         inSize, 
         IMATH_NAMESPACE::Box2i (IMATH_NAMESPACE::V2i (_min[0], minY),
                                 IMATH_NAMESPACE::V2i (_max[0], minY + numScanLines() - 1)),
         outPtr);
}
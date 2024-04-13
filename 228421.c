FastHufDecoder::readBits
    (int numBits,
     Int64 &buffer,             // c
     int &bufferNumBits,        // lc
     const char *&currByte)     // in
{
    while (bufferNumBits < numBits)
    {
        buffer = (buffer << 8) | *(unsigned char*)(currByte++);
        bufferNumBits += 8;
    }

    bufferNumBits -= numBits;
    return (buffer >> bufferNumBits) & ((1 << numBits) - 1);
}
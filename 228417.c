FastHufDecoder::refill
    (Int64 &buffer,
     int numBits,                       // number of bits to refill
     Int64 &bufferBack,                 // the next 64-bits, to refill from
     int &bufferBackNumBits,            // number of bits left in bufferBack
     const unsigned char *&currByte,    // current byte in the bitstream
     int &currBitsLeft)                 // number of bits left in the bitsream
{
    // 
    // Refill bits into the bottom of buffer, from the top of bufferBack.
    // Always top up buffer to be completely full.
    //

    buffer |= bufferBack >> (64 - numBits);

    if (bufferBackNumBits < numBits)
    {
        numBits -= bufferBackNumBits;

        // 
        // Refill all of bufferBack from the bitstream. Either grab
        // a full 64-bit chunk, or whatever bytes are left. If we
        // don't have 64-bits left, pad with 0's.
        //

        if (currBitsLeft >= 64)
        {
            bufferBack        = READ64 (currByte); 
            bufferBackNumBits = 64;
            currByte         += sizeof (Int64);
            currBitsLeft     -= 8 * sizeof (Int64);

        }
        else
        {
            bufferBack        = 0;
            bufferBackNumBits = 64; 

            Int64 shift = 56;
            
            while (currBitsLeft > 0)
            {
                bufferBack |= ((Int64)(*currByte)) << shift;

                currByte++;
                shift        -= 8;
                currBitsLeft -= 8;
            }

            //
            // At this point, currBitsLeft might be negative, just because
            // we're subtracting whole bytes. To keep anyone from freaking
            // out, zero the counter.
            //

            if (currBitsLeft < 0)
                currBitsLeft = 0;
        }

        buffer |= bufferBack >> (64 - numBits);
    }
    
    bufferBack         = bufferBack << numBits;
    bufferBackNumBits -= numBits;

    // 
    // We can have cases where the previous shift of bufferBack is << 64 - 
    // in which case no shift occurs. The bit count math still works though,
    // so if we don't have any bits left, zero out bufferBack.
    //

    if (bufferBackNumBits == 0)
        bufferBack = 0;
}
FastHufDecoder::decode
    (const unsigned char *src,
     int numSrcBits,
     unsigned short *dst, 
     int numDstElems)
{
    if (numSrcBits < 128)
        throw IEX_NAMESPACE::InputExc ("Error choosing Huffman decoder implementation "
                                       "(insufficient number of bits).");

    //
    // Current position (byte/bit) in the src data stream
    // (after the first buffer fill)
    //

    const unsigned char *currByte = src + 2 * sizeof (Int64);

    numSrcBits -= 8 * 2 * sizeof (Int64);

    //
    // 64-bit buffer holding the current bits in the stream
    //

    Int64 buffer            = READ64 (src); 
    int   bufferNumBits     = 64;

    //
    // 64-bit buffer holding the next bits in the stream
    //

    Int64 bufferBack        = READ64 ((src + sizeof (Int64))); 
    int   bufferBackNumBits = 64;

    int dstIdx = 0;

    while (dstIdx < numDstElems)
    {
        int  codeLen;
        int  symbol;

        //
        // Test if we can be table accelerated. If so, directly
        // lookup the output symbol. Otherwise, we need to fall
        // back to searching for the code.
        //
        // If we're doing table lookups, we don't really need
        // a re-filled buffer, so long as we have TABLE_LOOKUP_BITS
        // left. But for a search, we do need a refilled table.
        //

        if (_tableMin <= buffer)
        {
            int tableIdx = buffer >> (64 - TABLE_LOOKUP_BITS);

            // 
            // For invalid codes, _tableCodeLen[] should return 0. This
            // will cause the decoder to get stuck in the current spot
            // until we run out of elements, then barf that the codestream
            // is bad.  So we don't need to stick a condition like
            //     if (codeLen > _maxCodeLength) in this inner.
            //

            codeLen = _tableCodeLen[tableIdx];
            symbol  = _tableSymbol[tableIdx];
        }
        else
        {
            if (bufferNumBits < 64)
            {
                refill (buffer,
                        64 - bufferNumBits,
                        bufferBack,
                        bufferBackNumBits,
                        currByte,
                        numSrcBits);

                bufferNumBits = 64;
            }

            // 
            // Brute force search: 
            // Find the smallest length where _ljBase[length] <= buffer
            //

            codeLen = TABLE_LOOKUP_BITS + 1;

            while (_ljBase[codeLen] > buffer && codeLen <= _maxCodeLength)
                codeLen++;

            if (codeLen > _maxCodeLength)
            {
                throw IEX_NAMESPACE::InputExc ("Huffman decode error "
                                               "(Decoded an invalid symbol).");
            }

            Int64 id = _ljOffset[codeLen] + (buffer >> (64 - codeLen));
            if (id < static_cast<Int64>(_numSymbols))
            {
                symbol = _idToSymbol[id];
            }
            else
            {
                throw IEX_NAMESPACE::InputExc ("Huffman decode error "
                                               "(Decoded an invalid symbol).");
            }
        }

        //
        // Shift over bit stream, and update the bit count in the buffer
        //

        buffer = buffer << codeLen;
        bufferNumBits -= codeLen;

        //
        // If we recieved a RLE symbol (_rleSymbol), then we need
        // to read ahead 8 bits to know how many times to repeat
        // the previous symbol. Need to ensure we at least have
        // 8 bits of data in the buffer
        //

        if (symbol == _rleSymbol)
        {
            if (bufferNumBits < 8)
            {
                refill (buffer,
                        64 - bufferNumBits,
                        bufferBack,
                        bufferBackNumBits,
                        currByte,
                        numSrcBits);

                bufferNumBits = 64;
            }

            int rleCount = buffer >> 56;

            if (dstIdx < 1)
            {
                throw IEX_NAMESPACE::InputExc ("Huffman decode error (RLE code "
                                               "with no previous symbol).");
            }

            if (dstIdx + rleCount > numDstElems)
            {
                throw IEX_NAMESPACE::InputExc ("Huffman decode error (Symbol run "
                                               "beyond expected output buffer length).");
            }

            if (rleCount <= 0) 
            {
                throw IEX_NAMESPACE::InputExc("Huffman decode error"
                                              " (Invalid RLE length)");
            }

            for (int i = 0; i < rleCount; ++i)
                dst[dstIdx + i] = dst[dstIdx - 1];

            dstIdx += rleCount;

            buffer = buffer << 8;
            bufferNumBits -= 8;
        }
        else
        {
            dst[dstIdx] = symbol;
            dstIdx++;
        }

        //
        // refill bit stream buffer if we're below the number of 
        // bits needed for a table lookup
        //

        if (bufferNumBits < TABLE_LOOKUP_BITS)
        {
            refill (buffer,
                    64 - bufferNumBits,
                    bufferBack,
                    bufferBackNumBits,
                    currByte,
                    numSrcBits);

            bufferNumBits = 64;
        }
    }

    if (numSrcBits != 0)
    {
        throw IEX_NAMESPACE::InputExc ("Huffman decode error (Compressed data remains "
                                       "after filling expected output buffer).");
    }
}
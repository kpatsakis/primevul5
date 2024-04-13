DwaCompressor::compress
    (const char             *inPtr,
     int                    inSize,
     IMATH_NAMESPACE::Box2i range,
     const char             *&outPtr)
{
    const char *inDataPtr   = inPtr;
    char       *packedAcEnd = 0;
    char       *packedDcEnd = 0; 
    int         fileVersion = 2;   // Starting with 2, we write the channel
                                   // classification rules into the file

    if (fileVersion < 2) 
        initializeLegacyChannelRules();
    else 
        initializeDefaultChannelRules();

    size_t outBufferSize = 0;
    initializeBuffers(outBufferSize);

    unsigned short          channelRuleSize = 0;
    std::vector<Classifier> channelRules;
    if (fileVersion >= 2) 
    {
        relevantChannelRules(channelRules);

        channelRuleSize = Xdr::size<unsigned short>();
        for (size_t i = 0; i < channelRules.size(); ++i) 
            channelRuleSize += channelRules[i].size();
    }

    //
    // Remember to allocate _outBuffer, if we haven't done so already.
    //

    outBufferSize += channelRuleSize;
    if (outBufferSize > _outBufferSize) 
    {
        _outBufferSize = outBufferSize;
        if (_outBuffer != 0)
            delete[] _outBuffer;       
        _outBuffer = new char[outBufferSize];
    }

    char *outDataPtr = &_outBuffer[NUM_SIZES_SINGLE * sizeof(OPENEXR_IMF_NAMESPACE::Int64) +
                                   channelRuleSize];

    //
    // We might not be dealing with any color data, in which
    // case the AC buffer size will be 0, and deferencing
    // a vector will not be a good thing to do.
    //

    if (_packedAcBuffer)
        packedAcEnd = _packedAcBuffer;

    if (_packedDcBuffer)
        packedDcEnd = _packedDcBuffer;

    #define OBIDX(x) (Int64 *)&_outBuffer[x * sizeof (Int64)]

    Int64 *version                 = OBIDX (VERSION);
    Int64 *unknownUncompressedSize = OBIDX (UNKNOWN_UNCOMPRESSED_SIZE);
    Int64 *unknownCompressedSize   = OBIDX (UNKNOWN_COMPRESSED_SIZE);
    Int64 *acCompressedSize        = OBIDX (AC_COMPRESSED_SIZE);
    Int64 *dcCompressedSize        = OBIDX (DC_COMPRESSED_SIZE);
    Int64 *rleCompressedSize       = OBIDX (RLE_COMPRESSED_SIZE);
    Int64 *rleUncompressedSize     = OBIDX (RLE_UNCOMPRESSED_SIZE);
    Int64 *rleRawSize              = OBIDX (RLE_RAW_SIZE);

    Int64 *totalAcUncompressedCount = OBIDX (AC_UNCOMPRESSED_COUNT);
    Int64 *totalDcUncompressedCount = OBIDX (DC_UNCOMPRESSED_COUNT);

    Int64 *acCompression            = OBIDX (AC_COMPRESSION);

    int minX   = range.min.x;
    int maxX   = std::min(range.max.x, _max[0]);
    int minY   = range.min.y;
    int maxY   = std::min(range.max.y, _max[1]);

    //
    // Zero all the numbers in the chunk header
    //

    memset (_outBuffer, 0, NUM_SIZES_SINGLE * sizeof (Int64));

    //
    // Setup the AC compression strategy and the version in the data block,
    // then write the relevant channel classification rules if needed
    //
    *version       = fileVersion;  
    *acCompression = _acCompression;

    setupChannelData (minX, minY, maxX, maxY);

    if (fileVersion >= 2) 
    {
        char *writePtr = &_outBuffer[NUM_SIZES_SINGLE * sizeof(OPENEXR_IMF_NAMESPACE::Int64)];
        Xdr::write<CharPtrIO> (writePtr, channelRuleSize);
        
        for (size_t i = 0; i < channelRules.size(); ++i) 
            channelRules[i].write(writePtr);
    }

    //
    // Determine the start of each row in the input buffer
    // Channels are interleaved by scanline
    //

    std::vector<bool> encodedChannels (_channelData.size());
    std::vector< std::vector<const char *> > rowPtrs (_channelData.size());

    for (unsigned int chan = 0; chan < _channelData.size(); ++chan)
        encodedChannels[chan] = false;

    inDataPtr =  inPtr;

    for (int y = minY; y <= maxY; ++y)
    {
        for (unsigned int chan = 0; chan < _channelData.size(); ++chan)
        {

            ChannelData *cd = &_channelData[chan];

            if (IMATH_NAMESPACE::modp(y, cd->ySampling) != 0)
                continue;

            rowPtrs[chan].push_back(inDataPtr);
            inDataPtr += cd->width * OPENEXR_IMF_NAMESPACE::pixelTypeSize(cd->type);
        }
    }

    inDataPtr = inPtr;

    // 
    // Make a pass over all our CSC sets and try to encode them first
    // 

    for (unsigned int csc = 0; csc < _cscSets.size(); ++csc)
    {

        LossyDctEncoderCsc encoder
            (_dwaCompressionLevel / 100000.f,
             rowPtrs[_cscSets[csc].idx[0]],
             rowPtrs[_cscSets[csc].idx[1]],
             rowPtrs[_cscSets[csc].idx[2]],
             packedAcEnd,
             packedDcEnd,
             dwaCompressorToNonlinear,
             _channelData[_cscSets[csc].idx[0]].width,
             _channelData[_cscSets[csc].idx[0]].height,
             _channelData[_cscSets[csc].idx[0]].type,
             _channelData[_cscSets[csc].idx[1]].type,
             _channelData[_cscSets[csc].idx[2]].type);

        encoder.execute();

        *totalAcUncompressedCount  += encoder.numAcValuesEncoded();
        *totalDcUncompressedCount  += encoder.numDcValuesEncoded();

        packedAcEnd += encoder.numAcValuesEncoded() * sizeof(unsigned short);
        packedDcEnd += encoder.numDcValuesEncoded() * sizeof(unsigned short);

        encodedChannels[_cscSets[csc].idx[0]] = true;
        encodedChannels[_cscSets[csc].idx[1]] = true;
        encodedChannels[_cscSets[csc].idx[2]] = true;
    }

    for (unsigned int chan = 0; chan < _channelData.size(); ++chan)
    {
        ChannelData *cd = &_channelData[chan];

        if (encodedChannels[chan])
            continue;

        switch (cd->compression)
        {
          case LOSSY_DCT:

            //
            // For LOSSY_DCT, treat this just like the CSC'd case,
            // but only operate on one channel
            //

            {
                const unsigned short *nonlinearLut = 0;

                if (!cd->pLinear)
                    nonlinearLut = dwaCompressorToNonlinear; 

                LossyDctEncoder encoder
                    (_dwaCompressionLevel / 100000.f,
                     rowPtrs[chan],
                     packedAcEnd,
                     packedDcEnd,
                     nonlinearLut,
                     cd->width,
                     cd->height,
                     cd->type);

                encoder.execute();

                *totalAcUncompressedCount  += encoder.numAcValuesEncoded();
                *totalDcUncompressedCount  += encoder.numDcValuesEncoded();

                packedAcEnd +=
                    encoder.numAcValuesEncoded() * sizeof (unsigned short);

                packedDcEnd +=
                    encoder.numDcValuesEncoded() * sizeof (unsigned short);
            }

            break;

          case RLE:

            //
            // For RLE, bash the bytes up so that the first bytes of each
            // pixel are contingous, as are the second bytes, and so on.
            //

            for (unsigned int y = 0; y < rowPtrs[chan].size(); ++y)
            {
                const char *row = rowPtrs[chan][y];

                for (int x = 0; x < cd->width; ++x)
                {
                    for (int byte = 0;
                         byte < OPENEXR_IMF_NAMESPACE::pixelTypeSize (cd->type);
                         ++byte)
                    {
                            
                        *cd->planarUncRleEnd[byte]++ = *row++;
                    }
                }

                *rleRawSize += cd->width * OPENEXR_IMF_NAMESPACE::pixelTypeSize(cd->type);
            }

            break;

          case UNKNOWN:
           
            //
            // Otherwise, just copy data over verbatim
            //

            {
                int scanlineSize = cd->width * OPENEXR_IMF_NAMESPACE::pixelTypeSize(cd->type);

                for (unsigned int y = 0; y < rowPtrs[chan].size(); ++y)
                {
                    memcpy (cd->planarUncBufferEnd,
                            rowPtrs[chan][y],
                            scanlineSize);
    
                    cd->planarUncBufferEnd += scanlineSize;
                }

                *unknownUncompressedSize += cd->planarUncSize;
            }

            break;

          default:

            assert (false);
        }

        encodedChannels[chan] = true;
    }

    //
    // Pack the Unknown data into the output buffer first. Instead of
    // just copying it uncompressed, try zlib compression at least.
    //

    if (*unknownUncompressedSize > 0)
    {
        uLongf inSize  = (uLongf)(*unknownUncompressedSize);
        uLongf outSize = compressBound (inSize);

        if (Z_OK != ::compress2 ((Bytef *)outDataPtr,
                                 &outSize,
                                 (const Bytef *)_planarUncBuffer[UNKNOWN],
                                 inSize,
                                 9))
        {
            throw IEX_NAMESPACE::BaseExc ("Data compression (zlib) failed.");
        }

        outDataPtr += outSize;
        *unknownCompressedSize = outSize;
    }

    //
    // Now, pack all the Lossy DCT coefficients into our output
    // buffer, with Huffman encoding.
    //
    // Also, record the compressed size and the number of 
    // uncompressed componentns we have.
    //

    if (*totalAcUncompressedCount > 0)
    { 
        switch (_acCompression)
        {
          case STATIC_HUFFMAN:

            *acCompressedSize = (int)
                hufCompress((unsigned short *)_packedAcBuffer,
                            (int)*totalAcUncompressedCount,
                            outDataPtr);                
            break;

          case DEFLATE:

            {
                uLongf destLen = compressBound (
                    (*totalAcUncompressedCount) * sizeof (unsigned short));

                if (Z_OK != ::compress2
                                ((Bytef *)outDataPtr,
                                 &destLen,
                                 (Bytef *)_packedAcBuffer, 
                                 (uLong)(*totalAcUncompressedCount
                                                * sizeof (unsigned short)),
                                 9))
                {
                    throw IEX_NAMESPACE::InputExc ("Data compression (zlib) failed.");
                }

                *acCompressedSize = destLen;        
            }

            break;

          default:
            
            assert (false);
        }

        outDataPtr += *acCompressedSize;
    }

    // 
    // Handle the DC components separately
    //

    if (*totalDcUncompressedCount > 0)
    {
        *dcCompressedSize = _zip->compress
            (_packedDcBuffer,
             (int)(*totalDcUncompressedCount) * sizeof (unsigned short),
             outDataPtr);

        outDataPtr += *dcCompressedSize;
    }

    // 
    // If we have RLE data, first RLE encode it and set the uncompressed
    // size. Then, deflate the results and set the compressed size.
    //    

    if (*rleRawSize > 0)
    {
        *rleUncompressedSize = rleCompress
            ((int)(*rleRawSize),
             _planarUncBuffer[RLE],
             (signed char *)_rleBuffer);

        uLongf dstLen = compressBound ((uLongf)*rleUncompressedSize);

        if (Z_OK != ::compress2
                        ((Bytef *)outDataPtr, 
                         &dstLen, 
                         (Bytef *)_rleBuffer, 
                         (uLong)(*rleUncompressedSize),
                         9))
        {
            throw IEX_NAMESPACE::BaseExc ("Error compressing RLE'd data.");
        }
        
       *rleCompressedSize = dstLen;
        outDataPtr       += *rleCompressedSize;
    }

    // 
    // Flip the counters to XDR format
    //         

    for (int i = 0; i < NUM_SIZES_SINGLE; ++i)
    {
        Int64  src = *(((Int64 *)_outBuffer) + i);
        char  *dst = (char *)(((Int64 *)_outBuffer) + i);

        Xdr::write<CharPtrIO> (dst, src);
    }

    //
    // We're done - compute the number of bytes we packed
    //

    outPtr = _outBuffer;

    return static_cast<int>(outDataPtr - _outBuffer + 1);
}
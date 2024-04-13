DwaCompressor::initializeBuffers (size_t &outBufferSize)
{
    classifyChannels (_channels, _channelData, _cscSets);

    //
    // _outBuffer needs to be big enough to hold all our 
    // compressed data - which could vary depending on what sort
    // of channels we have. 
    //

    int maxOutBufferSize  = 0;
    int numLossyDctChans  = 0;
    int unknownBufferSize = 0;
    int rleBufferSize     = 0;

    int maxLossyDctAcSize = (int)ceil ((float)numScanLines() / 8.0f) * 
                            (int)ceil ((float)(_max[0] - _min[0] + 1) / 8.0f) *
                            63 * sizeof (unsigned short);

    int maxLossyDctDcSize = (int)ceil ((float)numScanLines() / 8.0f) * 
                            (int)ceil ((float)(_max[0] - _min[0] + 1) / 8.0f) *
                            sizeof (unsigned short);

    for (unsigned int chan = 0; chan < _channelData.size(); ++chan)
    {
        switch (_channelData[chan].compression)
        {
          case LOSSY_DCT:

            //
            // This is the size of the number of packed
            // components, plus the requirements for
            // maximum Huffman encoding size (for STATIC_HUFFMAN)
            // or for zlib compression (for DEFLATE)
            //

            maxOutBufferSize += std::max(
                            (int)(2 * maxLossyDctAcSize + 65536),
                            (int)compressBound (maxLossyDctAcSize) );
            numLossyDctChans++;
            break;

          case RLE:
            {
                //
                // RLE, if gone horribly wrong, could double the size
                // of the source data.
                //

                int rleAmount = 2 * numScanLines() * (_max[0] - _min[0] + 1) *
                                OPENEXR_IMF_NAMESPACE::pixelTypeSize (_channelData[chan].type);

                rleBufferSize += rleAmount;
            }
            break;


          case UNKNOWN:

            unknownBufferSize += numScanLines() * (_max[0] - _min[0] + 1) *
                                 OPENEXR_IMF_NAMESPACE::pixelTypeSize (_channelData[chan].type);
            break;

          default:

            throw IEX_NAMESPACE::NoImplExc ("Unhandled compression scheme case");
            break;
        }
    }

    //
    // Also, since the results of the RLE are packed into 
    // the output buffer, we need the extra room there. But
    // we're going to zlib compress() the data we pack, 
    // which could take slightly more space
    //

    maxOutBufferSize += (int)compressBound ((uLongf)rleBufferSize);
    
    //
    // And the same goes for the UNKNOWN data
    //

    maxOutBufferSize += (int)compressBound ((uLongf)unknownBufferSize);

    //
    // Allocate a zip/deflate compressor big enought to hold the DC data
    // and include it's compressed results in the size requirements
    // for our output buffer
    //

    if (_zip == 0) 
        _zip = new Zip (maxLossyDctDcSize * numLossyDctChans);
    else if (_zip->maxRawSize() < static_cast<size_t>(maxLossyDctDcSize * numLossyDctChans))
    {
        delete _zip;
        _zip = new Zip (maxLossyDctDcSize * numLossyDctChans);
    }


    maxOutBufferSize += _zip->maxCompressedSize();

    //
    // We also need to reserve space at the head of the buffer to 
    // write out the size of our various packed and compressed data.
    //

    maxOutBufferSize += NUM_SIZES_SINGLE * sizeof (Int64); 
                    

    //
    // Later, we're going to hijack outBuffer for the result of
    // both encoding and decoding. So it needs to be big enough
    // to hold either a buffers' worth of uncompressed or
    // compressed data
    //
    // For encoding, we'll need _outBuffer to hold maxOutBufferSize bytes,
    // but for decoding, we only need it to be maxScanLineSize*numScanLines.
    // Cache the max size for now, and alloc the buffer when we either
    // encode or decode.
    //

    outBufferSize = maxOutBufferSize;


    //
    // _packedAcBuffer holds the quantized DCT coefficients prior
    // to Huffman encoding
    //

    if (static_cast<size_t>(maxLossyDctAcSize * numLossyDctChans) > _packedAcBufferSize)
    {
        _packedAcBufferSize = maxLossyDctAcSize * numLossyDctChans;
        if (_packedAcBuffer != 0) 
            delete[] _packedAcBuffer;
        _packedAcBuffer = new char[_packedAcBufferSize];
    }

    //
    // _packedDcBuffer holds one quantized DCT coef per 8x8 block
    //

    if (static_cast<size_t>(maxLossyDctDcSize * numLossyDctChans) > _packedDcBufferSize)
    {
        _packedDcBufferSize = maxLossyDctDcSize * numLossyDctChans;
        if (_packedDcBuffer != 0) 
            delete[] _packedDcBuffer;
        _packedDcBuffer     = new char[_packedDcBufferSize];
    }

    if (static_cast<size_t>(rleBufferSize) > _rleBufferSize) 
    {
        _rleBufferSize = rleBufferSize;
        if (_rleBuffer != 0) 
            delete[] _rleBuffer;
        _rleBuffer = new char[rleBufferSize];
    }

    // 
    // The planar uncompressed buffer will hold float data for LOSSY_DCT
    // compressed values, and whatever the native type is for other
    // channels. We're going to use this to hold data in a planar
    // format, as opposed to the native interleaved format we take
    // into compress() and give back from uncompress().
    //
    // This also makes it easier to compress the UNKNOWN and RLE data
    // all in one swoop (for each compression scheme).
    //

    int planarUncBufferSize[NUM_COMPRESSOR_SCHEMES];
    for (int i=0; i<NUM_COMPRESSOR_SCHEMES; ++i)
        planarUncBufferSize[i] = 0;

    for (unsigned int chan = 0; chan < _channelData.size(); ++chan)
    {
        switch (_channelData[chan].compression)
        {
          case LOSSY_DCT:
            break;

          case RLE:
            planarUncBufferSize[RLE] +=
                     numScanLines() * (_max[0] - _min[0] + 1) *
                     OPENEXR_IMF_NAMESPACE::pixelTypeSize (_channelData[chan].type);
            break;

          case UNKNOWN: 
            planarUncBufferSize[UNKNOWN] +=
                     numScanLines() * (_max[0] - _min[0] + 1) *
                     OPENEXR_IMF_NAMESPACE::pixelTypeSize (_channelData[chan].type);
            break;

          default:
            throw IEX_NAMESPACE::NoImplExc ("Unhandled compression scheme case");
            break;
        }
    }

    //
    // UNKNOWN data is going to be zlib compressed, which needs 
    // a little extra headroom
    //

    if (planarUncBufferSize[UNKNOWN] > 0)
    {
        planarUncBufferSize[UNKNOWN] = 
            compressBound ((uLongf)planarUncBufferSize[UNKNOWN]);
    }

    for (int i = 0; i < NUM_COMPRESSOR_SCHEMES; ++i)
    {
        if (static_cast<size_t>(planarUncBufferSize[i]) > _planarUncBufferSize[i]) 
        {
            _planarUncBufferSize[i] = planarUncBufferSize[i];
            if (_planarUncBuffer[i] != 0) 
                delete[] _planarUncBuffer[i];
            _planarUncBuffer[i] = new char[planarUncBufferSize[i]];
        }
    }
}
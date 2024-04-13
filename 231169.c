DwaCompressor::LossyDctEncoderBase::execute ()
{
    int  numBlocksX   = (int)ceil ((float)_width / 8.0f);
    int  numBlocksY   = (int)ceil ((float)_height/ 8.0f);

    half halfZigCoef[64]; 
    half halfCoef[64];

    std::vector<unsigned short *> currDcComp (_rowPtrs.size());
    unsigned short               *currAcComp = (unsigned short *)_packedAc;

    _dctData.resize (_rowPtrs.size());
    _numAcComp = 0;
    _numDcComp = 0;
 
    assert (_type.size() == _rowPtrs.size());
    assert ((_rowPtrs.size() == 3) || (_rowPtrs.size() == 1));

    // 
    // Allocate a temp half buffer to quantize into for
    // any FLOAT source channels.
    //

    int tmpHalfBufferElements = 0;

    for (unsigned int chan = 0; chan < _rowPtrs.size(); ++chan)
        if (_type[chan] == FLOAT)
            tmpHalfBufferElements += _width * _height;

    std::vector<unsigned short> tmpHalfBuffer (tmpHalfBufferElements);

    char *tmpHalfBufferPtr = 0;

    if (tmpHalfBufferElements)
        tmpHalfBufferPtr = (char *)&tmpHalfBuffer[0];

    //
    // Run over all the float scanlines, quantizing, 
    // and re-assigning _rowPtr[y]. We need to translate
    // FLOAT XDR to HALF XDR.
    //

    for (unsigned int chan = 0; chan < _rowPtrs.size(); ++chan)
    {
        if (_type[chan] != FLOAT)
            continue;
    
        for (int y = 0; y < _height; ++y)
        {
            float       src = 0;
            const char *srcXdr = _rowPtrs[chan][y];
            char       *dstXdr = tmpHalfBufferPtr;
           
            for (int x = 0; x < _width; ++x)
            {

                Xdr::read<CharPtrIO> (srcXdr, src);

                //
                // Clamp to half ranges, instead of just casting. This
                // avoids introducing Infs which end up getting zeroed later
                //
                src = std::max (
                    std::min ((float) std::numeric_limits<half>::max(), src),
                              (float)-std::numeric_limits<half>::max());

                Xdr::write<CharPtrIO> (dstXdr, ((half)src).bits());

                //
                // Xdr::read and Xdr::write will advance the ptr
                //
            }

            _rowPtrs[chan][y] = (const char *)tmpHalfBufferPtr;
            tmpHalfBufferPtr += _width * sizeof (unsigned short);
        }
    }

    //
    // Pack DC components together by common plane, so we can get 
    // a little more out of differencing them. We'll always have 
    // one component per block, so we can computed offsets.
    //

    currDcComp[0] = (unsigned short *)_packedDc;

    for (unsigned int chan = 1; chan < _rowPtrs.size(); ++chan)
        currDcComp[chan] = currDcComp[chan-1] + numBlocksX * numBlocksY;

    for (int blocky = 0; blocky < numBlocksY; ++blocky)
    {
        for (int blockx = 0; blockx < numBlocksX; ++blockx)
        {
            half           h;
            unsigned short tmpShortXdr, tmpShortNative;
            char          *tmpCharPtr;

            for (unsigned int chan = 0; chan < _rowPtrs.size(); ++chan)
            {
                //
                // Break the source into 8x8 blocks. If we don't
                // fit at the edges, mirror.
                //
                // Also, convert from linear to nonlinear representation.
                // Our source is assumed to be XDR, and we need to convert
                // to NATIVE prior to converting to float.
                //
                // If we're converting linear -> nonlinear, assume that the
                // XDR -> NATIVE conversion is built into the lookup. Otherwise,
                // we'll need to explicitly do it.
                //

                for (int y = 0; y < 8; ++y)
                {
                    for (int x = 0; x < 8; ++x)
                    {
                        int vx = 8 * blockx + x;
                        int vy = 8 * blocky + y;

                        if (vx >= _width)
                            vx = _width - (vx - (_width - 1));
                        
                        if (vx < 0) vx = _width-1;

                        if (vy >=_height)
                            vy = _height - (vy - (_height - 1));

                        if (vy < 0) vy = _height-1;
                    
                        tmpShortXdr =
                            ((const unsigned short *)(_rowPtrs[chan])[vy])[vx];

                        if (_toNonlinear)
                        {
                            h.setBits (_toNonlinear[tmpShortXdr]);
                        }
                        else
                        {
                            const char *tmpConstCharPtr =
                                (const char *)(&tmpShortXdr);

                            Xdr::read<CharPtrIO>
                                (tmpConstCharPtr, tmpShortNative);

                            h.setBits(tmpShortNative);
                        }

                        _dctData[chan]._buffer[y * 8 + x] = (float)h;
                    } // x
                } // y
            } // chan

            //
            // Color space conversion
            //

            if (_rowPtrs.size() == 3)
            {
                csc709Forward64 (_dctData[0]._buffer, 
                                 _dctData[1]._buffer, 
                                 _dctData[2]._buffer);
            }

            for (unsigned int chan = 0; chan < _rowPtrs.size(); ++chan)
            {
                //
                // Forward DCT
                //

                dctForward8x8(_dctData[chan]._buffer);

                //
                // Quantize to half, and zigzag
                //

                if (chan == 0)
                {
                    for (int i = 0; i < 64; ++i)
                    {
                        halfCoef[i] =
                            quantize ((half)_dctData[chan]._buffer[i],
                                      _quantBaseError*_quantTableY[i]);
                    }
                }
                else
                {
                    for (int i = 0; i < 64; ++i)
                    {
                        halfCoef[i] =
                            quantize ((half)_dctData[chan]._buffer[i],
                                      _quantBaseError*_quantTableCbCr[i]);
                    }
                }

                toZigZag (halfZigCoef, halfCoef);
                
                //
                // Convert from NATIVE back to XDR, before we write out
                //

                for (int i = 0; i < 64; ++i)
                {
                    tmpCharPtr = (char *)&tmpShortXdr;
                    Xdr::write<CharPtrIO>(tmpCharPtr, halfZigCoef[i].bits());
                    halfZigCoef[i].setBits(tmpShortXdr);
                }

                //
                // Save the DC component separately, to be compressed on
                // its own.
                //

                *currDcComp[chan]++ = halfZigCoef[0].bits();
                _numDcComp++;
                
                //
                // Then RLE the AC components (which will record the count
                // of the resulting number of items)
                //

                rleAc (halfZigCoef, currAcComp);
            } // chan
        } // blockx
    } // blocky              
}
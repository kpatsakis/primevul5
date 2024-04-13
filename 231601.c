LineBufferTask::execute ()
{
    try
    {
        //
        // Uncompress the data, if necessary
        //
    
        if (_lineBuffer->uncompressedData == 0)
        {
            size_t uncompressedSize = 0;
            int maxY = min (_lineBuffer->maxY, _ifd->maxY);
    
            for (int i = _lineBuffer->minY - _ifd->minY;
                 i <= maxY - _ifd->minY;
		 ++i)
	    {
                uncompressedSize += _ifd->bytesPerLine[i];
	    }
    
            if (_lineBuffer->compressor &&
                static_cast<size_t>(_lineBuffer->dataSize) < uncompressedSize)
            {
                _lineBuffer->format = _lineBuffer->compressor->format();

                _lineBuffer->dataSize = _lineBuffer->compressor->uncompress
                    (_lineBuffer->buffer,
                     _lineBuffer->dataSize,
		     _lineBuffer->minY,
                     _lineBuffer->uncompressedData);
            }
            else
            {
                //
                // If the line is uncompressed, it's in XDR format,
                // regardless of the compressor's output format.
                //
    
                _lineBuffer->format = Compressor::XDR;
                _lineBuffer->uncompressedData = _lineBuffer->buffer;
            }
        }
        
        int yStart, yStop, dy;

        if (_ifd->lineOrder == INCREASING_Y)
        {
            yStart = _scanLineMin;
            yStop = _scanLineMax + 1;
            dy = 1;
        }
        else
        {
            yStart = _scanLineMax;
            yStop = _scanLineMin - 1;
            dy = -1;
        }
    
        for (int y = yStart; y != yStop; y += dy)
        {
            //
            // Convert one scan line's worth of pixel data back
            // from the machine-independent representation, and
            // store the result in the frame buffer.
            //
    
            const char *readPtr = _lineBuffer->uncompressedData +
                                  _ifd->offsetInLineBuffer[y - _ifd->minY];
    
            //
            // Iterate over all image channels.
            //
    
            for (unsigned int i = 0; i < _ifd->slices.size(); ++i)
            {
                //
                // Test if scan line y of this channel contains any data
		// (the scan line contains data only if y % ySampling == 0).
                //
    
                const InSliceInfo &slice = _ifd->slices[i];
    
                if (modp (y, slice.ySampling) != 0)
                    continue;
    
                //
                // Find the x coordinates of the leftmost and rightmost
                // sampled pixels (i.e. pixels within the data window
                // for which x % xSampling == 0).
                //
    
                int dMinX = divp (_ifd->minX, slice.xSampling);
                int dMaxX = divp (_ifd->maxX, slice.xSampling);
    
                //
		// Fill the frame buffer with pixel data.
                //
    
                if (slice.skip)
                {
                    //
                    // The file contains data for this channel, but
                    // the frame buffer contains no slice for this channel.
                    //
    
                    skipChannel (readPtr, slice.typeInFile, dMaxX - dMinX + 1);
                }
                else
                {
                    //
                    // The frame buffer contains a slice for this channel.
                    //
    
                    intptr_t base = reinterpret_cast<intptr_t>(slice.base);

                    intptr_t linePtr  = base +
                                        intptr_t( divp (y, slice.ySampling) ) *
                                        intptr_t( slice.yStride );
    
                    char *writePtr = reinterpret_cast<char*> (linePtr + intptr_t( dMinX ) * intptr_t( slice.xStride ));
                    char *endPtr   = reinterpret_cast<char*> (linePtr + intptr_t( dMaxX ) * intptr_t( slice.xStride ));
                    
                    copyIntoFrameBuffer (readPtr, writePtr, endPtr,
                                         slice.xStride, slice.fill,
                                         slice.fillValue, _lineBuffer->format,
                                         slice.typeInFrameBuffer,
                                         slice.typeInFile);
                }
            }
        }
    }
    catch (std::exception &e)
    {
        if (!_lineBuffer->hasException)
        {
            _lineBuffer->exception = e.what();
            _lineBuffer->hasException = true;
        }
    }
    catch (...)
    {
        if (!_lineBuffer->hasException)
        {
            _lineBuffer->exception = "unrecognized exception";
            _lineBuffer->hasException = true;
        }
    }
}
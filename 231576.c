LineBufferTaskIIF::execute()
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
                
                _lineBuffer->dataSize =
                _lineBuffer->compressor->uncompress (_lineBuffer->buffer,
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
            if (modp (y, _optimizationMode._ySampling) != 0)
                continue;
            
            //
            // Convert one scan line's worth of pixel data back
            // from the machine-independent representation, and
            // store the result in the frame buffer.
            //
            
            // Set the readPtr to read at the start of uncompressedData
            // but with an offet based on calculated array.
            // _ifd->offsetInLineBuffer contains offsets based on which
            // line we are currently processing.
            // Stride will be taken into consideration later.
                
                
            const char* readPtr = _lineBuffer->uncompressedData +
            _ifd->offsetInLineBuffer[y - _ifd->minY];
            
            size_t pixelsToCopySSE = 0;
            size_t pixelsToCopyNormal = 0;
            
            unsigned short* writePtrLeft = 0;
            unsigned short* writePtrRight = 0;
            
            size_t channels = _ifd->optimizationData.size();
       
            if(channels>4)
            {
                getWritePointerStereo<half>(y, writePtrRight, writePtrLeft, pixelsToCopySSE, pixelsToCopyNormal);
            }
            else 
            {
                getWritePointer<half>(y, writePtrRight, pixelsToCopySSE, pixelsToCopyNormal);
            }
            
            if (writePtrRight == 0 && pixelsToCopySSE == 0 && pixelsToCopyNormal == 0)
            {
                continue;
            }
            
            
            //
            // support reading up to eight channels
            //
            unsigned short* readPointers[8];
            
            for (size_t i = 0; i < channels ; ++i)
            {
                readPointers[i] = (unsigned short*)readPtr + (_ifd->optimizationData[i].offset * (pixelsToCopySSE * 8 + pixelsToCopyNormal));
            }
            
            //RGB only
            if(channels==3 || channels == 6 )
            {
                    optimizedWriteToRGB(readPointers[0], readPointers[1], readPointers[2], writePtrRight, pixelsToCopySSE, pixelsToCopyNormal);
                  
                    //stereo RGB
                    if( channels == 6)
                    {
                        optimizedWriteToRGB(readPointers[3], readPointers[4], readPointers[5], writePtrLeft, pixelsToCopySSE, pixelsToCopyNormal);
                    }                
            //RGBA
            }else if(channels==4 || channels==8)
            {
                
                if(_ifd->optimizationData[3].fill)
                {
                    optimizedWriteToRGBAFillA(readPointers[0], readPointers[1], readPointers[2], _ifd->optimizationData[3].fillValue.bits() , writePtrRight, pixelsToCopySSE, pixelsToCopyNormal);
                }else{
                    optimizedWriteToRGBA(readPointers[0], readPointers[1], readPointers[2], readPointers[3] , writePtrRight, pixelsToCopySSE, pixelsToCopyNormal);
                }
                
                //stereo RGBA
                if( channels == 8)
                {
                    if(_ifd->optimizationData[7].fill)
                    {
                        optimizedWriteToRGBAFillA(readPointers[4], readPointers[5], readPointers[6], _ifd->optimizationData[7].fillValue.bits() , writePtrLeft, pixelsToCopySSE, pixelsToCopyNormal);
                    }else{
                        optimizedWriteToRGBA(readPointers[4], readPointers[5], readPointers[6], readPointers[7] , writePtrLeft, pixelsToCopySSE, pixelsToCopyNormal);
                    }
                }
            }
            else {
                throw(IEX_NAMESPACE::LogicExc("IIF mode called with incorrect channel pattern"));
            }
            
            // If we are in NO_OPTIMIZATION mode, this class will never
            // get instantiated, so no need to check for it and duplicate
            // the code.
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
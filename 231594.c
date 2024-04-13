ScanLineInputFile::rawPixelData (int firstScanLine,
				 const char *&pixelData,
				 int &pixelDataSize)
{
    try
    {
#if ILMBASE_THREADING_ENABLED
        std::lock_guard<std::mutex> lock (*_streamData);
#endif
	if (firstScanLine < _data->minY || firstScanLine > _data->maxY)
	{
	    throw IEX_NAMESPACE::ArgExc ("Tried to read scan line outside "
			       "the image file's data window.");
	}

        int minY = lineBufferMinY
	    (firstScanLine, _data->minY, _data->linesInBuffer);

	readPixelData
	    (_streamData, _data, minY, _data->lineBuffers[0]->buffer, pixelDataSize);

	pixelData = _data->lineBuffers[0]->buffer;
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
	REPLACE_EXC (e, "Error reading pixel data from image "
                 "file \"" << fileName() << "\". " << e.what());
	throw;
    }
}
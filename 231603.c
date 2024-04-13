ScanLineInputFile::readPixels (int scanLine1, int scanLine2)
{
    try
    {
#if ILMBASE_THREADING_ENABLED
        std::lock_guard<std::mutex> lock (*_streamData);
#endif
        if (_data->slices.size() == 0)
            throw IEX_NAMESPACE::ArgExc (
                "No frame buffer specified as pixel data destination.");

	int scanLineMin = min (scanLine1, scanLine2);
	int scanLineMax = max (scanLine1, scanLine2);

	if (scanLineMin < _data->minY || scanLineMax > _data->maxY)
	    throw IEX_NAMESPACE::ArgExc ("Tried to read scan line outside "
			       "the image file's data window.");

        //
        // We impose a numbering scheme on the lineBuffers where the first
        // scanline is contained in lineBuffer 1.
        //
        // Determine the first and last lineBuffer numbers in this scanline
        // range. We always attempt to read the scanlines in the order that
        // they are stored in the file.
        //

        int start, stop, dl;

        if (_data->lineOrder == INCREASING_Y)
        {
            start = (scanLineMin - _data->minY) / _data->linesInBuffer;
            stop  = (scanLineMax - _data->minY) / _data->linesInBuffer + 1;
            dl = 1;
        }
        else
        {
            start = (scanLineMax - _data->minY) / _data->linesInBuffer;
            stop  = (scanLineMin - _data->minY) / _data->linesInBuffer - 1;
            dl = -1;
        }

        //
        // Create a task group for all line buffer tasks.  When the
	// task group goes out of scope, the destructor waits until
	// all tasks are complete.
        //
        
        {
            TaskGroup taskGroup;
    
            //
            // Add the line buffer tasks.
            //
            // The tasks will execute in the order that they are created
            // because we lock the line buffers during construction and the
            // constructors are called by the main thread.  Hence, in order
	    // for a successive task to execute the previous task which
	    // used that line buffer must have completed already.
            //
    
            for (int l = start; l != stop; l += dl)
            {
                ThreadPool::addGlobalTask (newLineBufferTask (&taskGroup,
                                                              _streamData,
                                                              _data, l,
                                                              scanLineMin,
                                                              scanLineMax,
                                                              _data->optimizationMode));
            }
        
	    //
            // finish all tasks
	    //
        }
        
	//
	// Exeption handling:
	//
	// LineBufferTask::execute() may have encountered exceptions, but
	// those exceptions occurred in another thread, not in the thread
	// that is executing this call to ScanLineInputFile::readPixels().
	// LineBufferTask::execute() has caught all exceptions and stored
	// the exceptions' what() strings in the line buffers.
	// Now we check if any line buffer contains a stored exception; if
	// this is the case then we re-throw the exception in this thread.
	// (It is possible that multiple line buffers contain stored
	// exceptions.  We re-throw the first exception we find and
	// ignore all others.)
	//

	const string *exception = 0;

        for (size_t i = 0; i < _data->lineBuffers.size(); ++i)
	{
            LineBuffer *lineBuffer = _data->lineBuffers[i];

	    if (lineBuffer->hasException && !exception)
		exception = &lineBuffer->exception;

	    lineBuffer->hasException = false;
	}

	if (exception)
	    throw IEX_NAMESPACE::IoExc (*exception);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
	REPLACE_EXC (e, "Error reading pixel data from image "
                 "file \"" << fileName() << "\". " << e.what());
	throw;
    }
}
ScanLineInputFile::ScanLineInputFile(InputPartData* part)
{
    if (part->header.type() != SCANLINEIMAGE)
        throw IEX_NAMESPACE::ArgExc("Can't build a ScanLineInputFile from a type-mismatched part.");

    _data = new Data(part->numThreads);
    _streamData = part->mutex;
    _data->memoryMapped = _streamData->is->isMemoryMapped();

    _data->version = part->version;

    try
    {
       initialize(part->header);
    }
    catch(...)
    {
        if (!_data->memoryMapped)
        {
            for (size_t i = 0; i < _data->lineBuffers.size(); i++)
            {
                if( _data->lineBuffers[i] )
                {
                   EXRFreeAligned(_data->lineBuffers[i]->buffer);
                   _data->lineBuffers[i]->buffer=nullptr;
                }
            }
        }
        
        delete _data;
        throw;
    }
    _data->lineOffsets = part->chunkOffsets;

    _data->partNumber = part->partNumber;
    //
    // (TODO) change this code later.
    // The completeness of the file should be detected in MultiPartInputFile.
    //
    _data->fileIsComplete = true;
}
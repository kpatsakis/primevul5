ScanLineInputFile::~ScanLineInputFile ()
{
    if (!_data->memoryMapped)
    {
        for (size_t i = 0; i < _data->lineBuffers.size(); i++)
        {
            EXRFreeAligned(_data->lineBuffers[i]->buffer);
        }
    }
            

    //
    // ScanLineInputFile should never delete the stream,
    // because it does not own the stream.
    // We just delete the Mutex here.
    //
    if (_data->partNumber == -1)
        delete _streamData;

    delete _data;
}
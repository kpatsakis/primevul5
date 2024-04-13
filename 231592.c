ScanLineInputFile::frameBuffer () const
{
#if ILMBASE_THREADING_ENABLED
    std::lock_guard<std::mutex> lock (*_streamData);
#endif
    return _data->frameBuffer;
}
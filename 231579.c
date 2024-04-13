void ScanLineInputFile::rawPixelDataToBuffer(int scanLine,
                                             char *pixelData,
                                             int &pixelDataSize) const
{
  if (_data->memoryMapped) {
    throw IEX_NAMESPACE::ArgExc ("Reading raw pixel data to a buffer "
                                 "is not supported for memory mapped "
                                 "streams." );
  }

  try 
  {
#if ILMBASE_THREADING_ENABLED
    std::lock_guard<std::mutex> lock (*_streamData);
#endif
    if (scanLine < _data->minY || scanLine > _data->maxY) 
    {
      throw IEX_NAMESPACE::ArgExc ("Tried to read scan line outside "
                                   "the image file's data window.");
    }
    
    readPixelData
      (_streamData, _data, scanLine, pixelData, pixelDataSize);
    
  }
  catch (IEX_NAMESPACE::BaseExc &e) 
  {
    REPLACE_EXC (e, "Error reading pixel data from image "
                 "file \"" << fileName() << "\". " << e.what());
    throw;
  }
}
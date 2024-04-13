LineBufferTaskIIF::LineBufferTaskIIF
    (TaskGroup *group,
     ScanLineInputFile::Data *ifd,
     LineBuffer *lineBuffer,
     int scanLineMin,
     int scanLineMax,
     OptimizationMode optimizationMode
    )
    :
     Task (group),
     _ifd (ifd),
     _lineBuffer (lineBuffer),
     _scanLineMin (scanLineMin),
     _scanLineMax (scanLineMax),
     _optimizationMode (optimizationMode)
{
     /*
     //
     // indicates the optimised path has been taken
     //
     static bool could_optimise=false;
     if(could_optimise==false)
     {
         std::cerr << " optimised path\n";
         could_optimise=true;
     }
     */
}
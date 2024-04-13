bool ScanLineInputFile::isOptimizationEnabled() const
{
    if (_data->slices.size() == 0)
        throw IEX_NAMESPACE::ArgExc ("No frame buffer specified "
        "as pixel data destination.");
    
    return _data->optimizationMode._optimizable;
}
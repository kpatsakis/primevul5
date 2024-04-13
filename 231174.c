    Classifier (std::string suffix,
                CompressorScheme scheme,
                PixelType type,
                int cscIdx,
                bool caseInsensitive):
        _suffix(suffix),
        _scheme(scheme),
        _type(type),
        _cscIdx(cscIdx),
        _caseInsensitive(caseInsensitive)
    {
        if (caseInsensitive) 
            std::transform(_suffix.begin(), _suffix.end(), _suffix.begin(), tolower);
    }
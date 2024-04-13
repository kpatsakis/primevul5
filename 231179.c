    bool match (const std::string &suffix, const PixelType type) const
    {
        if (_type != type) return false;

        if (_caseInsensitive) 
        {
            std::string tmp(suffix);
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), tolower);
            return tmp == _suffix;
        }

        return suffix == _suffix;
    }
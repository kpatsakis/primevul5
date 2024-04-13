    Metadata(const String *str, uint repertoire_arg)
    {
      MY_STRING_METADATA::repertoire= repertoire_arg;
      MY_STRING_METADATA::char_length= str->numchars();
    }
    Metadata(const String *str)
    {
      my_string_metadata_get(this, str->charset(), str->ptr(), str->length());
    }
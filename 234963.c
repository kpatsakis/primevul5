    bool convert_if_needed(THD *thd, String *str)
    {
      /*
        Check is so simple because all charsets were set up properly
        in setup_one_conversion_function, where typecode of
        placeholder was also taken into account: the variables are different
        here only if conversion is really necessary.
      */
      if (needs_conversion())
        return convert(thd, str);
      str->set_charset(final_character_set_of_str_value);
      return false;
    }
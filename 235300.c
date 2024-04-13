  void fix_charset_and_length_from_str_value(Derivation dv, Metadata metadata)
  {
    /*
      We have to have a different max_length than 'length' here to
      ensure that we get the right length if we do use the item
      to create a new table. In this case max_length must be the maximum
      number of chars for a string of this type because we in Create_field::
      divide the max_length with mbmaxlen).
    */
    collation.set(str_value.charset(), dv, metadata.repertoire());
    fix_char_length(metadata.char_length());
    decimals= NOT_FIXED_DEC;
  }
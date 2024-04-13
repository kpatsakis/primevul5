  bool needs_charset_converter(uint32 length, CHARSET_INFO *tocs) const
  {
    /*
      This will return "true" if conversion happens:
      - between two non-binary different character sets
      - from "binary" to "unsafe" character set
        (those that can have non-well-formed string)
      - from "binary" to UCS2-alike character set with mbminlen>1,
        when prefix left-padding is needed for an incomplete character:
        binary 0xFF -> ucs2 0x00FF)
    */
    if (!String::needs_conversion_on_storage(length,
                                             collation.collation, tocs))
      return false;
    /*
      No needs to add converter if an "arg" is NUMERIC or DATETIME
      value (which is pure ASCII) and at the same time target DTCollation
      is ASCII-compatible. For example, no needs to rewrite:
        SELECT * FROM t1 WHERE datetime_field = '2010-01-01';
      to
        SELECT * FROM t1 WHERE CONVERT(datetime_field USING cs) = '2010-01-01';

      TODO: avoid conversion of any values with
      repertoire ASCII and 7bit-ASCII-compatible,
      not only numeric/datetime origin.
    */
    if (collation.derivation == DERIVATION_NUMERIC &&
        collation.repertoire == MY_REPERTOIRE_ASCII &&
        !(collation.collation->state & MY_CS_NONASCII) &&
        !(tocs->state & MY_CS_NONASCII))
      return false;
    return true;
  }
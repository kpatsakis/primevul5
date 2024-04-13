  Item *safe_charset_converter(THD *thd, CHARSET_INFO *tocs)
  {
    /**
      Item_cache_str::safe_charset_converter() returns a new Item_cache
      with Item_func_conv_charset installed on "example". The original
      Item_cache is not referenced (neither directly nor recursively)
      from the result of Item_cache_str::safe_charset_converter().

      For NULLIF() purposes we need a different behavior:
      we need a new instance of Item_func_conv_charset,
      with the original Item_cache referenced in args[0]. See MDEV-9181.
    */
    return Item::safe_charset_converter(thd, tocs);
  }
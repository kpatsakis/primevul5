  Load_data_outvar *get_load_data_outvar_or_error()
  {
    Load_data_outvar *dst= get_load_data_outvar();
    if (dst)
      return dst;
    my_error(ER_NONUPDATEABLE_COLUMN, MYF(0), name);
    return NULL;
  }
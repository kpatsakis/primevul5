int rr_sequential_and_unpack(READ_RECORD *info)
{
  int error;
  if ((error= rr_sequential(info)))
    return error;
  
  for (Copy_field *cp= info->copy_field; cp != info->copy_field_end; cp++)
    (*cp->do_copy)(cp);

  return error;
}
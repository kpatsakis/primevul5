unpack_fields(const MYSQL *mysql,
              MYSQL_DATA *data, MA_MEM_ROOT *alloc, uint fields,
	      my_bool default_value)
{
  MYSQL_ROWS	*row;
  MYSQL_FIELD	*field,*result;
  char    *p;
  unsigned int i, field_count= sizeof(rset_field_offsets)/sizeof(size_t)/2;

  field=result=(MYSQL_FIELD*) ma_alloc_root(alloc,sizeof(MYSQL_FIELD)*fields);
  if (!result)
    return(0);

  for (row=data->data; row ; row = row->next,field++)
  {
    if (field >= result + fields)
      goto error;

    for (i=0; i < field_count; i++)
    {
      uint length= (uint)(row->data[i+1] - row->data[i] - 1);
      if (!row->data[i] && row->data[i][length])
        goto error;

      *(char **)(((char *)field) + rset_field_offsets[i*2])=
        ma_strdup_root(alloc, (char *)row->data[i]);
      *(unsigned int *)(((char *)field) + rset_field_offsets[i*2+1])= length;
    }

    field->extension= NULL;
    if (ma_has_extended_type_info(mysql))
    {
      if (row->data[i+1] - row->data[i] > 1)
      {
        size_t len= row->data[i+1] - row->data[i] - 1;
        MA_FIELD_EXTENSION *ext= new_ma_field_extension(alloc);
        if ((field->extension= ext))
          ma_field_extension_init_type_info(alloc, ext, row->data[i], len);
      }
      i++;
    }

    p= (char *)row->data[i];
    /* filler */
    field->charsetnr= uint2korr(p);
    p+= 2;
    field->length= (uint) uint4korr(p);
    p+= 4;
    field->type=   (enum enum_field_types)uint1korr(p);
    p++;
    field->flags= uint2korr(p);
    p+= 2;
    field->decimals= (uint) p[0];
    p++;

    /* filler */
    p+= 2;

    if (INTERNAL_NUM_FIELD(field))
      field->flags|= NUM_FLAG;

    i++;
    /* This is used by deprecated function mysql_list_fields only,
       however the reported length is not correct, so we always zero it */
    if (default_value && row->data[i])
      field->def=ma_strdup_root(alloc,(char*) row->data[i]);
    else
      field->def=0;
    field->def_length= 0;

    field->max_length= 0;
  }
  if (field < result + fields)
    goto error;
  free_rows(data);				/* Free old data */
  return(result);
error:
  free_rows(data);
  ma_free_root(alloc, MYF(0));
  return(0);
}
mariadb_field_attr(MARIADB_CONST_STRING *attr,
                   const MYSQL_FIELD *field,
                   enum mariadb_field_attr_t type)
{
  MA_FIELD_EXTENSION *ext= (MA_FIELD_EXTENSION*) field->extension;
  if (!ext || type > MARIADB_FIELD_ATTR_LAST)
  {
    *attr= null_const_string;
    return 1;
  }
  *attr= ext->metadata[type];
  return 0;
}
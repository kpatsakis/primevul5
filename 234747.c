void Virtual_tmp_table::setup_field_pointers()
{
  uchar *null_pos= record[0];
  uchar *field_pos= null_pos + s->null_bytes;
  uint null_bit= 1;

  for (Field **cur_ptr= field; *cur_ptr; ++cur_ptr)
  {
    Field *cur_field= *cur_ptr;
    if ((cur_field->flags & NOT_NULL_FLAG))
      cur_field->move_field(field_pos);
    else
    {
      cur_field->move_field(field_pos, (uchar*) null_pos, null_bit);
      null_bit<<= 1;
      if (null_bit == (uint)1 << 8)
      {
        ++null_pos;
        null_bit= 1;
      }
    }
    if (cur_field->type() == MYSQL_TYPE_BIT &&
        cur_field->key_type() == HA_KEYTYPE_BIT)
    {
      /* This is a Field_bit since key_type is HA_KEYTYPE_BIT */
      static_cast<Field_bit*>(cur_field)->set_bit_ptr(null_pos, null_bit);
      null_bit+= cur_field->field_length & 7;
      if (null_bit > 7)
      {
        null_pos++;
        null_bit-= 8;
      }
    }
    cur_field->reset();
    field_pos+= cur_field->pack_length();
  }
}
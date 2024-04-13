handler *ha_maria::clone(const char *name __attribute__((unused)),
                         MEM_ROOT *mem_root)
{
  ha_maria *new_handler=
    static_cast <ha_maria *>(handler::clone(file->s->open_file_name.str,
                                            mem_root));
  if (new_handler)
  {
    new_handler->file->state= file->state;
    /* maria_create_trn_for_mysql() is never called for clone() tables */
    new_handler->file->trn= file->trn;
    DBUG_ASSERT(new_handler->file->trn_prev == 0 &&
                new_handler->file->trn_next == 0);
  }
  return new_handler;
}
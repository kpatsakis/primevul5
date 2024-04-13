  DbugStringItemTypeValue(THD *thd, const Item *item)
  {
    append('(');
    append(item->type_handler()->name().ptr());
    append(')');
    const_cast<Item*>(item)->print(this, QT_EXPLAIN);
    /* Append end \0 to allow usage of c_ptr() */
    append('\0');
    str_length--;
  }
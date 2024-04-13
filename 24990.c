  void set_param_func(uchar **pos, ulong len)
  {
    /*
      To avoid Item_param::set_xxx() asserting on data type mismatch,
      we set the value type handler here:
      - It can not be initialized yet after Item_param::setup_conversion().
      - Also, for LIMIT clause parameters, the value type handler might have
        changed from the real type handler to type_handler_longlong.
        So here we'll restore it.
    */
    const Type_handler *h= Item_param::type_handler();
    value.set_handler(h);
    h->Item_param_set_param_func(this, pos, len);
  }
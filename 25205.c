  virtual bool send(Protocol *protocol, st_value *buffer)
  {
    return type_handler()->Item_send(this, protocol, buffer);
  }
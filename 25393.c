  bool send(Protocol *protocol, st_value *buffer)
  {
    return value_item->send(protocol, buffer);
  }
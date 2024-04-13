inline bool Item_sp_variable::send(Protocol *protocol, st_value *buffer)
{
  return this_item()->send(protocol, buffer);
}
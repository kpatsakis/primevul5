inline void Item_sp_variable::make_field(THD *thd, Send_field *field)
{
  Item *it= this_item();

  if (name)
    it->set_name(thd, name, (uint) strlen(name), system_charset_info);
  else
    it->set_name(thd, m_name.str, (uint) m_name.length, system_charset_info);
  it->make_field(thd, field);
}
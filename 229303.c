char *ma_send_connect_attr(MYSQL *mysql, unsigned char *buffer)
{
  if (mysql->server_capabilities & CLIENT_CONNECT_ATTRS)
  {
    buffer= (unsigned char *)mysql_net_store_length((unsigned char *)buffer, (mysql->options.extension) ?
                             mysql->options.extension->connect_attrs_len : 0);
    if (mysql->options.extension &&
        hash_inited(&mysql->options.extension->connect_attrs))
    {
      uint i;
      for (i=0; i < mysql->options.extension->connect_attrs.records; i++)
      {
        size_t len;
        uchar *p= hash_element(&mysql->options.extension->connect_attrs, i);

        len= strlen((char *)p);
        buffer= mysql_net_store_length(buffer, len);
        memcpy(buffer, p, len);
        buffer+= (len);
        p+= (len + 1);
        len= strlen((char *)p);
        buffer= mysql_net_store_length(buffer, len);
        memcpy(buffer, p, len);
        buffer+= len;
      }
    }
  }
  return (char *)buffer;
}
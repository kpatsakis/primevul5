dnp3_append_2item_text(proto_item *item1, proto_item *item2, const gchar *text)
{
  proto_item_append_text(item1, "%s", text);
  proto_item_append_text(item2, "%s", text);
}
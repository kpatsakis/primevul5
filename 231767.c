static void get_contype_from_attrs(map<string, bufferlist>& attrs,
				   string& content_type)
{
  map<string, bufferlist>::iterator iter = attrs.find(RGW_ATTR_CONTENT_TYPE);
  if (iter != attrs.end()) {
    content_type = rgw_bl_str(iter->second);
  }
}
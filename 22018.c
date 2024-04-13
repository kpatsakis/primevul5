PJ_DEF(const char*) pj_stun_get_attr_name(unsigned attr_type)
{
    const struct attr_desc *attr_desc;

    attr_desc = find_attr_desc(attr_type);
    if (!attr_desc || attr_desc->name==NULL)
	return "???";

    return attr_desc->name;
}
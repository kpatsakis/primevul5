static void freeUserdata(userdata_t * ud)
{
    dtclose(ud->nameMap);
    agxbfree(&(ud->xml_attr_name));
    agxbfree(&(ud->xml_attr_value));
    agxbfree(&(ud->composite_buffer));
    freeString(ud->elements);
    free(ud);
}
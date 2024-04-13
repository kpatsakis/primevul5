static userdata_t *genUserdata(char* dfltname)
{
    userdata_t *user = NEW(userdata_t);
    agxbinit(&(user->xml_attr_name), NAMEBUF, 0);
    agxbinit(&(user->xml_attr_value), SMALLBUF, 0);
    agxbinit(&(user->composite_buffer), SMALLBUF, 0);
    user->listen = FALSE;
    user->elements = 0;
    user->closedElementType = TAG_NONE;
    user->globalAttrType = TAG_NONE;
    user->compositeReadState = FALSE;
    user->edgeinverted = FALSE;
    user->gname = dfltname;
    user->nameMap = dtopen(&nameDisc, Dtoset);
    return user;
}
static int get_xml_attr(char *attrname, const char **atts)
{
    int count = 0;
    while (atts[count] != NULL) {
	if (strcmp(attrname, atts[count]) == 0) {
	    return count + 1;
	}
	count += 2;
    }
    return -1;
}
xsltNewKeyTable(const xmlChar *name, const xmlChar *nameURI) {
    xsltKeyTablePtr cur;

    cur = (xsltKeyTablePtr) xmlMalloc(sizeof(xsltKeyTable));
    if (cur == NULL) {
	xsltTransformError(NULL, NULL, NULL,
		"xsltNewKeyTable : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xsltKeyTable));
    if (name != NULL)
	cur->name = xmlStrdup(name);
    if (nameURI != NULL)
	cur->nameURI = xmlStrdup(nameURI);
    cur->keys = xmlHashCreate(0);
    return(cur);
}
xsltFreeKeyTable(xsltKeyTablePtr keyt) {
    if (keyt == NULL)
	return;
    if (keyt->name != NULL)
	xmlFree(keyt->name);
    if (keyt->nameURI != NULL)
	xmlFree(keyt->nameURI);
    if (keyt->keys != NULL)
	xmlHashFree(keyt->keys,
		    (xmlHashDeallocator) xmlXPathFreeNodeSet);
    memset(keyt, -1, sizeof(xsltKeyTable));
    xmlFree(keyt);
}
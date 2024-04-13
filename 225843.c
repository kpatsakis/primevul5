xmlXPathNodeSetAddNs(xmlNodeSetPtr cur, xmlNodePtr node, xmlNsPtr ns) {
    int i;


    if ((cur == NULL) || (ns == NULL) || (node == NULL) ||
        (ns->type != XML_NAMESPACE_DECL) ||
	(node->type != XML_ELEMENT_NODE))
	return(-1);

    /* @@ with_ns to check whether namespace nodes should be looked at @@ */
    /*
     * prevent duplicates
     */
    for (i = 0;i < cur->nodeNr;i++) {
        if ((cur->nodeTab[i] != NULL) &&
	    (cur->nodeTab[i]->type == XML_NAMESPACE_DECL) &&
	    (((xmlNsPtr)cur->nodeTab[i])->next == (xmlNsPtr) node) &&
	    (xmlStrEqual(ns->prefix, ((xmlNsPtr)cur->nodeTab[i])->prefix)))
	    return(0);
    }

    /*
     * grow the nodeTab if needed
     */
    if (cur->nodeMax == 0) {
        cur->nodeTab = (xmlNodePtr *) xmlMalloc(XML_NODESET_DEFAULT *
					     sizeof(xmlNodePtr));
	if (cur->nodeTab == NULL) {
	    xmlXPathErrMemory(NULL, "growing nodeset\n");
	    return(-1);
	}
	memset(cur->nodeTab, 0 ,
	       XML_NODESET_DEFAULT * (size_t) sizeof(xmlNodePtr));
        cur->nodeMax = XML_NODESET_DEFAULT;
    } else if (cur->nodeNr == cur->nodeMax) {
        xmlNodePtr *temp;

        if (cur->nodeMax >= XPATH_MAX_NODESET_LENGTH) {
            xmlXPathErrMemory(NULL, "growing nodeset hit limit\n");
            return(-1);
        }
	temp = (xmlNodePtr *) xmlRealloc(cur->nodeTab, cur->nodeMax * 2 *
				      sizeof(xmlNodePtr));
	if (temp == NULL) {
	    xmlXPathErrMemory(NULL, "growing nodeset\n");
	    return(-1);
	}
        cur->nodeMax *= 2;
	cur->nodeTab = temp;
    }
    cur->nodeTab[cur->nodeNr++] = xmlXPathNodeSetDupNs(node, ns);
    return(0);
}
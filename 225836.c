xmlXPathNodeSetKeepLast(xmlNodeSetPtr set)
{
    int i;
    xmlNodePtr node;

    if ((set == NULL) || (set->nodeNr <= 1))
	return;
    for (i = 0; i < set->nodeNr - 1; i++) {
        node = set->nodeTab[i];
        if ((node != NULL) &&
            (node->type == XML_NAMESPACE_DECL))
            xmlXPathNodeSetFreeNs((xmlNsPtr) node);
    }
    set->nodeTab[0] = set->nodeTab[set->nodeNr-1];
    set->nodeNr = 1;
}
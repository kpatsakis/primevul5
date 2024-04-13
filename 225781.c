xmlXPathNodeSetClearFromPos(xmlNodeSetPtr set, int pos, int hasNsNodes)
{
    if ((set == NULL) || (pos >= set->nodeNr))
	return;
    else if ((hasNsNodes)) {
	int i;
	xmlNodePtr node;

	for (i = pos; i < set->nodeNr; i++) {
	    node = set->nodeTab[i];
	    if ((node != NULL) &&
		(node->type == XML_NAMESPACE_DECL))
		xmlXPathNodeSetFreeNs((xmlNsPtr) node);
	}
    }
    set->nodeNr = pos;
}
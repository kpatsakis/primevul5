 */
static xmlNodePtr
xmlXPathNextDescendantOrSelfElemParent(xmlNodePtr cur,
				       xmlNodePtr contextNode)
{
    if (cur == NULL) {
	if (contextNode == NULL)
	    return(NULL);
	switch (contextNode->type) {
	    case XML_ELEMENT_NODE:
	    case XML_XINCLUDE_START:
	    case XML_DOCUMENT_FRAG_NODE:
	    case XML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	    case XML_DOCB_DOCUMENT_NODE:
#endif
	    case XML_HTML_DOCUMENT_NODE:
		return(contextNode);
	    default:
		return(NULL);
	}
	return(NULL);
    } else {
	xmlNodePtr start = cur;

	while (cur != NULL) {
	    switch (cur->type) {
		case XML_ELEMENT_NODE:
		/* TODO: OK to have XInclude here? */
		case XML_XINCLUDE_START:
		case XML_DOCUMENT_FRAG_NODE:
		    if (cur != start)
			return(cur);
		    if (cur->children != NULL) {
			cur = cur->children;
			continue;
		    }
		    break;
		/* Not sure if we need those here. */
		case XML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
		case XML_DOCB_DOCUMENT_NODE:
#endif
		case XML_HTML_DOCUMENT_NODE:
		    if (cur != start)
			return(cur);
		    return(xmlDocGetRootElement((xmlDocPtr) cur));
		default:
		    break;
	    }

next_sibling:
	    if ((cur == NULL) || (cur == contextNode))
		return(NULL);
	    if (cur->next != NULL) {
		cur = cur->next;
	    } else {
		cur = cur->parent;
		goto next_sibling;
	    }
	}
    }
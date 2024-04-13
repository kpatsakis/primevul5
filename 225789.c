 */
xmlNodePtr
xmlXPathNextNamespace(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (ctxt->context->node->type != XML_ELEMENT_NODE) return(NULL);
    if (cur == NULL) {
        if (ctxt->context->tmpNsList != NULL)
	    xmlFree(ctxt->context->tmpNsList);
	ctxt->context->tmpNsList =
	    xmlGetNsList(ctxt->context->doc, ctxt->context->node);
	ctxt->context->tmpNsNr = 0;
	if (ctxt->context->tmpNsList != NULL) {
	    while (ctxt->context->tmpNsList[ctxt->context->tmpNsNr] != NULL) {
		ctxt->context->tmpNsNr++;
	    }
	}
	return((xmlNodePtr) xmlXPathXMLNamespace);
    }
    if (ctxt->context->tmpNsNr > 0) {
	return (xmlNodePtr)ctxt->context->tmpNsList[--ctxt->context->tmpNsNr];
    } else {
	if (ctxt->context->tmpNsList != NULL)
	    xmlFree(ctxt->context->tmpNsList);
	ctxt->context->tmpNsList = NULL;
	return(NULL);
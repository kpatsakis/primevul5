 */
void
xmlXPathCountFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;

    CHECK_ARITY(1);
    if ((ctxt->value == NULL) ||
	((ctxt->value->type != XPATH_NODESET) &&
	 (ctxt->value->type != XPATH_XSLT_TREE)))
	XP_ERROR(XPATH_INVALID_TYPE);
    cur = valuePop(ctxt);

    if ((cur == NULL) || (cur->nodesetval == NULL))
	valuePush(ctxt, xmlXPathCacheNewFloat(ctxt->context, (double) 0));
    else if ((cur->type == XPATH_NODESET) || (cur->type == XPATH_XSLT_TREE)) {
	valuePush(ctxt, xmlXPathCacheNewFloat(ctxt->context,
	    (double) cur->nodesetval->nodeNr));
    } else {
	if ((cur->nodesetval->nodeNr != 1) ||
	    (cur->nodesetval->nodeTab == NULL)) {
	    valuePush(ctxt, xmlXPathCacheNewFloat(ctxt->context, (double) 0));
	} else {
	    xmlNodePtr tmp;
	    int i = 0;

	    tmp = cur->nodesetval->nodeTab[0];
	    if ((tmp != NULL) && (tmp->type != XML_NAMESPACE_DECL)) {
		tmp = tmp->children;
		while (tmp != NULL) {
		    tmp = tmp->next;
		    i++;
		}
	    }
	    valuePush(ctxt, xmlXPathCacheNewFloat(ctxt->context, (double) i));
	}
    }
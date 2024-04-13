 */
static void
xmlXPathCompFunctionCall(xmlXPathParserContextPtr ctxt) {
    xmlChar *name;
    xmlChar *prefix;
    int nbargs = 0;
    int sort = 1;

    name = xmlXPathParseQName(ctxt, &prefix);
    if (name == NULL) {
	xmlFree(prefix);
	XP_ERROR(XPATH_EXPR_ERROR);
    }
    SKIP_BLANKS;
#ifdef DEBUG_EXPR
    if (prefix == NULL)
	xmlGenericError(xmlGenericErrorContext, "Calling function %s\n",
			name);
    else
	xmlGenericError(xmlGenericErrorContext, "Calling function %s:%s\n",
			prefix, name);
#endif

    if (CUR != '(') {
	xmlFree(name);
	xmlFree(prefix);
	XP_ERROR(XPATH_EXPR_ERROR);
    }
    NEXT;
    SKIP_BLANKS;

    /*
    * Optimization for count(): we don't need the node-set to be sorted.
    */
    if ((prefix == NULL) && (name[0] == 'c') &&
	xmlStrEqual(name, BAD_CAST "count"))
    {
	sort = 0;
    }
    ctxt->comp->last = -1;
    if (CUR != ')') {
	while (CUR != 0) {
	    int op1 = ctxt->comp->last;
	    ctxt->comp->last = -1;
	    xmlXPathCompileExpr(ctxt, sort);
	    if (ctxt->error != XPATH_EXPRESSION_OK) {
		xmlFree(name);
		xmlFree(prefix);
		return;
	    }
	    PUSH_BINARY_EXPR(XPATH_OP_ARG, op1, ctxt->comp->last, 0, 0);
	    nbargs++;
	    if (CUR == ')') break;
	    if (CUR != ',') {
		xmlFree(name);
		xmlFree(prefix);
		XP_ERROR(XPATH_EXPR_ERROR);
	    }
	    NEXT;
	    SKIP_BLANKS;
	}
    }
    PUSH_LONG_EXPR(XPATH_OP_FUNCTION, nbargs, 0, 0,
	           name, prefix);
    NEXT;
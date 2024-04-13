 */
xmlXPathCompExprPtr
xmlXPathCtxtCompile(xmlXPathContextPtr ctxt, const xmlChar *str) {
    xmlXPathParserContextPtr pctxt;
    xmlXPathCompExprPtr comp;

#ifdef XPATH_STREAMING
    comp = xmlXPathTryStreamCompile(ctxt, str);
    if (comp != NULL)
        return(comp);
#endif

    xmlXPathInit();

    pctxt = xmlXPathNewParserContext(str, ctxt);
    if (pctxt == NULL)
        return NULL;
    xmlXPathCompileExpr(pctxt, 1);

    if( pctxt->error != XPATH_EXPRESSION_OK )
    {
        xmlXPathFreeParserContext(pctxt);
        return(NULL);
    }

    if (*pctxt->cur != 0) {
	/*
	 * aleksey: in some cases this line prints *second* error message
	 * (see bug #78858) and probably this should be fixed.
	 * However, we are not sure that all error messages are printed
	 * out in other places. It's not critical so we leave it as-is for now
	 */
	xmlXPatherror(pctxt, __FILE__, __LINE__, XPATH_EXPR_ERROR);
	comp = NULL;
    } else {
	comp = pctxt->comp;
	pctxt->comp = NULL;
    }
    xmlXPathFreeParserContext(pctxt);

    if (comp != NULL) {
	comp->expr = xmlStrdup(str);
#ifdef DEBUG_EVAL_COUNTS
	comp->string = xmlStrdup(str);
	comp->nb = 0;
#endif
	if ((comp->nbStep > 1) && (comp->last >= 0)) {
	    xmlXPathOptimizeExpression(comp, &comp->steps[comp->last]);
	}
    }
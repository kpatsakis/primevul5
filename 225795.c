 */
void
xmlXPathEvalExpr(xmlXPathParserContextPtr ctxt) {
#ifdef XPATH_STREAMING
    xmlXPathCompExprPtr comp;
#endif

    if (ctxt == NULL) return;

#ifdef XPATH_STREAMING
    comp = xmlXPathTryStreamCompile(ctxt->context, ctxt->base);
    if (comp != NULL) {
        if (ctxt->comp != NULL)
	    xmlXPathFreeCompExpr(ctxt->comp);
        ctxt->comp = comp;
    } else
#endif
    {
	xmlXPathCompileExpr(ctxt, 1);
        CHECK_ERROR;

        /* Check for trailing characters. */
        if (*ctxt->cur != 0)
            XP_ERROR(XPATH_EXPR_ERROR);

	if ((ctxt->comp->nbStep > 1) && (ctxt->comp->last >= 0))
	    xmlXPathOptimizeExpression(ctxt->comp,
		&ctxt->comp->steps[ctxt->comp->last]);
    }

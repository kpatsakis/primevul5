 */
static int
xmlXPathCompiledEvalInternal(xmlXPathCompExprPtr comp,
			     xmlXPathContextPtr ctxt,
			     xmlXPathObjectPtr *resObjPtr,
			     int toBool)
{
    xmlXPathParserContextPtr pctxt;
    xmlXPathObjectPtr resObj;
#ifndef LIBXML_THREAD_ENABLED
    static int reentance = 0;
#endif
    int res;

    CHECK_CTXT_NEG(ctxt)

    if (comp == NULL)
	return(-1);
    xmlXPathInit();

#ifndef LIBXML_THREAD_ENABLED
    reentance++;
    if (reentance > 1)
	xmlXPathDisableOptimizer = 1;
#endif

#ifdef DEBUG_EVAL_COUNTS
    comp->nb++;
    if ((comp->string != NULL) && (comp->nb > 100)) {
	fprintf(stderr, "100 x %s\n", comp->string);
	comp->nb = 0;
    }
#endif
    pctxt = xmlXPathCompParserContext(comp, ctxt);
    res = xmlXPathRunEval(pctxt, toBool);

    if (pctxt->error != XPATH_EXPRESSION_OK) {
        resObj = NULL;
    } else {
        resObj = valuePop(pctxt);
        if (resObj == NULL) {
            if (!toBool)
                xmlGenericError(xmlGenericErrorContext,
                    "xmlXPathCompiledEval: No result on the stack.\n");
        } else if (pctxt->valueNr > 0) {
            xmlGenericError(xmlGenericErrorContext,
                "xmlXPathCompiledEval: %d object(s) left on the stack.\n",
                pctxt->valueNr);
        }
    }

    if (resObjPtr)
        *resObjPtr = resObj;
    else
        xmlXPathReleaseObject(ctxt, resObj);

    pctxt->comp = NULL;
    xmlXPathFreeParserContext(pctxt);
#ifndef LIBXML_THREAD_ENABLED
    reentance--;
#endif

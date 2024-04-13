 */
xmlXPathObjectPtr
xmlXPathEval(const xmlChar *str, xmlXPathContextPtr ctx) {
    xmlXPathParserContextPtr ctxt;
    xmlXPathObjectPtr res;

    CHECK_CTXT(ctx)

    xmlXPathInit();

    ctxt = xmlXPathNewParserContext(str, ctx);
    if (ctxt == NULL)
        return NULL;
    xmlXPathEvalExpr(ctxt);

    if (ctxt->error != XPATH_EXPRESSION_OK) {
	res = NULL;
    } else {
	res = valuePop(ctxt);
        if (res == NULL) {
            xmlGenericError(xmlGenericErrorContext,
                "xmlXPathCompiledEval: No result on the stack.\n");
        } else if (ctxt->valueNr > 0) {
            xmlGenericError(xmlGenericErrorContext,
                "xmlXPathCompiledEval: %d object(s) left on the stack.\n",
                ctxt->valueNr);
        }
    }

    xmlXPathFreeParserContext(ctxt);
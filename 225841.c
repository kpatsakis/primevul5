 */
void
xmlXPathFreeParserContext(xmlXPathParserContextPtr ctxt) {
    int i;

    if (ctxt->valueTab != NULL) {
        for (i = 0; i < ctxt->valueNr; i++) {
            if (ctxt->context)
                xmlXPathReleaseObject(ctxt->context, ctxt->valueTab[i]);
            else
                xmlXPathFreeObject(ctxt->valueTab[i]);
        }
        xmlFree(ctxt->valueTab);
    }
    if (ctxt->comp != NULL) {
#ifdef XPATH_STREAMING
	if (ctxt->comp->stream != NULL) {
	    xmlFreePatternList(ctxt->comp->stream);
	    ctxt->comp->stream = NULL;
	}
#endif
	xmlXPathFreeCompExpr(ctxt->comp);
    }
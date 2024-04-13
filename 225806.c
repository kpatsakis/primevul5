valuePush(xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr value)
{
    if ((ctxt == NULL) || (value == NULL)) return(-1);
    if (ctxt->valueNr >= ctxt->valueMax) {
        xmlXPathObjectPtr *tmp;

        if (ctxt->valueMax >= XPATH_MAX_STACK_DEPTH) {
            xmlXPathErrMemory(NULL, "XPath stack depth limit reached\n");
            ctxt->error = XPATH_MEMORY_ERROR;
            return (0);
        }
        tmp = (xmlXPathObjectPtr *) xmlRealloc(ctxt->valueTab,
                                             2 * ctxt->valueMax *
                                             sizeof(ctxt->valueTab[0]));
        if (tmp == NULL) {
            xmlXPathErrMemory(NULL, "pushing value\n");
            ctxt->error = XPATH_MEMORY_ERROR;
            return (0);
        }
        ctxt->valueMax *= 2;
	ctxt->valueTab = tmp;
    }
    ctxt->valueTab[ctxt->valueNr] = value;
    ctxt->value = value;
    return (ctxt->valueNr++);
}

xmlChar *
xmlXPathParseName(xmlXPathParserContextPtr ctxt) {
    const xmlChar *in;
    xmlChar *ret;
    size_t count = 0;

    if ((ctxt == NULL) || (ctxt->cur == NULL)) return(NULL);
    /*
     * Accelerator for simple ASCII names
     */
    in = ctxt->cur;
    if (((*in >= 0x61) && (*in <= 0x7A)) ||
	((*in >= 0x41) && (*in <= 0x5A)) ||
	(*in == '_') || (*in == ':')) {
	in++;
	while (((*in >= 0x61) && (*in <= 0x7A)) ||
	       ((*in >= 0x41) && (*in <= 0x5A)) ||
	       ((*in >= 0x30) && (*in <= 0x39)) ||
	       (*in == '_') || (*in == '-') ||
	       (*in == ':') || (*in == '.'))
	    in++;
	if ((*in > 0) && (*in < 0x80)) {
	    count = in - ctxt->cur;
            if (count > XML_MAX_NAME_LENGTH) {
                ctxt->cur = in;
                XP_ERRORNULL(XPATH_EXPR_ERROR);
            }
	    ret = xmlStrndup(ctxt->cur, count);
	    ctxt->cur = in;
	    return(ret);
	}
    }
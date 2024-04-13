 */
void
xmlXPathNormalizeFunction(xmlXPathParserContextPtr ctxt, int nargs) {
  xmlXPathObjectPtr obj = NULL;
  xmlChar *source = NULL;
  xmlBufPtr target;
  xmlChar blank;

  if (ctxt == NULL) return;
  if (nargs == 0) {
    /* Use current context node */
      valuePush(ctxt,
	  xmlXPathCacheWrapString(ctxt->context,
	    xmlXPathCastNodeToString(ctxt->context->node)));
    nargs = 1;
  }

  CHECK_ARITY(1);
  CAST_TO_STRING;
  CHECK_TYPE(XPATH_STRING);
  obj = valuePop(ctxt);
  source = obj->stringval;

  target = xmlBufCreate();
  if (target && source) {

    /* Skip leading whitespaces */
    while (IS_BLANK_CH(*source))
      source++;

    /* Collapse intermediate whitespaces, and skip trailing whitespaces */
    blank = 0;
    while (*source) {
      if (IS_BLANK_CH(*source)) {
	blank = 0x20;
      } else {
	if (blank) {
	  xmlBufAdd(target, &blank, 1);
	  blank = 0;
	}
	xmlBufAdd(target, source, 1);
      }
      source++;
    }
    valuePush(ctxt, xmlXPathCacheNewString(ctxt->context,
	xmlBufContent(target)));
    xmlBufFree(target);
  }
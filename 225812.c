 */
void
xmlXPathSubstringAfterFunction(xmlXPathParserContextPtr ctxt, int nargs) {
  xmlXPathObjectPtr str;
  xmlXPathObjectPtr find;
  xmlBufPtr target;
  const xmlChar *point;
  int offset;

  CHECK_ARITY(2);
  CAST_TO_STRING;
  find = valuePop(ctxt);
  CAST_TO_STRING;
  str = valuePop(ctxt);

  target = xmlBufCreate();
  if (target) {
    point = xmlStrstr(str->stringval, find->stringval);
    if (point) {
      offset = (int)(point - str->stringval) + xmlStrlen(find->stringval);
      xmlBufAdd(target, &str->stringval[offset],
		   xmlStrlen(str->stringval) - offset);
    }
    valuePush(ctxt, xmlXPathCacheNewString(ctxt->context,
	xmlBufContent(target)));
    xmlBufFree(target);
  }
  xmlXPathReleaseObject(ctxt->context, str);
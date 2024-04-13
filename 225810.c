 */
static void
xmlXPathEscapeUriFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr str;
    int escape_reserved;
    xmlBufPtr target;
    xmlChar *cptr;
    xmlChar escape[4];

    CHECK_ARITY(2);

    escape_reserved = xmlXPathPopBoolean(ctxt);

    CAST_TO_STRING;
    str = valuePop(ctxt);

    target = xmlBufCreate();

    escape[0] = '%';
    escape[3] = 0;

    if (target) {
	for (cptr = str->stringval; *cptr; cptr++) {
	    if ((*cptr >= 'A' && *cptr <= 'Z') ||
		(*cptr >= 'a' && *cptr <= 'z') ||
		(*cptr >= '0' && *cptr <= '9') ||
		*cptr == '-' || *cptr == '_' || *cptr == '.' ||
		*cptr == '!' || *cptr == '~' || *cptr == '*' ||
		*cptr == '\''|| *cptr == '(' || *cptr == ')' ||
		(*cptr == '%' &&
		 ((cptr[1] >= 'A' && cptr[1] <= 'F') ||
		  (cptr[1] >= 'a' && cptr[1] <= 'f') ||
		  (cptr[1] >= '0' && cptr[1] <= '9')) &&
		 ((cptr[2] >= 'A' && cptr[2] <= 'F') ||
		  (cptr[2] >= 'a' && cptr[2] <= 'f') ||
		  (cptr[2] >= '0' && cptr[2] <= '9'))) ||
		(!escape_reserved &&
		 (*cptr == ';' || *cptr == '/' || *cptr == '?' ||
		  *cptr == ':' || *cptr == '@' || *cptr == '&' ||
		  *cptr == '=' || *cptr == '+' || *cptr == '$' ||
		  *cptr == ','))) {
		xmlBufAdd(target, cptr, 1);
	    } else {
		if ((*cptr >> 4) < 10)
		    escape[1] = '0' + (*cptr >> 4);
		else
		    escape[1] = 'A' - 10 + (*cptr >> 4);
		if ((*cptr & 0xF) < 10)
		    escape[2] = '0' + (*cptr & 0xF);
		else
		    escape[2] = 'A' - 10 + (*cptr & 0xF);

		xmlBufAdd(target, &escape[0], 3);
	    }
	}
    }
    valuePush(ctxt, xmlXPathCacheNewString(ctxt->context,
	xmlBufContent(target)));
    xmlBufFree(target);
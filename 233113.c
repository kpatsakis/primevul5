xsltInitDocKeyTable(xsltTransformContextPtr ctxt, const xmlChar *name,
                    const xmlChar *nameURI)
{
    xsltStylesheetPtr style;
    xsltKeyDefPtr keyd = NULL;
    int found = 0;

#ifdef KEY_INIT_DEBUG
fprintf(stderr, "xsltInitDocKeyTable %s\n", name);
#endif

    style = ctxt->style;
    while (style != NULL) {
	keyd = (xsltKeyDefPtr) style->keys;
	while (keyd != NULL) {
	    if (((keyd->nameURI != NULL) ==
		 (nameURI != NULL)) &&
		xmlStrEqual(keyd->name, name) &&
		xmlStrEqual(keyd->nameURI, nameURI))
	    {
		xsltInitCtxtKey(ctxt, ctxt->document, keyd);
		if (ctxt->document->nbKeysComputed == ctxt->nbKeys)
		    return(0);
		found = 1;
	    }
	    keyd = keyd->next;
	}
	style = xsltNextImport(style);
    }
    if (found == 0) {
#ifdef WITH_XSLT_DEBUG_KEYS
	XSLT_TRACE(ctxt,XSLT_TRACE_KEYS,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltInitDocKeyTable: did not found %s\n", name));
#endif
	xsltTransformError(ctxt, NULL, keyd? keyd->inst : NULL,
	    "Failed to find key definition for %s\n", name);
	ctxt->state = XSLT_STATE_STOPPED;
        return(-1);
    }
#ifdef KEY_INIT_DEBUG
fprintf(stderr, "xsltInitDocKeyTable %s done\n", name);
#endif
    return(0);
}
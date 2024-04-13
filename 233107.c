xsltFreeKeys(xsltStylesheetPtr style) {
    if (style->keys)
	xsltFreeKeyDefList((xsltKeyDefPtr) style->keys);
}
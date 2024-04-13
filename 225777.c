 */
xmlNodePtr
xmlXPathNextDescendantOrSelf(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL)
        return(ctxt->context->node);

    if (ctxt->context->node == NULL)
        return(NULL);
    if ((ctxt->context->node->type == XML_ATTRIBUTE_NODE) ||
        (ctxt->context->node->type == XML_NAMESPACE_DECL))
        return(NULL);

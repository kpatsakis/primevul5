 */
static xmlNodePtr
xmlXPathNextPrecedingInternal(xmlXPathParserContextPtr ctxt,
                              xmlNodePtr cur)
{
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL) {
        cur = ctxt->context->node;
        if (cur == NULL)
            return (NULL);
        if (cur->type == XML_ATTRIBUTE_NODE) {
            cur = cur->parent;
        } else if (cur->type == XML_NAMESPACE_DECL) {
            xmlNsPtr ns = (xmlNsPtr) cur;

            if ((ns->next == NULL) ||
                (ns->next->type == XML_NAMESPACE_DECL))
                return (NULL);
            cur = (xmlNodePtr) ns->next;
        }
        ctxt->ancestor = cur->parent;
    }
    if (cur->type == XML_NAMESPACE_DECL)
        return(NULL);
    if ((cur->prev != NULL) && (cur->prev->type == XML_DTD_NODE))
	cur = cur->prev;
    while (cur->prev == NULL) {
        cur = cur->parent;
        if (cur == NULL)
            return (NULL);
        if (cur == ctxt->context->doc->children)
            return (NULL);
        if (cur != ctxt->ancestor)
            return (cur);
        ctxt->ancestor = cur->parent;
    }
    cur = cur->prev;
    while (cur->last != NULL)
        cur = cur->last;
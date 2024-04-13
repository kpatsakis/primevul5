 */
static int
xmlXPathRunStreamEval(xmlXPathContextPtr ctxt, xmlPatternPtr comp,
		      xmlXPathObjectPtr *resultSeq, int toBool)
{
    int max_depth, min_depth;
    int from_root;
    int ret, depth;
    int eval_all_nodes;
    xmlNodePtr cur = NULL, limit = NULL;
    xmlStreamCtxtPtr patstream = NULL;

    int nb_nodes = 0;

    if ((ctxt == NULL) || (comp == NULL))
        return(-1);
    max_depth = xmlPatternMaxDepth(comp);
    if (max_depth == -1)
        return(-1);
    if (max_depth == -2)
        max_depth = 10000;
    min_depth = xmlPatternMinDepth(comp);
    if (min_depth == -1)
        return(-1);
    from_root = xmlPatternFromRoot(comp);
    if (from_root < 0)
        return(-1);
#if 0
    printf("stream eval: depth %d from root %d\n", max_depth, from_root);
#endif

    if (! toBool) {
	if (resultSeq == NULL)
	    return(-1);
	*resultSeq = xmlXPathCacheNewNodeSet(ctxt, NULL);
	if (*resultSeq == NULL)
	    return(-1);
    }

    /*
     * handle the special cases of "/" amd "." being matched
     */
    if (min_depth == 0) {
	if (from_root) {
	    /* Select "/" */
	    if (toBool)
		return(1);
	    xmlXPathNodeSetAddUnique((*resultSeq)->nodesetval,
		                     (xmlNodePtr) ctxt->doc);
	} else {
	    /* Select "self::node()" */
	    if (toBool)
		return(1);
	    xmlXPathNodeSetAddUnique((*resultSeq)->nodesetval, ctxt->node);
	}
    }
    if (max_depth == 0) {
	return(0);
    }

    if (from_root) {
        cur = (xmlNodePtr)ctxt->doc;
    } else if (ctxt->node != NULL) {
        switch (ctxt->node->type) {
            case XML_ELEMENT_NODE:
            case XML_DOCUMENT_NODE:
            case XML_DOCUMENT_FRAG_NODE:
            case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
            case XML_DOCB_DOCUMENT_NODE:
#endif
	        cur = ctxt->node;
		break;
            case XML_ATTRIBUTE_NODE:
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_ENTITY_REF_NODE:
            case XML_ENTITY_NODE:
            case XML_PI_NODE:
            case XML_COMMENT_NODE:
            case XML_NOTATION_NODE:
            case XML_DTD_NODE:
            case XML_DOCUMENT_TYPE_NODE:
            case XML_ELEMENT_DECL:
            case XML_ATTRIBUTE_DECL:
            case XML_ENTITY_DECL:
            case XML_NAMESPACE_DECL:
            case XML_XINCLUDE_START:
            case XML_XINCLUDE_END:
		break;
	}
	limit = cur;
    }
    if (cur == NULL) {
        return(0);
    }

    patstream = xmlPatternGetStreamCtxt(comp);
    if (patstream == NULL) {
	/*
	* QUESTION TODO: Is this an error?
	*/
	return(0);
    }

    eval_all_nodes = xmlStreamWantsAnyNode(patstream);

    if (from_root) {
	ret = xmlStreamPush(patstream, NULL, NULL);
	if (ret < 0) {
	} else if (ret == 1) {
	    if (toBool)
		goto return_1;
	    xmlXPathNodeSetAddUnique((*resultSeq)->nodesetval, cur);
	}
    }
    depth = 0;
    goto scan_children;
next_node:
    do {
        nb_nodes++;

	switch (cur->type) {
	    case XML_ELEMENT_NODE:
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
	    case XML_COMMENT_NODE:
	    case XML_PI_NODE:
		if (cur->type == XML_ELEMENT_NODE) {
		    ret = xmlStreamPush(patstream, cur->name,
				(cur->ns ? cur->ns->href : NULL));
		} else if (eval_all_nodes)
		    ret = xmlStreamPushNode(patstream, NULL, NULL, cur->type);
		else
		    break;

		if (ret < 0) {
		    /* NOP. */
		} else if (ret == 1) {
		    if (toBool)
			goto return_1;
		    if (xmlXPathNodeSetAddUnique((*resultSeq)->nodesetval, cur)
		        < 0) {
			ctxt->lastError.domain = XML_FROM_XPATH;
			ctxt->lastError.code = XML_ERR_NO_MEMORY;
		    }
		}
		if ((cur->children == NULL) || (depth >= max_depth)) {
		    ret = xmlStreamPop(patstream);
		    while (cur->next != NULL) {
			cur = cur->next;
			if ((cur->type != XML_ENTITY_DECL) &&
			    (cur->type != XML_DTD_NODE))
			    goto next_node;
		    }
		}
	    default:
		break;
	}

scan_children:
	if (cur->type == XML_NAMESPACE_DECL) break;
	if ((cur->children != NULL) && (depth < max_depth)) {
	    /*
	     * Do not descend on entities declarations
	     */
	    if (cur->children->type != XML_ENTITY_DECL) {
		cur = cur->children;
		depth++;
		/*
		 * Skip DTDs
		 */
		if (cur->type != XML_DTD_NODE)
		    continue;
	    }
	}

	if (cur == limit)
	    break;

	while (cur->next != NULL) {
	    cur = cur->next;
	    if ((cur->type != XML_ENTITY_DECL) &&
		(cur->type != XML_DTD_NODE))
		goto next_node;
	}

	do {
	    cur = cur->parent;
	    depth--;
	    if ((cur == NULL) || (cur == limit))
	        goto done;
	    if (cur->type == XML_ELEMENT_NODE) {
		ret = xmlStreamPop(patstream);
	    } else if ((eval_all_nodes) &&
		((cur->type == XML_TEXT_NODE) ||
		 (cur->type == XML_CDATA_SECTION_NODE) ||
		 (cur->type == XML_COMMENT_NODE) ||
		 (cur->type == XML_PI_NODE)))
	    {
		ret = xmlStreamPop(patstream);
	    }
	    if (cur->next != NULL) {
		cur = cur->next;
		break;
	    }
	} while (cur != NULL);

    } while ((cur != NULL) && (depth >= 0));

done:

#if 0
    printf("stream eval: checked %d nodes selected %d\n",
           nb_nodes, retObj->nodesetval->nodeNr);
#endif

    if (patstream)
	xmlFreeStreamCtxt(patstream);
    return(0);

return_1:
    if (patstream)
	xmlFreeStreamCtxt(patstream);
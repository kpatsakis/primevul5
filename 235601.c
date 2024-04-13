XMLTree::find(const string xpath, XMLNode* node) const
{
	xmlXPathContext* ctxt;
	xmlDocPtr doc = 0;

	if (node) {
		doc = xmlNewDoc(xml_version);
		writenode(doc, node, doc->children, 1);
		ctxt = xmlXPathNewContext(doc);
	} else {
		ctxt = xmlXPathNewContext(_doc);
	}

	boost::shared_ptr<XMLSharedNodeList> result =
		boost::shared_ptr<XMLSharedNodeList>(find_impl(ctxt, xpath));

	xmlXPathFreeContext(ctxt);
	if (doc) {
		xmlFreeDoc (doc);
	}

	return result;
}
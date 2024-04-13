writenode(xmlDocPtr doc, XMLNode* n, xmlNodePtr p, int root = 0)
{
	xmlNodePtr node;

	if (root) {
		node = doc->children = xmlNewDocNode(doc, 0, (const xmlChar*) n->name().c_str(), 0);
	} else {
		node = xmlNewChild(p, 0, (const xmlChar*) n->name().c_str(), 0);
	}

	if (n->is_content()) {
		node->type = XML_TEXT_NODE;
		xmlNodeSetContentLen(node, (const xmlChar*)n->content().c_str(), n->content().length());
	}

	const XMLPropertyList& props = n->properties();

	for (XMLPropertyConstIterator prop_iter = props.begin (); prop_iter != props.end ();
	     ++prop_iter) {
		xmlSetProp (node, (const xmlChar*)(*prop_iter)->name ().c_str (),
		            (const xmlChar*)(*prop_iter)->value ().c_str ());
	}

	const XMLNodeList& children = n->children ();
	for (XMLNodeConstIterator child_iter = children.begin (); child_iter != children.end ();
	     ++child_iter) {
		writenode (doc, *child_iter, node);
	}
}
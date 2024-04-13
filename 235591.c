XMLTree::debug(FILE* out) const
{
#ifdef LIBXML_DEBUG_ENABLED
	xmlDocPtr doc;
	XMLNodeList children;

	xmlKeepBlanksDefault(0);
	doc = xmlNewDoc(xml_version);
	xmlSetDocCompressMode(doc, _compression);
	writenode(doc, _root, doc->children, 1);
	xmlDebugDumpDocument (out, doc);
	xmlFreeDoc(doc);
#endif
}
XMLTree::write_buffer() const
{
	static string retval;
	char* ptr;
	int len;
	xmlDocPtr doc;
	XMLNodeList children;

	xmlKeepBlanksDefault(0);
	doc = xmlNewDoc(xml_version);
	xmlSetDocCompressMode(doc, _compression);
	writenode(doc, _root, doc->children, 1);
	xmlDocDumpMemory(doc, (xmlChar **) & ptr, &len);
	xmlFreeDoc(doc);

	retval = ptr;

	free(ptr);

	return retval;
}
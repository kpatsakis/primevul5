XMLTree::read_internal(bool validate)
{
	//shouldnt be used anywhere ATM, remove if so!
	assert(!validate);

	delete _root;
	_root = 0;

	if (_doc) {
		xmlFreeDoc (_doc);
		_doc = 0;
	}

	/* Calling this prevents libxml2 from treating whitespace as active
	   nodes. It needs to be called before we create a parser context.
	*/
	xmlKeepBlanksDefault(0);

	/* create a parser context */
	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	if (ctxt == NULL) {
		return false;
	}

	/* parse the file, activating the DTD validation option */
	if (validate) {
		_doc = xmlCtxtReadFile(ctxt, _filename.c_str(), NULL, XML_PARSE_DTDVALID);
	} else {
		_doc = xmlCtxtReadFile(ctxt, _filename.c_str(), NULL, XML_PARSE_HUGE);
	}

	/* check if parsing suceeded */
	if (_doc == NULL) {
		xmlFreeParserCtxt(ctxt);
		return false;
	} else {
		/* check if validation suceeded */
		if (validate && ctxt->valid == 0) {
			xmlFreeParserCtxt(ctxt);
			throw XMLException("Failed to validate document " + _filename);
		}
	}

	_root = readnode(xmlDocGetRootElement(_doc));

	/* free up the parser context */
	xmlFreeParserCtxt(ctxt);

	return true;
}
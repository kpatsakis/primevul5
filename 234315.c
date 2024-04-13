Agraph_t *graphml_to_gv(char* gname, FILE * graphmlFile, int* rv)
{
    char buf[BUFSIZE];
    int done;
    userdata_t *udata = genUserdata(gname);
    XML_Parser parser = XML_ParserCreate(NULL);

    *rv = 0;
    XML_SetUserData(parser, udata);
    XML_SetElementHandler(parser, startElementHandler, endElementHandler);
    XML_SetCharacterDataHandler(parser, characterDataHandler);

    Current_class = TAG_GRAPH;
    root = 0;
    do {
	size_t len = fread(buf, 1, sizeof(buf), graphmlFile);
	if (len == 0)
	    break;
	done = len < sizeof(buf);
	if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
	    fprintf(stderr,
		    "%s at line %lu\n",
		    XML_ErrorString(XML_GetErrorCode(parser)),
		    XML_GetCurrentLineNumber(parser));
	    *rv = 1;
	    break;
	}
    } while (!done);
    XML_ParserFree(parser);
    freeUserdata(udata);

    return root;
}
static PHP_MINIT_FUNCTION(libxml)
{
	zend_class_entry ce;

	php_libxml_initialize();

	REGISTER_LONG_CONSTANT("LIBXML_VERSION",			LIBXML_VERSION,			CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("LIBXML_DOTTED_VERSION",	LIBXML_DOTTED_VERSION,	CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("LIBXML_LOADED_VERSION",	(char *)xmlParserVersion,		CONST_CS | CONST_PERSISTENT);

	/* For use with loading xml */
	REGISTER_LONG_CONSTANT("LIBXML_NOENT",		XML_PARSE_NOENT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDLOAD",	XML_PARSE_DTDLOAD,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDATTR",	XML_PARSE_DTDATTR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDVALID",	XML_PARSE_DTDVALID,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOERROR",	XML_PARSE_NOERROR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOWARNING",	XML_PARSE_NOWARNING,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOBLANKS",	XML_PARSE_NOBLANKS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_XINCLUDE",	XML_PARSE_XINCLUDE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NSCLEAN",	XML_PARSE_NSCLEAN,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOCDATA",	XML_PARSE_NOCDATA,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NONET",		XML_PARSE_NONET,		CONST_CS | CONST_PERSISTENT);
#if LIBXML_VERSION >= 20621
	REGISTER_LONG_CONSTANT("LIBXML_COMPACT",	XML_PARSE_COMPACT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOXMLDECL",	XML_SAVE_NO_DECL,		CONST_CS | CONST_PERSISTENT);
#endif
#if LIBXML_VERSION >= 20703
	REGISTER_LONG_CONSTANT("LIBXML_PARSEHUGE",	XML_PARSE_HUGE,			CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("LIBXML_NOEMPTYTAG",	LIBXML_SAVE_NOEMPTYTAG,	CONST_CS | CONST_PERSISTENT);

	/* Error levels */
	REGISTER_LONG_CONSTANT("LIBXML_ERR_NONE",		XML_ERR_NONE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_WARNING",	XML_ERR_WARNING,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_ERROR",		XML_ERR_ERROR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_FATAL",		XML_ERR_FATAL,		CONST_CS | CONST_PERSISTENT);

	INIT_CLASS_ENTRY(ce, "LibXMLError", NULL);
	libxmlerror_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}
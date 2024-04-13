static void SVGCDataBlock(void *context,const xmlChar *value,int length)
{
  SVGInfo
    *svg_info;

  xmlNodePtr
    child;

  xmlParserCtxtPtr
    parser;

  /*
    Called when a pcdata block has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.pcdata(%s, %d)",
    value,length);
  svg_info=(SVGInfo *) context;
  parser=svg_info->parser;
  child=xmlGetLastChild(parser->node);
  if ((child != (xmlNodePtr) NULL) && (child->type == XML_CDATA_SECTION_NODE))
    {
      xmlTextConcat(child,value,length);
      return;
    }
  child=xmlNewCDataBlock(parser->myDoc,value,length);
  if (xmlAddChild(parser->node,child) == (xmlNodePtr) NULL)
    xmlFreeNode(child);
}
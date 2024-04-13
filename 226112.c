static int xar_get_numeric_from_xml_element(xmlTextReaderPtr reader, size_t * value)
{
    const xmlChar * numstr;
    ssize_t numval;

    if (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {
        numstr = xmlTextReaderConstValue(reader);
        if (numstr) {
            numval = atol((const char *)numstr);
            if (numval < 0) {
                cli_dbgmsg("cli_scanxar: XML element value %li\n", *value);
                return CL_EFORMAT;
            }
            *value = numval;
            return CL_SUCCESS;
        }
    }
    cli_dbgmsg("cli_scanxar: No text for XML element\n");
    return CL_EFORMAT;
}
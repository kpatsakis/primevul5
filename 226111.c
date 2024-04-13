static int xar_scan_subdocuments(xmlTextReaderPtr reader, cli_ctx *ctx)
{
    int rc = CL_SUCCESS, subdoc_len, fd;
    xmlChar * subdoc;
    const xmlChar *name;
    char * tmpname;

    while (xmlTextReaderRead(reader) == 1) {
        name = xmlTextReaderConstLocalName(reader);
        if (name == NULL) {
            cli_dbgmsg("cli_scanxar: xmlTextReaderConstLocalName() no name.\n");
            rc = CL_EFORMAT;
            break;
        }
        if (xmlStrEqual(name, (const xmlChar *)"toc") && 
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
            return CL_SUCCESS;
        if (xmlStrEqual(name, (const xmlChar *)"subdoc") && 
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
            subdoc = xmlTextReaderReadInnerXml(reader);
            if (subdoc == NULL) {
                cli_dbgmsg("cli_scanxar: no content in subdoc element.\n");
                xmlTextReaderNext(reader);
                continue;
            }
            subdoc_len = xmlStrlen(subdoc);
            cli_dbgmsg("cli_scanxar: in-memory scan of xml subdocument, len %i.\n", subdoc_len);
            rc = cli_mem_scandesc(subdoc, subdoc_len, ctx);
            if (rc == CL_VIRUS && SCAN_ALL)
                rc = CL_SUCCESS;
            
            /* make a file to leave if --leave-temps in effect */
            if(ctx->engine->keeptmp) {
                if ((rc = cli_gentempfd(ctx->engine->tmpdir, &tmpname, &fd)) != CL_SUCCESS) {
                    cli_dbgmsg("cli_scanxar: Can't create temporary file for subdocument.\n");
                } else {
                    cli_dbgmsg("cli_scanxar: Writing subdoc to temp file %s.\n", tmpname);
                    if (cli_writen(fd, subdoc, subdoc_len) < 0) {
                        cli_dbgmsg("cli_scanxar: cli_writen error writing subdoc temporary file.\n");
                        rc = CL_EWRITE;
                    }
                    rc = xar_cleanup_temp_file(ctx, fd, tmpname);
                }
            }

            xmlFree(subdoc);
            if (rc != CL_SUCCESS)
                return rc;
            xmlTextReaderNext(reader);
        }        
    }
    return rc;
}
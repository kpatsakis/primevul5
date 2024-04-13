gopherEndHTML(GopherStateData * gopherState)
{
    StoreEntry *e = gopherState->entry;

    if (!gopherState->HTML_header_added) {
        gopherHTMLHeader(e, "Server Return Nothing", NULL);
        storeAppendPrintf(e, "<P>The Gopher query resulted in a blank response</P>");
    } else if (gopherState->HTML_pre) {
        storeAppendPrintf(e, "</PRE>\n");
    }

    gopherHTMLFooter(e);
}
gopherHTMLHeader(StoreEntry * e, const char *title, const char *substring)
{
    storeAppendPrintf(e, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n");
    storeAppendPrintf(e, "<HTML><HEAD><TITLE>");
    storeAppendPrintf(e, title, substring);
    storeAppendPrintf(e, "</TITLE>");
    storeAppendPrintf(e, "<STYLE type=\"text/css\"><!--BODY{background-color:#ffffff;font-family:verdana,sans-serif}--></STYLE>\n");
    storeAppendPrintf(e, "</HEAD>\n<BODY><H1>");
    storeAppendPrintf(e, title, substring);
    storeAppendPrintf(e, "</H1>\n");
}
xsltFreeDocumentKeys(xsltDocumentPtr idoc) {
    if (idoc != NULL)
        xsltFreeKeyTableList(idoc->keys);
}
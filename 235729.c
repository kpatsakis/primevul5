static bool jsi_csObjIsTrue(void *data)
{
    CDataObj *fo = (CDataObj *)data;
    if (!fo->fobj) return JSI_OK;
    else return 1;
}
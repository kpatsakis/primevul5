GetHeriarchy(
	     TPMI_DH_OBJECT   handle         // IN :object handle
	     )
{
    OBJECT          *object = HandleToObject(handle);
    return ObjectGetHierarchy(object);
}
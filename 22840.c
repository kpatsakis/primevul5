FlushObject(
	    TPMI_DH_OBJECT   handle         // IN: handle to be freed
	    )
{
    UINT32      index = handle - TRANSIENT_FIRST;
    pAssert(index < MAX_LOADED_OBJECTS);
    // Clear all the object attributes
    MemorySet((BYTE*)&(s_objects[index].attributes),
	      0, sizeof(OBJECT_ATTRIBUTES));
    return;
}
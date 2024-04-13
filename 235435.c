bool address_space_access_valid(AddressSpace *as, hwaddr addr,
                                hwaddr len, bool is_write,
                                MemTxAttrs attrs)
{
    FlatView *fv;

    RCU_READ_LOCK_GUARD();
    fv = address_space_to_flatview(as);
    return flatview_access_valid(fv, addr, len, is_write, attrs);
}
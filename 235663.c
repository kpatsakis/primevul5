static int jsi_NumWideBytes(Jsi_Wide val) {
    if (val < 0x10000)
        return (val < 0x100 ? 1 : 2);
    return(val < 0x100000000L ? 4 : 8);
}
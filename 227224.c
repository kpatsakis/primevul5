static const char *get_type_name(int type) {
    const char *types[9] = {
        "null", "int (8-bit)", "int (16 bit)", "int (32 bit)",
        "unknown", "float", "unknown", "char", "unknown"
    };
    int t = (type >= 0 && type < 8) ? type : 8;
    return types[t];
}
Jsi_Value *jsi_ObjArrayLookup(Jsi_Interp *interp, Jsi_Obj *obj, const char *key) {
    if (!obj->arr || !key || !isdigit(*key))
        return NULL;
    char *ep = NULL;
    int n = (int)strtol(key, &ep, 0);
    if (n<0 || n >= (int)obj->arrCnt)
        return NULL;
    Jsi_Value *v = obj->arr[n];
    return v;
}
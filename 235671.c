void Jsi_ObjListifyArray(Jsi_Interp *interp, Jsi_Obj *obj)
{
    if (!obj->isarrlist) {
        Jsi_LogBug("Can not listify a non-array");
        return;
    }
    if (obj->arr) return;
    Jsi_TreeWalk(obj->tree, ObjListifyCallback, obj, 0);

    do {
        interp->delRBCnt = 0;
        Jsi_TreeWalk(obj->tree, ObjListifyArrayCallback, obj, 0);
    } while (interp->delRBCnt);
}
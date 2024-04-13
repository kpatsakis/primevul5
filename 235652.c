Jsi_IterObj *Jsi_IterObjNew(Jsi_Interp *interp, Jsi_IterProc *iterCmd)
{
    Jsi_IterObj *o = (Jsi_IterObj*)Jsi_Calloc(1,sizeof(Jsi_IterObj));
    o->interp = interp;
    o->iterCmd = iterCmd;
    return o;
}
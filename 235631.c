Jsi_Obj *Jsi_ObjNewType(Jsi_Interp *interp, Jsi_otype otype)
{
    Jsi_Obj *obj = Jsi_ObjNew(interp);
    obj->ot = (otype==JSI_OT_ARRAY?JSI_OT_OBJECT:otype);
    switch (otype) {
        case JSI_OT_BOOL:   obj->__proto__ = interp->Boolean_prototype; break;
        case JSI_OT_NUMBER: obj->__proto__ = interp->Number_prototype; break;
        case JSI_OT_STRING: obj->__proto__ = interp->String_prototype; break;
        case JSI_OT_FUNCTION:obj->__proto__ = interp->Function_prototype; break;
        case JSI_OT_REGEXP: obj->__proto__ = interp->RegExp_prototype; break;
        case JSI_OT_OBJECT: obj->__proto__ = interp->Object_prototype; break;
        case JSI_OT_ARRAY:  obj->__proto__ = interp->Array_prototype;
            obj->isarrlist = 1;
            break;
        default: assert(0); break;
    }
    if (interp->protoInit)
        assert(obj->__proto__);
    return obj;
}
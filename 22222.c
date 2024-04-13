static void sungem_instance_init(Object *obj)
{
    SunGEMState *s = SUNGEM(obj);

    device_add_bootindex_property(obj, &s->conf.bootindex,
                                  "bootindex", "/ethernet-phy@0",
                                  DEVICE(obj));
}
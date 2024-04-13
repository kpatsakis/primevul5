virSecuritySELinuxSetSysinfoLabel(virSecurityManager *mgr,
                                  virSysinfoDef *def,
                                  virSecuritySELinuxData *data)
{
    size_t i;

    for (i = 0; i < def->nfw_cfgs; i++) {
        virSysinfoFWCfgDef *f = &def->fw_cfgs[i];

        if (f->file &&
            virSecuritySELinuxSetFilecon(mgr, f->file,
                                         data->content_context, true) < 0)
            return -1;
    }

    return 0;
}
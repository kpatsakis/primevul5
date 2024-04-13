virSecuritySELinuxRestoreSysinfoLabel(virSecurityManager *mgr,
                                      virSysinfoDef *def)
{
    size_t i;

    for (i = 0; i < def->nfw_cfgs; i++) {
        virSysinfoFWCfgDef *f = &def->fw_cfgs[i];

        if (f->file &&
            virSecuritySELinuxRestoreFileLabel(mgr, f->file, true) < 0)
            return -1;
    }

    return 0;
}
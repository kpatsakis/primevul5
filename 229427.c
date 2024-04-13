virSecuritySELinuxGenLabel(virSecurityManager *mgr,
                           virDomainDef *def)
{
    int rc = -1;
    char *mcs = NULL;
    context_t ctx = NULL;
    const char *range;
    virSecurityLabelDef *seclabel;
    virSecuritySELinuxData *data;
    const char *baselabel;
    char *sens = NULL;
    int catMin, catMax;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (seclabel == NULL)
        return 0;

    data = virSecurityManagerGetPrivateData(mgr);

    VIR_DEBUG("label=%s", virSecurityManagerGetVirtDriver(mgr));
    if (seclabel->type == VIR_DOMAIN_SECLABEL_DYNAMIC &&
        seclabel->label) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("security label already defined for VM"));
        return rc;
    }

    if (seclabel->imagelabel) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("security image label already defined for VM"));
        return rc;
    }

    if (seclabel->model &&
        STRNEQ(seclabel->model, SECURITY_SELINUX_NAME)) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("security label model %s is not supported with selinux"),
                       seclabel->model);
        return rc;
    }

    VIR_DEBUG("type=%d", seclabel->type);

    switch (seclabel->type) {
    case VIR_DOMAIN_SECLABEL_STATIC:
        if (!(ctx = context_new(seclabel->label))) {
            virReportSystemError(errno,
                                 _("unable to allocate socket security context '%s'"),
                                 seclabel->label);
            return rc;
        }

        if (!(range = context_range_get(ctx))) {
            virReportSystemError(errno, "%s", _("unable to get selinux context range"));
            goto cleanup;
        }
        mcs = g_strdup(range);
        break;

    case VIR_DOMAIN_SECLABEL_DYNAMIC:
        if (virSecuritySELinuxMCSGetProcessRange(&sens,
                                                 &catMin,
                                                 &catMax) < 0)
            goto cleanup;

        if (!(mcs = virSecuritySELinuxMCSFind(mgr,
                                              sens,
                                              catMin,
                                              catMax)))
            goto cleanup;

        if (virSecuritySELinuxMCSAdd(mgr, mcs) < 0)
            goto cleanup;

        baselabel = seclabel->baselabel;
        if (!baselabel) {
            if (def->virtType == VIR_DOMAIN_VIRT_QEMU) {
                if (data->alt_domain_context == NULL) {
                    static bool warned;
                    if (!warned) {
                        VIR_WARN("SELinux policy does not define a domain type for QEMU TCG. "
                                 "Guest startup may be denied due to missing 'execmem' privilege "
                                 "unless the 'virt_use_execmem' policy boolean is enabled");
                        warned = true;
                    }
                    baselabel = data->domain_context;
                } else {
                    baselabel = data->alt_domain_context;
                }
            } else {
                baselabel = data->domain_context;
            }
        }

        seclabel->label = virSecuritySELinuxGenNewContext(baselabel, mcs, false);
        if (!seclabel->label)
            goto cleanup;

        break;

    case VIR_DOMAIN_SECLABEL_NONE:
        if (virSecuritySELinuxMCSGetProcessRange(&sens,
                                                 &catMin,
                                                 &catMax) < 0)
            goto cleanup;

        mcs = g_strdup(sens);

        break;

    default:
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("unexpected security label type '%s'"),
                       virDomainSeclabelTypeToString(seclabel->type));
        goto cleanup;
    }

    /* always generate a image label, needed to label new objects */
    seclabel->imagelabel = virSecuritySELinuxGenNewContext(data->file_context,
                                                           mcs,
                                                           true);
    if (!seclabel->imagelabel)
        goto cleanup;

    if (!seclabel->model)
        seclabel->model = g_strdup(SECURITY_SELINUX_NAME);

    rc = 0;

 cleanup:
    if (rc != 0) {
        if (seclabel->type == VIR_DOMAIN_SECLABEL_DYNAMIC)
            VIR_FREE(seclabel->label);
        VIR_FREE(seclabel->imagelabel);
        if (seclabel->type == VIR_DOMAIN_SECLABEL_DYNAMIC &&
            !seclabel->baselabel)
            VIR_FREE(seclabel->model);
    }

    if (ctx)
        context_free(ctx);
    VIR_FREE(mcs);
    VIR_FREE(sens);

    VIR_DEBUG("model=%s label=%s imagelabel=%s baselabel=%s",
              NULLSTR(seclabel->model),
              NULLSTR(seclabel->label),
              NULLSTR(seclabel->imagelabel),
              NULLSTR(seclabel->baselabel));

    return rc;
}
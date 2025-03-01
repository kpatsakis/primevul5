static Jsi_Interp* jsi_InterpNew(Jsi_Interp *parent, Jsi_Value *opts, Jsi_InterpOpts *iopts)
{
    Jsi_Interp* interp;
    if (parent && parent->noSubInterps) {
        interp = parent;
        Jsi_LogError("subinterps disallowed");
        return NULL;
    }
    if (opts && parent && (Jsi_ValueIsObjType(parent, opts, JSI_OT_OBJECT)==0 ||
        Jsi_TreeSize(opts->d.obj->tree)<=0))
        opts = NULL;
    interp = (Jsi_Interp *)Jsi_Calloc(1,sizeof(*interp) + sizeof(jsi_Frame));
    interp->framePtr = (jsi_Frame*)(((uchar*)interp)+sizeof(*interp));
    if (!parent)
        interp->maxInterpDepth = JSI_MAX_SUBINTERP_DEPTH;
    else {
        interp->maxInterpDepth = parent->maxInterpDepth;
        interp->interpDepth = parent->interpDepth+1;
        if (interp->interpDepth > interp->maxInterpDepth) {
            Jsi_Free(interp);
            interp = parent;
            Jsi_LogError("exceeded max subinterp depth");
            return NULL;
        }
    }
    interp->maxDepth = JSI_MAX_EVAL_DEPTH;
    interp->maxIncDepth = JSI_MAX_INCLUDE_DEPTH;
    interp->maxArrayList = MAX_ARRAY_LIST;
    interp->typeWarnMax = 50;
    interp->subOpts.dblPrec = __DBL_DECIMAL_DIG__-1;
    interp->subOpts.prompt = "$ ";
    interp->subOpts.prompt2 = "> ";

    int iocnt;
    if (iopts) {
        iopts->interp = interp;
        interp->opts = *iopts;
    }
    interp->logOpts.file = 1;
    interp->logOpts.func = 1;
    interp->logOpts.Info = 1;
    interp->logOpts.Warn = 1;
    interp->logOpts.Error = 1;
    int argc = interp->opts.argc;
    char **argv = interp->opts.argv;
    char *argv0 = (argv?argv[0]:NULL);
    interp->parent = parent;
    interp->topInterp = (parent == NULL ? interp: parent->topInterp);
    if (jsiIntData.mainInterp == NULL)
        jsiIntData.mainInterp = interp->topInterp;
    interp->mainInterp = jsiIntData.mainInterp; // The first interps handles exit.
    interp->memDebug = interp->opts.mem_debug;
    if (parent) {
        interp->dbPtr = parent->dbPtr;
    } else {
        interp->dbPtr = &interp->dbStatic;
    }
#ifdef JSI_MEM_DEBUG
    if (!interp->dbPtr->valueDebugTbl) {
        interp->dbPtr->valueDebugTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, NULL);
        interp->dbPtr->objDebugTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, NULL);
    }
#endif
    if (parent) {
        if (parent->pkgDirs)
            interp->pkgDirs = Jsi_ValueDupJSON(interp, parent->pkgDirs);
    } else {
#ifdef JSI_PKG_DIRS
        interp->pkgDirs = Jsi_StringSplit(interp, JSI_PKG_DIRS, ",");
        Jsi_IncrRefCount(interp, interp->pkgDirs);
#endif
    }
#ifdef JSI_USE_COMPAT
    interp->compat = JSI_USE_COMPAT;
#endif
#ifndef JSI_CONF_ARGS
#define JSI_CONF_ARGS ""
#endif
    interp->confArgs = JSI_CONF_ARGS;
    for (iocnt = 1; (iocnt+1)<argc; iocnt+=2)
    {
        const char *aio = argv[iocnt];
        if (Jsi_Strcmp(aio, "--T") == 0 || Jsi_Strcmp(aio, "--C") == 0 || Jsi_Strcmp(aio, "--L") == 0) {
            continue;
        }
        if (Jsi_Strcmp(aio, "--F") == 0 || Jsi_Strcmp(aio, "--U") == 0 || Jsi_Strcmp(aio, "--V") == 0) {
            iocnt--;
            continue;
        }
        if (!Jsi_Strcmp(aio, "--I")) {
            const char *aio2 = argv[iocnt+1];
            if (!Jsi_Strncmp("memDebug:", aio2, sizeof("memDebug")))
                interp->memDebug=strtol(aio2+sizeof("memDebug"), NULL, 0);
            else if (!Jsi_Strncmp("compat", aio2, sizeof("compat")))
                interp->subOpts.compat=strtol(aio2+sizeof("compat"), NULL, 0);
            continue;
        }
        break;
    }
    SIGINIT(interp,INTERP);
    interp->NullValue = Jsi_ValueNewNull(interp);
    Jsi_IncrRefCount(interp, interp->NullValue);
#ifdef __WIN32
    Jsi_DString cwdStr;
    Jsi_DSInit(&cwdStr);
    interp->curDir = Jsi_Strdup(Jsi_GetCwd(interp, &cwdStr));
    Jsi_DSFree(&cwdStr);
#else
    char buf[JSI_BUFSIZ];
    interp->curDir = getcwd(buf, sizeof(buf));
    interp->curDir = Jsi_Strdup(interp->curDir?interp->curDir:".");
#endif
    interp->onDeleteTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, freeOnDeleteTbl);
    interp->assocTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, freeAssocTbl);
    interp->cmdSpecTbl = Jsi_MapNew(interp, JSI_MAP_TREE, JSI_KEYS_STRING, freeCmdSpecTbl);
    interp->eventTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, freeEventTbl);
    interp->fileTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, jsi_HashFree);
    interp->funcObjTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, freeFuncObjTbl);
    interp->funcsTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, freeFuncsTbl);
    interp->bindTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, freeBindObjTbl);
    interp->protoTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL/*freeValueTbl*/);
    interp->regexpTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, regExpFree);
    interp->preserveTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, jsi_HashFree);
    interp->loadTbl = (parent?parent->loadTbl:Jsi_HashNew(interp, JSI_KEYS_STRING, jsi_FreeOneLoadHandle));
    interp->packageHash = Jsi_HashNew(interp, JSI_KEYS_STRING, packageHashFree);
    interp->aliasHash = Jsi_HashNew(interp, JSI_KEYS_STRING, jsi_AliasFree);

    interp->lockTimeout = -1;
#ifdef JSI_LOCK_TIMEOUT
    interp->lockTimeout JSI_LOCK_TIMEOUT;
#endif
#ifndef JSI_DO_UNLOCK
#define JSI_DO_UNLOCK 1
#endif
    interp->subOpts.mutexUnlock = JSI_DO_UNLOCK;
    Jsi_Map_Type mapType = JSI_MAP_HASH;
#ifdef JSI_USE_MANY_STRKEY
    mapType = JSI_MAP_TREE;
#endif

    if (interp == jsiIntData.mainInterp || interp->threadId != jsiIntData.mainInterp->threadId) {
        interp->strKeyTbl = Jsi_MapNew(interp,  mapType, JSI_KEYS_STRING, NULL);
        interp->subOpts.privKeys = 1;
    }
    // Handle interp options: -T value and -Ixxx value
    for (iocnt = 1; (iocnt+1)<argc && !interp->parent; iocnt+=2)
    {
        const char *aio = argv[iocnt];
        if (Jsi_Strcmp(aio, "--F") == 0) {
            interp->traceCall |= (jsi_callTraceFuncs |jsi_callTraceArgs |jsi_callTraceReturn | jsi_callTraceBefore | jsi_callTraceFullPath);
            iocnt--;
            interp->iskips++;
            continue;
        }
        if (Jsi_Strcmp(aio, "--U") == 0) {
            interp->asserts = 1;
            interp->unitTest = 1;
            iocnt--;
            interp->iskips++;
            continue;
        }
        if (Jsi_Strcmp(aio, "--V") == 0) {
            interp->asserts = 1;
            interp->unitTest = 5;
            interp->tracePuts = 1;
            iocnt--;
            interp->iskips++;
            continue;
        }
        if (Jsi_Strcmp(aio, "--C") == 0) {
            if (interp->confFile)
               Jsi_LogWarn("overriding confFile: %s", interp->confFile);
            interp->confFile = argv[iocnt+1];
            interp->iskips+=2;
            continue;
        }
        if (Jsi_Strcmp(aio, "--L") == 0) {
            struct stat sb;
            const char* path = argv[iocnt+1]; //TODO: convert to Jsi_Value first?
            if (!path || stat(path, &sb)
                || !((S_ISREG(sb.st_mode) && !access(path, W_OK)) || (S_ISDIR(sb.st_mode) && !access(path, X_OK)))) {
                Jsi_LogError("Lockdown path must exist and be a writable file or executable dir: %s", path);
                Jsi_InterpDelete(interp);
                return NULL;
            }
            interp->isSafe = true;
            interp->safeMode = jsi_safe_Lockdown;
            if (interp->safeWriteDirs) {
                Jsi_LogWarn("Overriding safeWriteDirs");
                Jsi_DecrRefCount(interp, interp->safeWriteDirs);
            }
            const char *vda[2] = {};
            char npath[PATH_MAX];
            vda[0] = Jsi_FileRealpathStr(interp, path, npath);
            interp->safeWriteDirs = Jsi_ValueNewArray(interp, vda, 1);
            Jsi_IncrRefCount(interp, interp->safeWriteDirs);
            if (!interp->safeReadDirs) {
                interp->safeReadDirs = interp->safeWriteDirs;
                Jsi_IncrRefCount(interp, interp->safeReadDirs);
            }
            interp->iskips+=2;
            continue;
        }
        if (Jsi_Strcmp(aio, "--T") == 0) {
            if (jsi_ParseTypeCheckStr(interp, argv[iocnt+1]) != JSI_OK) {
                Jsi_InterpDelete(interp);
                return NULL;
            }
            interp->iskips+=2;
            continue;
        }
        if (!Jsi_Strcmp(aio, "--I"))  {
            bool bv = 1;
            char *aio2 = argv[iocnt+1], *aioc = Jsi_Strchr(aio2, ':'),
                argNamS[50], *argNam = aio2;
            const char *argVal;
            if (!Jsi_Strcmp("traceCall", aio2))
                interp->traceCall |= (jsi_callTraceFuncs |jsi_callTraceArgs |jsi_callTraceReturn | jsi_callTraceBefore | jsi_callTraceFullPath);
            else {
                if (aioc) {
                    argNam = argNamS;
                    argVal = aioc+1;
                    snprintf(argNamS, sizeof(argNamS), "%.*s", (int)(aioc-aio2), aio2);
                }
                
                DECL_VALINIT(argV);
                Jsi_Value *argValue = &argV;
                Jsi_Number dv;
                if (!aioc || Jsi_GetBool(interp, argVal, &bv) == JSI_OK) {
                    Jsi_ValueMakeBool(interp, &argValue, bv);
                } else if (!Jsi_Strcmp("null", argVal)) {
                    Jsi_ValueMakeNull(interp, &argValue);
                } else if (Jsi_GetDouble(interp, argVal, &dv) == JSI_OK) {
                    Jsi_ValueMakeNumber(interp, &argValue, dv);
                } else {
                    Jsi_ValueMakeStringKey(interp, &argValue, argVal);
                }
                if (JSI_OK != Jsi_OptionsSet(interp, InterpOptions, interp, argNam, argValue, 0)) {
                    Jsi_InterpDelete(interp);
                    return NULL;
                }
            }
            interp->iskips+=2;
            continue;
        }
        break;
    }
    if (!interp->strKeyTbl)
        interp->strKeyTbl = jsiIntData.mainInterp->strKeyTbl;
    if (opts) {
        interp->inopts = opts = Jsi_ValueDupJSON(interp, opts);
        if (Jsi_OptionsProcess(interp, InterpOptions, interp, opts, 0) < 0) {
            Jsi_DecrRefCount(interp, opts);
            interp->inopts = NULL;
            Jsi_InterpDelete(interp);
            return NULL;
        }
    }
    if (interp == jsiIntData.mainInterp) {
        interp->subthread = 0;
    } else {
        if (opts) {
            if (interp->subOpts.privKeys && interp->strKeyTbl == jsiIntData.mainInterp->strKeyTbl) {
                //Jsi_HashDelete(interp->strKeyTbl);
                Jsi_OptionsFree(interp, InterpOptions, interp, 0); /* Reparse options to populate new key table. */
                interp->strKeyTbl = Jsi_MapNew(interp, mapType, JSI_KEYS_STRING, NULL);
                if (opts->vt != JSI_VT_NULL) Jsi_OptionsProcess(interp, InterpOptions, interp, opts, 0);
            } else if (interp->subOpts.privKeys == 0 && interp->strKeyTbl != jsiIntData.mainInterp->strKeyTbl) {
                Jsi_OptionsFree(interp, InterpOptions, interp, 0); /* Reparse options to populate new key table. */
                Jsi_MapDelete(interp->strKeyTbl);
                interp->strKeyTbl = jsiIntData.mainInterp->strKeyTbl;
                if (opts->vt != JSI_VT_NULL) Jsi_OptionsProcess(interp, InterpOptions, interp, opts, 0);
            }
        }
        if (parent && parent->isSafe) {
            interp->isSafe = 1;
            interp->safeMode = parent->safeMode;
        }
        if (interp->subthread && interp->isSafe) {
            interp->subthread = 0;
            Jsi_LogError("threading disallowed in safe mode");
            Jsi_InterpDelete(interp);
            return NULL;
        }
        if (interp->subthread)
            jsiIntData.mainInterp->threadCnt++;
        if (interp->subthread && interp->strKeyTbl == jsiIntData.mainInterp->strKeyTbl)
            jsiIntData.mainInterp->threadShrCnt++;
        if (jsiIntData.mainInterp->threadShrCnt)
#ifdef JSI_USE_MANY_STRKEY
            jsiIntData.mainInterp->strKeyTbl->v.tree->opts.lockTreeProc = KeyLockerTree;
#else
            jsiIntData.mainInterp->strKeyTbl->v.hash->opts.lockHashProc = KeyLocker;
#endif
    }
    if (parent && parent->isSafe) {
        interp->isSafe = 1;
        interp->safeMode = parent->safeMode;
        interp->maxOpCnt = parent->maxOpCnt;
        if (interp->safeWriteDirs || interp->safeReadDirs || interp->safeExecPattern) {
            Jsi_LogWarn("ignoring safe* options in safe sub-sub-interp");
            if (interp->safeWriteDirs) Jsi_DecrRefCount(interp, interp->safeWriteDirs);
            if (interp->safeReadDirs) Jsi_DecrRefCount(interp, interp->safeReadDirs);
            interp->safeWriteDirs = interp->safeReadDirs = NULL;
            interp->safeExecPattern = NULL;
        }
    }

    jsi_InterpConfFiles(interp);
    if (!interp->udata) {
        interp->udata = Jsi_ValueNewObj(interp, NULL);
        Jsi_IncrRefCount(interp, interp->udata);
    }
    if (interp->subthread && !interp->scriptStr && !interp->scriptFile) {
        Jsi_LogError("subthread interp must be specify either scriptFile or scriptStr");
        Jsi_InterpDelete(interp);
        return NULL;
    }
#ifndef JSI_MEM_DEBUG
    static int warnNoDebug = 0;
    if (interp->memDebug && warnNoDebug == 0) {
        Jsi_LogWarn("ignoring memDebug as jsi was compiled without memory debugging");
        warnNoDebug = 1;
    }
#endif
    interp->threadId = Jsi_CurrentThread();
    if (interp->parent && interp->subthread==0 && interp->threadId != interp->parent->threadId) {
        interp->threadId = interp->parent->threadId;
#ifndef JSI_MEM_DEBUG
        Jsi_LogWarn("non-threaded sub-interp created by different thread than parent");
#endif
    }
    if (interp->safeMode != jsi_safe_None)
        interp->isSafe = interp->startSafe = 1;
    if (!interp->parent) {
        if (interp->isSafe)
            interp->startSafe = 1;
        if (interp->debugOpts.msgCallback)
            Jsi_LogWarn("ignoring msgCallback");
        if (interp->debugOpts.putsCallback)
            Jsi_LogWarn("ignoring putsCallback");
        if (interp->busyCallback)
            Jsi_LogWarn("ignoring busyCallback");
        if (interp->debugOpts.traceCallback)
            Jsi_LogWarn("ignoring traceCallback");
    } else if (interp->busyCallback && interp->threadId != interp->parent->threadId) {
        Jsi_LogWarn("disabling busyCallback due to threads");
        interp->busyCallback = NULL;
    }
    if (interp == jsiIntData.mainInterp)
        interp->lexkeyTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    else
        interp->lexkeyTbl = jsiIntData.mainInterp->lexkeyTbl;
    interp->thisTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, freeValueTbl);
    interp->userdataTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, freeUserdataTbl);
    interp->varTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    interp->codeTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, freeCodeTbl);
    interp->genValueTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD,freeValueTbl);
    interp->genObjTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, freeGenObjTbl);
#ifdef JSI_MEM_DEBUG
    interp->codesTbl = (interp == jsiIntData.mainInterp ? Jsi_HashNew(interp, JSI_KEYS_ONEWORD, NULL) : jsiIntData.mainInterp->codesTbl);
#endif
    if (interp->typeCheck.all|interp->typeCheck.parse|interp->typeCheck.funcsig)
        interp->staticFuncsTbl = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    if (!jsiIntData.isInit) {
        jsiIntData.isInit = 1;
        jsi_InitValue(interp, 0);
        jsiIntData.interpsTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, 0);
    }

    /* current scope, also global */
    interp->csc = Jsi_ValueNew1(interp);
    Jsi_ValueMakeObject(interp, &interp->csc, Jsi_ObjNew(interp));
    interp->framePtr->incsc = interp->csc;

#define JSIDOINIT(nam) if (!jsi_ModBlacklisted(interp,#nam)) { if (jsi_Init##nam(interp, 0) != JSI_OK) { Jsi_LogBug("Init failure in %s", #nam); } }
#define JSIDOINIT2(nam) if (!jsi_ModBlacklisted(interp,#nam)) { if (Jsi_Init##nam(interp, 0) != JSI_OK) { Jsi_LogBug("Init failure in %s", #nam); } }

    JSIDOINIT(Proto);

    if (interp->pkgDirs) // Fix-up because above, array was not yet initialized.
        interp->pkgDirs->d.obj->__proto__ = interp->Array_prototype;

    Jsi_Value *modObj = Jsi_ValueNewObj(interp, Jsi_ObjNewType(interp, JSI_OT_OBJECT));
    Jsi_ValueInsert(interp, interp->csc, "Jsi_Auto", modObj, JSI_OM_DONTDEL);

    /* initial scope chain, nothing */
    interp->framePtr->ingsc = interp->gsc = jsi_ScopeChainNew(interp, 0);

    interp->ps = jsi_PstateNew(interp); /* Default parser. */
    if (interp->unitTest&2) {
        interp->logOpts.before = 1;
        interp->logOpts.full = 1;
        interp->tracePuts = 1;
        interp->noStderr = 1;
    }
    if (interp->args && argc) {
        Jsi_LogBug("args may not be specified both as options and parameter");
        Jsi_InterpDelete(interp);
        return NULL;
    }
    if (interp->maxDepth>JSI_MAX_EVAL_DEPTH)
        interp->maxDepth = JSI_MAX_EVAL_DEPTH;

    // Create the args array.
    if (argc >= 0 && !interp->args) {
        Jsi_Value *iargs = Jsi_ValueNew1(interp);
        iargs->f.bits.dontdel = 1;
        iargs->f.bits.readonly = 1;
        Jsi_Obj *iobj = Jsi_ObjNew(interp);
        Jsi_ValueMakeArrayObject(interp, &iargs, iobj);
        int i = 1, ii = (iocnt>1 ? iocnt : 1);
        int msiz = (argc?argc-iocnt:0);
        Jsi_ObjArraySizer(interp, iobj, msiz);
        iobj->arrMaxSize = msiz;
        iocnt--;
        iobj->arrCnt = argc-iocnt;
        for (i = 1; ii < argc; ++ii, i++) {
            iobj->arr[i-1] = Jsi_ValueNewStringKey(interp, argv[ii]);
            Jsi_IncrRefCount(interp, iobj->arr[i-1]);
            jsi_ValueDebugLabel(iobj->arr[i-1], "InterpCreate", "args");
        }
        Jsi_ObjSetLength(interp, iobj, msiz);
        interp->args = iargs;
    } else if (interp->parent && interp->args) {
        // Avoid strings from sneeking in with options from parent...
        Jsi_Value *nar = Jsi_ValueDupJSON(interp, interp->args);
        Jsi_DecrRefCount(interp, interp->args);
        interp->args = nar;
    }
    JSIDOINIT(Options);
    JSIDOINIT(Cmds);
    JSIDOINIT(Interp);
    JSIDOINIT(JSON);

    interp->retValue = Jsi_ValueNew1(interp);
    interp->Mutex = Jsi_MutexNew(interp, -1, JSI_MUTEX_RECURSIVE);
    if (1 || interp->subthread) {
        interp->QMutex = Jsi_MutexNew(interp, -1, JSI_MUTEX_RECURSIVE);
        //Jsi_DSInit(&interp->interpEvalQ);
    }
    JSIDOINIT(Lexer);
    if (interp != jsiIntData.mainInterp && !parent)
        Jsi_HashSet(jsiIntData.interpsTbl, interp, NULL);

    if (!interp->isSafe) {
        JSIDOINIT(Load);
#if JSI__SIGNAL==1
        JSIDOINIT(Signal);
#endif
    }
    if (interp->isSafe == 0 || interp->startSafe || interp->safeWriteDirs!=NULL || interp->safeReadDirs!=NULL) {
#if JSI__FILESYS==1
        JSIDOINIT(FileCmds);
        JSIDOINIT(Filesys);
#endif
    }
#if JSI__SQLITE==1
    JSIDOINIT2(Sqlite);
#else
    Jsi_initSqlite(interp, 0);
#endif
#if JSI__MYSQL==1
    if (!interp->noNetwork) {
        JSIDOINIT2(MySql);
    }
#endif
#if JSI__SOCKET==1
    JSIDOINIT2(Socket);
#endif
#if JSI__WEBSOCKET==1
    JSIDOINIT2(WebSocket);
#endif

#if JSI__CDATA==1
    JSIDOINIT(CData);
#endif

#ifdef JSI_USER_EXTENSION
    extern int JSI_USER_EXTENSION(Jsi_Interp *interp, int release);
    if (JSI_USER_EXTENSION (interp, 0) != JSI_OK) {
        fprintf(stderr, "extension load failed");
        return jsi_DoExit(interp, 1);
    }
#endif
    Jsi_PkgProvide(interp, "Jsi", JSI_VERSION, NULL);
    if (argc > 0) {
        char *ss = argv0;
        char epath[PATH_MAX] = ""; // Path of executable
#ifdef __WIN32

        if (GetModuleFileName(NULL, epath, sizeof(epath))>0)
            ss = epath;
#else
#ifndef PROC_SELF_DIR
#define PROC_SELF_DIR "/proc/self/exe"
#endif
        if (ss && *ss != '/' && readlink(PROC_SELF_DIR, epath, sizeof(epath)) && epath[0])
            ss = epath;
#endif
        Jsi_Value *src = Jsi_ValueNewStringDup(interp, ss);
        Jsi_IncrRefCount(interp, src);
        jsiIntData.execName = Jsi_Realpath(interp, src, NULL);
        Jsi_DecrRefCount(interp, src);
        if (!jsiIntData.execName) jsiIntData.execName = Jsi_Strdup("");
        jsiIntData.execValue = Jsi_ValueNewString(interp, jsiIntData.execName, -1);
        Jsi_IncrRefCount(interp, jsiIntData.execValue);
        Jsi_HashSet(interp->genValueTbl, jsiIntData.execValue, jsiIntData.execValue);
    }

    //interp->nocacheOpCodes = 1;
    if (interp->debugOpts.debugCallback && !interp->debugOpts.hook) {
        interp->debugOpts.hook = jsi_InterpDebugHook;
    }
    interp->startTime = jsi_GetTimestamp();
#ifdef JSI_INTERP_EXTENSION_CODE // For extending interp from jsi.c
    JSI_INTERP_EXTENSION_CODE
#endif
    if (interp->opts.initProc && (*interp->opts.initProc)(interp, 0) != JSI_OK)
        Jsi_LogBug("Init failure in initProc");

    return interp;
}
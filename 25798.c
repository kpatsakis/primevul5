static void _printStatus(Output_Type type, HttpResponse res, Service_T s) {
        if (Util_hasServiceStatus(s)) {
                switch (s->type) {
                        case Service_System:
                                _formatStatus("load average", Event_Resource, type, res, s, true, "[%.2f] [%.2f] [%.2f]", systeminfo.loadavg[0], systeminfo.loadavg[1], systeminfo.loadavg[2]);
                                _formatStatus("cpu", Event_Resource, type, res, s, true, "%.1f%%us %.1f%%sy"
#ifdef HAVE_CPU_WAIT
                                        " %.1f%%wa"
#endif
                                        , systeminfo.cpu.usage.user > 0. ? systeminfo.cpu.usage.user : 0., systeminfo.cpu.usage.system > 0. ? systeminfo.cpu.usage.system : 0.
#ifdef HAVE_CPU_WAIT
                                        , systeminfo.cpu.usage.wait > 0. ? systeminfo.cpu.usage.wait : 0.
#endif
                                );
                                _formatStatus("memory usage", Event_Resource, type, res, s, true, "%s [%.1f%%]", Fmt_bytes2str(systeminfo.memory.usage.bytes, (char[10]){}), systeminfo.memory.usage.percent);
                                _formatStatus("swap usage", Event_Resource, type, res, s, true, "%s [%.1f%%]", Fmt_bytes2str(systeminfo.swap.usage.bytes, (char[10]){}), systeminfo.swap.usage.percent);
                                _formatStatus("uptime", Event_Uptime, type, res, s, systeminfo.booted > 0, "%s", _getUptime(Time_now() - systeminfo.booted, (char[256]){}));
                                _formatStatus("boot time", Event_Null, type, res, s, true, "%s", Time_string(systeminfo.booted, (char[32]){}));
                                break;

                        case Service_File:
                                _formatStatus("permission", Event_Permission, type, res, s, s->inf.file->mode >= 0, "%o", s->inf.file->mode & 07777);
                                _formatStatus("uid", Event_Uid, type, res, s, s->inf.file->uid >= 0, "%d", s->inf.file->uid);
                                _formatStatus("gid", Event_Gid, type, res, s, s->inf.file->gid >= 0, "%d", s->inf.file->gid);
                                _formatStatus("size", Event_Size, type, res, s, s->inf.file->size >= 0, "%s", Fmt_bytes2str(s->inf.file->size, (char[10]){}));
                                _formatStatus("access timestamp", Event_Timestamp, type, res, s, s->inf.file->timestamp.access > 0, "%s", Time_string(s->inf.file->timestamp.access, (char[32]){}));
                                _formatStatus("change timestamp", Event_Timestamp, type, res, s, s->inf.file->timestamp.change > 0, "%s", Time_string(s->inf.file->timestamp.change, (char[32]){}));
                                _formatStatus("modify timestamp", Event_Timestamp, type, res, s, s->inf.file->timestamp.modify > 0, "%s", Time_string(s->inf.file->timestamp.modify, (char[32]){}));
                                if (s->matchlist)
                                        _formatStatus("content match", Event_Content, type, res, s, true, "%s", (s->error & Event_Content) ? "yes" : "no");
                                if (s->checksum)
                                        _formatStatus("checksum", Event_Checksum, type, res, s, *s->inf.file->cs_sum, "%s (%s)", s->inf.file->cs_sum, checksumnames[s->checksum->type]);
                                break;

                        case Service_Directory:
                                _formatStatus("permission", Event_Permission, type, res, s, s->inf.directory->mode >= 0, "%o", s->inf.directory->mode & 07777);
                                _formatStatus("uid", Event_Uid, type, res, s, s->inf.directory->uid >= 0, "%d", s->inf.directory->uid);
                                _formatStatus("gid", Event_Gid, type, res, s, s->inf.directory->gid >= 0, "%d", s->inf.directory->gid);
                                _formatStatus("access timestamp", Event_Timestamp, type, res, s, s->inf.directory->timestamp.access > 0, "%s", Time_string(s->inf.directory->timestamp.access, (char[32]){}));
                                _formatStatus("change timestamp", Event_Timestamp, type, res, s, s->inf.directory->timestamp.change > 0, "%s", Time_string(s->inf.directory->timestamp.change, (char[32]){}));
                                _formatStatus("modify timestamp", Event_Timestamp, type, res, s, s->inf.directory->timestamp.modify > 0, "%s", Time_string(s->inf.directory->timestamp.modify, (char[32]){}));
                                break;

                        case Service_Fifo:
                                _formatStatus("permission", Event_Permission, type, res, s, s->inf.fifo->mode >= 0, "%o", s->inf.fifo->mode & 07777);
                                _formatStatus("uid", Event_Uid, type, res, s, s->inf.fifo->uid >= 0, "%d", s->inf.fifo->uid);
                                _formatStatus("gid", Event_Gid, type, res, s, s->inf.fifo->gid >= 0, "%d", s->inf.fifo->gid);
                                _formatStatus("access timestamp", Event_Timestamp, type, res, s, s->inf.fifo->timestamp.access > 0, "%s", Time_string(s->inf.fifo->timestamp.access, (char[32]){}));
                                _formatStatus("change timestamp", Event_Timestamp, type, res, s, s->inf.fifo->timestamp.change > 0, "%s", Time_string(s->inf.fifo->timestamp.change, (char[32]){}));
                                _formatStatus("modify timestamp", Event_Timestamp, type, res, s, s->inf.fifo->timestamp.modify > 0, "%s", Time_string(s->inf.fifo->timestamp.modify, (char[32]){}));
                                break;

                        case Service_Net:
                                {
                                        long long speed = Link_getSpeed(s->inf.net->stats);
                                        long long ibytes = Link_getBytesInPerSecond(s->inf.net->stats);
                                        long long obytes = Link_getBytesOutPerSecond(s->inf.net->stats);
                                        _formatStatus("link", Event_Link, type, res, s, Link_getState(s->inf.net->stats) == 1, "%d errors", Link_getErrorsInPerSecond(s->inf.net->stats) + Link_getErrorsOutPerSecond(s->inf.net->stats));
                                        if (speed > 0) {
                                                _formatStatus("capacity", Event_Speed, type, res, s, Link_getState(s->inf.net->stats) == 1, "%.0lf Mb/s %s-duplex", (double)speed / 1000000., Link_getDuplex(s->inf.net->stats) == 1 ? "full" : "half");
                                                _formatStatus("download bytes", Event_ByteIn, type, res, s, Link_getState(s->inf.net->stats) == 1, "%s/s (%.1f%% link saturation)", Fmt_bytes2str(ibytes, (char[10]){}), 100. * ibytes * 8 / (double)speed);
                                                _formatStatus("upload bytes", Event_ByteOut, type, res, s, Link_getState(s->inf.net->stats) == 1, "%s/s (%.1f%% link saturation)", Fmt_bytes2str(obytes, (char[10]){}), 100. * obytes * 8 / (double)speed);
                                        } else {
                                                _formatStatus("download bytes", Event_ByteIn, type, res, s, Link_getState(s->inf.net->stats) == 1, "%s/s", Fmt_bytes2str(ibytes, (char[10]){}));
                                                _formatStatus("upload bytes", Event_ByteOut, type, res, s, Link_getState(s->inf.net->stats) == 1, "%s/s", Fmt_bytes2str(obytes, (char[10]){}));
                                        }
                                        _formatStatus("download packets", Event_PacketIn, type, res, s, Link_getState(s->inf.net->stats) == 1, "%lld per second", Link_getPacketsInPerSecond(s->inf.net->stats));
                                        _formatStatus("upload packets", Event_PacketOut, type, res, s, Link_getState(s->inf.net->stats) == 1, "%lld per second", Link_getPacketsOutPerSecond(s->inf.net->stats));
                                }
                                break;

                        case Service_Filesystem:
                                _formatStatus("filesystem type", Event_Null, type, res, s, *(s->inf.filesystem->object.type), "%s", s->inf.filesystem->object.type);
                                _formatStatus("filesystem flags", Event_FsFlag, type, res, s, *(s->inf.filesystem->flags), "%s", s->inf.filesystem->flags);
                                _formatStatus("permission", Event_Permission, type, res, s, s->inf.filesystem->mode >= 0, "%o", s->inf.filesystem->mode & 07777);
                                _formatStatus("uid", Event_Uid, type, res, s, s->inf.filesystem->uid >= 0, "%d", s->inf.filesystem->uid);
                                _formatStatus("gid", Event_Gid, type, res, s, s->inf.filesystem->gid >= 0, "%d", s->inf.filesystem->gid);
                                _formatStatus("block size", Event_Null, type, res, s, true, "%s", Fmt_bytes2str(s->inf.filesystem->f_bsize, (char[10]){}));
                                _formatStatus("space total", Event_Null, type, res, s, true, "%s (of which %.1f%% is reserved for root user)",
                                        s->inf.filesystem->f_bsize > 0 ? Fmt_bytes2str(s->inf.filesystem->f_blocks * s->inf.filesystem->f_bsize, (char[10]){}) : "0 MB",
                                        s->inf.filesystem->f_blocks > 0 ? ((float)100 * (float)(s->inf.filesystem->f_blocksfreetotal - s->inf.filesystem->f_blocksfree) / (float)s->inf.filesystem->f_blocks) : 0);
                                _formatStatus("space free for non superuser", Event_Null, type, res, s, true, "%s [%.1f%%]",
                                        s->inf.filesystem->f_bsize > 0 ? Fmt_bytes2str(s->inf.filesystem->f_blocksfree * s->inf.filesystem->f_bsize, (char[10]){}) : "0 MB",
                                        s->inf.filesystem->f_blocks > 0 ? ((float)100 * (float)s->inf.filesystem->f_blocksfree / (float)s->inf.filesystem->f_blocks) : 0);
                                _formatStatus("space free total", Event_Resource, type, res, s, true, "%s [%.1f%%]",
                                        s->inf.filesystem->f_bsize > 0 ? Fmt_bytes2str(s->inf.filesystem->f_blocksfreetotal * s->inf.filesystem->f_bsize, (char[10]){}) : "0 MB",
                                        s->inf.filesystem->f_blocks > 0 ? ((float)100 * (float)s->inf.filesystem->f_blocksfreetotal / (float)s->inf.filesystem->f_blocks) : 0);
                                if (s->inf.filesystem->f_files > 0) {
                                        _formatStatus("inodes total", Event_Null, type, res, s, true, "%lld", s->inf.filesystem->f_files);
                                        _formatStatus("inodes free", Event_Resource, type, res, s, true, "%lld [%.1f%%]", s->inf.filesystem->f_filesfree, (float)100 * (float)s->inf.filesystem->f_filesfree / (float)s->inf.filesystem->f_files);
                                }
                                _printIOStatistics(type, res, s, &(s->inf.filesystem->read), "read", "read");
                                _printIOStatistics(type, res, s, &(s->inf.filesystem->write), "write", "write");
                                boolean_t hasReadTime = Statistics_initialized(&(s->inf.filesystem->time.read));
                                boolean_t hasWriteTime = Statistics_initialized(&(s->inf.filesystem->time.write));
                                boolean_t hasWaitTime = Statistics_initialized(&(s->inf.filesystem->time.wait));
                                boolean_t hasRunTime = Statistics_initialized(&(s->inf.filesystem->time.run));
                                double deltaOperations = Statistics_delta(&(s->inf.filesystem->read.operations)) + Statistics_delta(&(s->inf.filesystem->write.operations));
                                if (hasReadTime && hasWriteTime) {
                                        double readTime = deltaOperations > 0. ? Statistics_deltaNormalize(&(s->inf.filesystem->time.read)) / deltaOperations : 0.;
                                        double writeTime = deltaOperations > 0. ? Statistics_deltaNormalize(&(s->inf.filesystem->time.write)) / deltaOperations : 0.;
                                        _formatStatus("service time", Event_Null, type, res, s, true, "%.3fms/operation (of which read %.3fms, write %.3fms)", readTime + writeTime, readTime, writeTime);
                                } else if (hasWaitTime && hasRunTime) {
                                        double waitTime = deltaOperations > 0. ? Statistics_deltaNormalize(&(s->inf.filesystem->time.wait)) / deltaOperations : 0.;
                                        double runTime = deltaOperations > 0. ? Statistics_deltaNormalize(&(s->inf.filesystem->time.run)) / deltaOperations : 0.;
                                        _formatStatus("service time", Event_Null, type, res, s, true, "%.3fms/operation (of which queue %.3fms, active %.3fms)", waitTime + runTime, waitTime, runTime);
                                } else if (hasWaitTime) {
                                        double waitTime = deltaOperations > 0. ? Statistics_deltaNormalize(&(s->inf.filesystem->time.wait)) / deltaOperations : 0.;
                                        _formatStatus("service time", Event_Null, type, res, s, true, "%.3fms/operation", waitTime);
                                } else if (hasRunTime) {
                                        double runTime = deltaOperations > 0. ? Statistics_deltaNormalize(&(s->inf.filesystem->time.run)) / deltaOperations : 0.;
                                        _formatStatus("service time", Event_Null, type, res, s, true, "%.3fms/operation", runTime);
                                }
                                break;

                        case Service_Process:
                                _formatStatus("pid", Event_Pid, type, res, s, s->inf.process->pid >= 0, "%d", s->inf.process->pid);
                                _formatStatus("parent pid", Event_PPid, type, res, s, s->inf.process->ppid >= 0, "%d", s->inf.process->ppid);
                                _formatStatus("uid", Event_Uid, type, res, s, s->inf.process->uid >= 0, "%d", s->inf.process->uid);
                                _formatStatus("effective uid", Event_Uid, type, res, s, s->inf.process->euid >= 0, "%d", s->inf.process->euid);
                                _formatStatus("gid", Event_Gid, type, res, s, s->inf.process->gid >= 0, "%d", s->inf.process->gid);
                                _formatStatus("uptime", Event_Uptime, type, res, s, s->inf.process->uptime >= 0, "%s", _getUptime(s->inf.process->uptime, (char[256]){}));
                                if (Run.flags & Run_ProcessEngineEnabled) {
                                        _formatStatus("threads", Event_Resource, type, res, s, s->inf.process->threads >= 0, "%d", s->inf.process->threads);
                                        _formatStatus("children", Event_Resource, type, res, s, s->inf.process->children >= 0, "%d", s->inf.process->children);
                                        _formatStatus("cpu", Event_Resource, type, res, s, s->inf.process->cpu_percent >= 0, "%.1f%%", s->inf.process->cpu_percent);
                                        _formatStatus("cpu total", Event_Resource, type, res, s, s->inf.process->total_cpu_percent >= 0, "%.1f%%", s->inf.process->total_cpu_percent);
                                        _formatStatus("memory", Event_Resource, type, res, s, s->inf.process->mem_percent >= 0, "%.1f%% [%s]", s->inf.process->mem_percent, Fmt_bytes2str(s->inf.process->mem, (char[10]){}));
                                        _formatStatus("memory total", Event_Resource, type, res, s, s->inf.process->total_mem_percent >= 0, "%.1f%% [%s]", s->inf.process->total_mem_percent, Fmt_bytes2str(s->inf.process->total_mem, (char[10]){}));
#ifdef LINUX
                                        _formatStatus("security attribute", Event_Invalid, type, res, s, *(s->inf.process->secattr), "%s", s->inf.process->secattr);
#endif
                                }
                                _printIOStatistics(type, res, s, &(s->inf.process->read), "disk read", "read");
                                _printIOStatistics(type, res, s, &(s->inf.process->write), "disk write", "write");
                                break;

                        case Service_Program:
                                if (s->program->started) {
                                        _formatStatus("last exit value", Event_Status, type, res, s, true, "%d", s->program->exitStatus);
                                        _formatStatus("last output", Event_Status, type, res, s, StringBuffer_length(s->program->lastOutput), "%s", StringBuffer_toString(s->program->lastOutput));
                                }
                                break;

                        default:
                                break;
                }
                for (Icmp_T i = s->icmplist; i; i = i->next) {
                        if (i->is_available == Connection_Failed)
                                _formatStatus("ping response time", Event_Icmp, type, res, s, true, "connection failed");
                        else
                                _formatStatus("ping response time", Event_Null, type, res, s, i->is_available != Connection_Init && i->response >= 0., "%s", Fmt_time2str(i->response, (char[11]){}));
                }
                for (Port_T p = s->portlist; p; p = p->next) {
                        if (p->is_available == Connection_Failed) {
                                _formatStatus("port response time", Event_Connection, type, res, s, true, "FAILED to [%s]:%d%s type %s/%s %sprotocol %s", p->hostname, p->target.net.port, Util_portRequestDescription(p), Util_portTypeDescription(p), Util_portIpDescription(p), p->target.net.ssl.options.flags ? "using TLS " : "", p->protocol->name);
                        } else {
                                char buf[STRLEN] = {};
                                if (p->target.net.ssl.options.flags)
                                        snprintf(buf, sizeof(buf), "using TLS (certificate valid for %d days) ", p->target.net.ssl.certificate.validDays);
                                _formatStatus("port response time", p->target.net.ssl.certificate.validDays < p->target.net.ssl.certificate.minimumDays ? Event_Timestamp : Event_Null, type, res, s, p->is_available != Connection_Init, "%s to %s:%d%s type %s/%s %sprotocol %s", Fmt_time2str(p->response, (char[11]){}), p->hostname, p->target.net.port, Util_portRequestDescription(p), Util_portTypeDescription(p), Util_portIpDescription(p), buf, p->protocol->name);
                        }
                }
                for (Port_T p = s->socketlist; p; p = p->next) {
                        if (p->is_available == Connection_Failed) {
                                _formatStatus("unix socket response time", Event_Connection, type, res, s, true, "FAILED to %s type %s protocol %s", p->target.unix.pathname, Util_portTypeDescription(p), p->protocol->name);
                        } else {
                                _formatStatus("unix socket response time", Event_Null, type, res, s, p->is_available != Connection_Init, "%s to %s type %s protocol %s", Fmt_time2str(p->response, (char[11]){}), p->target.unix.pathname, Util_portTypeDescription(p), p->protocol->name);
                        }
                }
        }
        _formatStatus("data collected", Event_Null, type, res, s, true, "%s", Time_string(s->collected.tv_sec, (char[32]){}));
}
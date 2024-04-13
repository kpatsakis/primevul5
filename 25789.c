static void _printIOStatistics(Output_Type type, HttpResponse res, Service_T s, IOStatistics_T io, const char *header, const char *name) {
        boolean_t hasOps = Statistics_initialized(&(io->operations));
        boolean_t hasBytes = Statistics_initialized(&(io->bytes));
        if (hasOps && hasBytes) {
                double deltaBytesPerSec = Statistics_deltaNormalize(&(io->bytes));
                double deltaOpsPerSec = Statistics_deltaNormalize(&(io->operations));
                _formatStatus(header, Event_Resource, type, res, s, true, "%s/s [%s total], %.1f %ss/s [%"PRIu64" %ss total]", Fmt_bytes2str(deltaBytesPerSec, (char[10]){}), Fmt_bytes2str(Statistics_raw(&(io->bytes)), (char[10]){}), deltaOpsPerSec, name, Statistics_raw(&(io->operations)), name);
        } else if (hasOps) {
                double deltaOpsPerSec = Statistics_deltaNormalize(&(io->operations));
                _formatStatus(header, Event_Resource, type, res, s, true, "%.1f %ss/s [%"PRIu64" %ss total]", deltaOpsPerSec, name, Statistics_raw(&(io->operations)), name);
        } else if (hasBytes) {
                double deltaBytesPerSec = Statistics_deltaNormalize(&(io->bytes));
                _formatStatus(header, Event_Resource, type, res, s, true, "%s/s [%s total]", Fmt_bytes2str(deltaBytesPerSec, (char[10]){}), Fmt_bytes2str(Statistics_raw(&(io->bytes)), (char[10]){}));
        }
}
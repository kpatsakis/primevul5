    void write(OutputStream& stream) const
    {
        stream.write_or_error({ end_of_central_directory_signature, sizeof(end_of_central_directory_signature) });
        stream << disk_number;
        stream << central_directory_start_disk;
        stream << disk_records_count;
        stream << total_records_count;
        stream << central_directory_size;
        stream << central_directory_offset;
        stream << comment_length;
        if (comment_length > 0)
            stream.write_or_error({ comment, comment_length });
    }
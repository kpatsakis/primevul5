    void write(OutputStream& stream) const
    {
        stream.write_or_error({ central_directory_record_signature, sizeof(central_directory_record_signature) });
        stream << made_by_version;
        stream << minimum_version;
        stream << general_purpose_flags;
        stream << compression_method;
        stream << modification_time;
        stream << modification_date;
        stream << crc32;
        stream << compressed_size;
        stream << uncompressed_size;
        stream << name_length;
        stream << extra_data_length;
        stream << comment_length;
        stream << start_disk;
        stream << internal_attributes;
        stream << external_attributes;
        stream << local_file_header_offset;
        if (name_length > 0)
            stream.write_or_error({ name, name_length });
        if (extra_data_length > 0)
            stream.write_or_error({ extra_data, extra_data_length });
        if (comment_length > 0)
            stream.write_or_error({ comment, comment_length });
    }
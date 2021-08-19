#include <ruby.h>
#include <ruby/version.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>

struct iseqc_header
{
    uint32_t mri_version;
    uint32_t page_size;
    uint32_t iseq_start;
    uint8_t file_crc32;
} __attribute__((packed));

const size_t iseqc_header_size = sizeof(struct iseqc_header);
const size_t iseqc_header_iseq_start = offsetof(struct iseqc_header, iseq_start);

typedef struct iseqc_header iseqc_header_t;

static inline int host_mri_version(void)
{
    return RUBY_API_VERSION_CODE;
}

static inline int host_page_size(void)
{
    return getpagesize();
}

static VALUE rb_host_page_size(VALUE self)
{
    return INT2NUM(host_page_size());
}

static inline uint32_t file_crc32(const uint8_t *bytes, const size_t len)
{
    return crc32(0, bytes, len) & 0xffffffff;
}

static inline void iseqc_new_package(const char *outfile)
{
    FILE *fp = fopen(outfile, "wb");
    fclose(fp);
}

/* Add a new 128 bits header to the beginning of the buffer */
static inline void iseqc_new_header(const char *outfile, uint32_t iseq_start)
{
    FILE *fp = fopen(outfile, "rb+");
    rewind(fp);

    iseqc_header_t header = {
        .mri_version = host_mri_version(),
        .page_size = host_page_size(),
        .iseq_start = iseq_start,
        .file_crc32 = 0,
    };

    fwrite(&header, iseqc_header_size, 1, fp);
    fclose(fp);
}

static inline void iseqc_header_crc32(const char *outfile)
{
    FILE *fp = fopen(outfile, "rb+");

    /* File size */
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    uint8_t *buffer = malloc(sizeof(uint8_t) * fsize);
    fread(buffer, sizeof(uint8_t), fsize, fp);

    /* Save the old header */
    iseqc_header_t header;
    fread(&header, iseqc_header_size, 1, fp);

    /* New header */
    header.file_crc32 = file_crc32(buffer, fsize);
    memcpy(buffer, &header, iseqc_header_size);

    fwrite(buffer, sizeof(uint8_t), fsize, fp);
    fclose(fp);
    free(buffer);
}

static inline int iseqc_index_start(void)
{
    /* At least enable 4k paging */
    return host_page_size();
}

static inline size_t iseqc_new_index(const char *outfile, const uint8_t *index_bin, size_t size)
{
    FILE *fp = fopen(outfile, "rb+");
    fseek(fp, iseqc_index_start(), SEEK_SET);
    fwrite(index_bin, sizeof(uint8_t), size, fp);
    fclose(fp);

    size_t unaligned = (iseqc_index_start() + size) % host_page_size();
    size_t padding = host_page_size() - unaligned;
    size_t iseq_start = size + padding + iseqc_index_start();

    return iseq_start;
}

static inline void iseqc_append_iseq(const char *outfile, const uint8_t *iseq_bin, size_t start, size_t size)
{
    FILE *fp = fopen(outfile, "rb+");

    uint32_t iseq_start = 0;
    fseek(fp, iseqc_header_iseq_start, SEEK_SET);
    fread(&iseq_start, sizeof(uint32_t), 1, fp);

    fseek(fp, iseq_start, SEEK_SET);
    fwrite(iseq_bin, sizeof(uint8_t), size, fp);

    fclose(fp);
}

static VALUE rb_iseqc_new_package(VALUE self, VALUE outfile)
{
    iseqc_new_package(rb_string_value_cstr(&outfile));

    return Qnil;
}

static VALUE rb_iseqc_add_index(VALUE self, VALUE outfile, VALUE index_bin)
{
    const char *outfile_cstr = rb_string_value_cstr(&outfile);
    const uint8_t *index_bin_ptr = (const uint8_t *)rb_string_value_ptr(&index_bin);
    size_t index_bin_size = RSTRING_LEN(index_bin);
    size_t iseq_start = iseqc_new_index(outfile_cstr, index_bin_ptr, index_bin_size);

    iseqc_new_header(outfile_cstr, (uint32_t)iseq_start);

    return Qnil;
}

static VALUE rb_mIseqc;
static VALUE rb_mIseqc_System;
static VALUE rb_mIseqc_Pack;

void Init_iseqc(void)
{
    rb_mIseqc = rb_define_module("Iseqc");
    rb_mIseqc_System = rb_define_module_under(rb_mIseqc, "System");
    rb_mIseqc_Pack = rb_define_module_under(rb_mIseqc, "Pack");

    rb_define_module_function(rb_mIseqc_System, "pagesize", rb_host_page_size, 0);
    rb_define_module_function(rb_mIseqc_Pack, "new_package", rb_iseqc_new_package, 1);
    rb_define_module_function(rb_mIseqc_Pack, "add_index", rb_iseqc_add_index, 2);
}

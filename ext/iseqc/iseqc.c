#include <ruby.h>
#include <ruby/version.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>

struct iseqc_header
{
    uint32_t mri_version;
    uint32_t page_size;
    uint32_t index_size;
    uint32_t iseq_start;
    uint32_t file_crc32;
    uint32_t padding[3];
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

static inline uint32_t file_crc32(const uint8_t *restrict bytes, const size_t len)
{
    return crc32(0, bytes, len) & 0xffffffff;
}

static inline void iseqc_new_package(const char *outfile)
{
    FILE *fp = fopen(outfile, "wb");
    fclose(fp);
}

/* Add a new 256 bits header to the beginning of the buffer */
static inline void iseqc_new_header(const char *restrict outfile, uint32_t index_size, uint32_t iseq_start)
{
    FILE *fp = fopen(outfile, "rb+");
    rewind(fp);

    iseqc_header_t header = {
        .mri_version = host_mri_version(),
        .page_size = host_page_size(),
        .index_size = index_size,
        .iseq_start = iseq_start,
        .file_crc32 = 0,
        .padding = {0, 0, 0}};

    fwrite(&header, iseqc_header_size, 1, fp);
    fclose(fp);
}

static inline void iseqc_header_crc32(const char *restrict outfile)
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

static inline size_t iseqc_new_index(const char *restrict outfile, const uint8_t *restrict index_bin, size_t size)
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

static inline void iseqc_append_iseq(const char *restrict outfile, const uint8_t *restrict iseq_bin, size_t start, size_t size)
{
    FILE *fp = fopen(outfile, "rb+");

    uint32_t iseq_start = 0;
    fseek(fp, iseqc_header_iseq_start, SEEK_SET);
    fread(&iseq_start, sizeof(uint32_t), 1, fp);

    fseek(fp, iseq_start + start, SEEK_SET);
    fwrite(iseq_bin, sizeof(uint8_t), size, fp);

    fclose(fp);
}

static VALUE rb_iseqc_new_package(VALUE self, VALUE outfile)
{
    iseqc_new_package(rb_string_value_ptr(&outfile));

    return Qnil;
}

static VALUE rb_iseqc_add_index(VALUE self, VALUE outfile, VALUE index_bin)
{
    const char *outfile_cstr = rb_string_value_ptr(&outfile);
    const uint8_t *index_bin_ptr = (const uint8_t *)rb_string_value_ptr(&index_bin);
    size_t index_bin_size = RSTRING_LEN(index_bin);
    size_t iseq_start = iseqc_new_index(outfile_cstr, index_bin_ptr, index_bin_size);

    iseqc_new_header(outfile_cstr, (uint32_t)index_bin_size, (uint32_t)iseq_start);

    return Qnil;
}

static VALUE rb_iseqc_append_iseq(VALUE self, VALUE outfile, VALUE iseq_bin, VALUE start)
{
    const char *outfile_cstr = rb_string_value_ptr(&outfile);
    const uint8_t *iseq_bin_ptr = (const uint8_t *)rb_string_value_ptr(&iseq_bin);
    iseqc_append_iseq(outfile_cstr, iseq_bin_ptr, NUM2SIZET(start), RSTRING_LEN(iseq_bin));

    return Qnil;
}

static inline iseqc_header_t *iseqc_load_header(int fd)
{
    iseqc_header_t *header = mmap(NULL, iseqc_header_size, PROT_READ, MAP_PRIVATE, fd, 0);
    return header;
}

static inline uint8_t *iseqc_load_index(int fd, size_t index_size)
{
    uint8_t *index_bin = mmap(NULL, index_size, PROT_READ, MAP_PRIVATE, fd, iseqc_index_start());
    return index_bin;
}

static inline uint8_t *iseqc_load_iseq(int fd, size_t abs_offset, size_t size)
{
    uint8_t *iseq_bin = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, abs_offset);
    return iseq_bin;
}

static VALUE rb_iseqc_load_metadata(VALUE self, VALUE file)
{
    int fd = open(rb_string_value_ptr(&file), O_RDONLY);
    const iseqc_header_t *header = iseqc_load_header(fd);
    const uint8_t *index_bin = iseqc_load_index(fd, header->index_size);
    close(fd);

    VALUE ary = rb_ary_new();
    rb_ary_push(ary, INT2NUM(header->mri_version));
    rb_ary_push(ary, INT2NUM(header->page_size));
    rb_ary_push(ary, INT2NUM(header->index_size));
    rb_ary_push(ary, INT2NUM(header->iseq_start));
    rb_ary_push(ary, rb_external_str_new((const char *)index_bin, header->index_size));

    /* [ mri_version, page_size, index_size, iseq_start, index_bin ] */
    return ary;
}

static VALUE rb_iseqc_load_iseq(VALUE self, VALUE file, VALUE iseq_start, VALUE offset, VALUE size)
{
    int fd = open(rb_string_value_ptr(&file), O_RDONLY);
    const uint8_t *iseq_bin = iseqc_load_iseq(fd, NUM2SIZET(iseq_start) + NUM2SIZET(offset), NUM2SIZET(size));
    close(fd);

    return rb_external_str_new((const char *)iseq_bin, NUM2SIZET(size));
}

static VALUE rb_mIseqc;
static VALUE rb_mIseqc_System;
static VALUE rb_mIseqc_Pack;
static VALUE rb_mIseqc_Unpack;

void Init_iseqc(void)
{
    rb_mIseqc = rb_define_module("Iseqc");
    rb_mIseqc_System = rb_define_module_under(rb_mIseqc, "System");
    rb_mIseqc_Pack = rb_define_module_under(rb_mIseqc, "Pack");
    rb_mIseqc_Unpack = rb_define_module_under(rb_mIseqc, "Unpack");

    rb_define_module_function(rb_mIseqc_System, "pagesize", rb_host_page_size, 0);
    rb_define_module_function(rb_mIseqc_Pack, "new_package", rb_iseqc_new_package, 1);
    rb_define_module_function(rb_mIseqc_Pack, "add_index", rb_iseqc_add_index, 2);
    rb_define_module_function(rb_mIseqc_Pack, "append_iseq", rb_iseqc_append_iseq, 3);
    rb_define_module_function(rb_mIseqc_Unpack, "load_metadata", rb_iseqc_load_metadata, 1);
    rb_define_module_function(rb_mIseqc_Unpack, "load_iseq", rb_iseqc_load_iseq, 4);
}

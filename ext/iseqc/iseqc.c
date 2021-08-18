#include <ruby.h>
#include <ruby/version.h>
#include <stdint.h>
#include <unistd.h>
#include <zlib.h>

struct iseqc_header
{
    uint32_t mri_version;
    uint32_t page_size;
    uint32_t iseq_start;
    uint8_t  file_crc32;
} __attribute__((packed));

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

/* Add a new 128 bits header to the beginning of the buffer */
static inline int iseqc_new_header(uint8_t *buf, uint32_t iseq_start)
{

}

static inline int iseqc_new_index(uint8_t* buf, const char* iseq_str)
{

}

static inline int iseqc_append_iseq(uint8_t* buf, const char* iseq_str)
{

}


static VALUE rb_mIseqc;
static VALUE rb_mIseqc_System;

void Init_iseqc(void)
{
    rb_mIseqc = rb_define_module("Iseqc");
    rb_mIseqc_System = rb_define_module_under(rb_mIseqc, "System");

    rb_define_module_function(rb_mIseqc_System, "pagesize", rb_host_page_size, 0);
}

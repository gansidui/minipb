#ifndef __PB_CODEC_H__
#define __PB_CODEC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	PB_WT_VARINT = 0,
	PB_WT_64BIT  = 1,
	PB_WT_STRING = 2,
	PB_WT_32BIT  = 5,
} pb_wire_type_t;

typedef struct {
	uint8_t  *buf;
	uint32_t buf_cap;
	uint32_t offset;
} pb_buffer_t;

// interface function
bool pb_encode_varint(pb_buffer_t *pb_buf, uint32_t field_number, uint64_t value);
bool pb_encode_string(pb_buffer_t *pb_buf, uint32_t field_number, const uint8_t *src, uint32_t n);

bool pb_decode_uint32(pb_buffer_t *pb_buf, void *dst);
bool pb_decode_uint64(pb_buffer_t *pb_buf, void *dst);
bool pb_decode_string(pb_buffer_t *pb_buf, uint8_t *dst, uint32_t dst_cap, uint32_t *n);
bool pb_decode_submsg(pb_buffer_t *pb_buf, pb_buffer_t *sub_pb_buf);

// helper function
bool pb_write(pb_buffer_t *pb_buf, const uint8_t *src, uint32_t n);
bool pb_read(pb_buffer_t *pb_buf, uint8_t *dst, uint32_t n);

bool pb_encode_tag(pb_buffer_t *pb_buf, uint32_t field_number, pb_wire_type_t wire_type);
bool pb_encode_varint_with_no_field(pb_buffer_t *pb_buf, uint64_t value);

bool pb_decode_varint(pb_buffer_t *pb_buf, void *dst, uint32_t data_len);
bool pb_decode_tag(pb_buffer_t *pb_buf, uint32_t *field_number, pb_wire_type_t *wire_type, bool *eof);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PB_CODEC_H__ */
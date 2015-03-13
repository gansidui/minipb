#include <string.h>
#include "pb_codec.h"

bool pb_encode_varint(pb_buffer_t *pb_buf, uint32_t field_number, uint64_t value) {
	if (!pb_encode_tag(pb_buf, field_number, PB_WT_VARINT)) {
		return false;
	}
	return pb_encode_varint_with_no_field(pb_buf, value);
}

bool pb_encode_string(pb_buffer_t *pb_buf, uint32_t field_number, const uint8_t *src, uint32_t n) {
	if (!pb_encode_tag(pb_buf, field_number, PB_WT_STRING)) {
		return false;
	}
	if (!pb_encode_varint_with_no_field(pb_buf, (uint64_t)n)) {
		return false;
	}
	return pb_write(pb_buf, src, n);
}

bool pb_decode_uint32(pb_buffer_t *pb_buf, void *dst) {
	return pb_decode_varint(pb_buf, dst, sizeof(uint32_t));
}

bool pb_decode_uint64(pb_buffer_t *pb_buf, void *dst) {
	return pb_decode_varint(pb_buf, dst, sizeof(uint64_t));
}

bool pb_decode_string(pb_buffer_t *pb_buf, uint8_t *dst, uint32_t dst_size, uint32_t *n) {

}

bool pb_decode_submsg(pb_buffer_t *pb_buf, pb_buffer_t *sub_pb_buf) {

}

bool pb_write(pb_buffer_t *pb_buf, const uint8_t *src, uint32_t n) {
	if (pb_buf->buf_cap < pb_buf->offset + n) {
		return false;
	}
	if (src) {
		memcpy(pb_buf->buf + pb_buf->offset, src, n);
	}
	pb_buf->offset += n;

	return true;
}

bool pb_read(pb_buffer_t *pb_buf, uint8_t *dst, uint32_t n) {
	if (pb_buf->buf_cap < pb_buf->offset + n) {
		return false;
	}

	memcpy(dst, pb_buf->buf + pb_buf->offset, n);
	pb_buf->offset += n;

	return true;
}

bool pb_encode_tag(pb_buffer_t *pb_buf, uint32_t field_number, pb_wire_type_t wire_type) {
	uint64_t tag = ((uint64_t)field_number << 3) | wire_type;
	return pb_encode_varint_with_no_field(pb_buf, tag);
}

bool pb_encode_varint_with_no_field(pb_buffer_t *pb_buf, uint64_t value) {
	uint8_t buf[10] = {0};
	uint8_t i = 0;

	if (0 == value) {
		return pb_write(pb_buf, (uint8_t*)&value, 1);
	}

	while (value) {
		buf[i++] = (uint8_t)((value & 0x7F) | 0x80);
		value >>= 7;
	}
	buf[i-1] &= 0x7F;

	return pb_write(pb_buf, buf, i);
}

bool pb_decode_varint(pb_buffer_t *pb_buf, void *dst, uint32_t data_len) {
	uint64_t value = 0;
	uint8_t i = 0;
	uint8_t byte;

	do {
		if (i >= 64) {
			return false;
		}
		if (!pb_read(pb_buf, &byte, 1)) {
			return false;
		}

		value |= (uint64_t)(byte & 0x7F) << i;
		i += 7;
	} while (byte & 0x80);

	switch (data_len) {
		case 4: *(uint32_t*)dst = (uint32_t)value; break;
		case 8: *(uint64_t*)dst = value; break;
		default: return false;
	}

	return true;
}

bool pb_decode_tag(pb_buffer_t *pb_buf, uint32_t *field_number, pb_wire_type_t *wire_type, bool *eof) {
	uint32_t temp;
	*field_number = 0;
	*wire_type = (pb_wire_type_t) 0;
	*eof = false;
	if (!pb_decode_uint32(pb_buf, &temp)) {
		if (pb_buf->offset >= pb_buf->buf_cap) {
			*eof = true;
		}
		return false;
	}
	if (0 == temp) {
		*eof = true;
		return false;
	}

	*field_number = temp >> 3;
	*wire_type = (pb_wire_type_t)(temp & 0x7);

	return true;
}
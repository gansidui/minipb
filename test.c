#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pb_codec.h"

/*
message msg
{
	optional uint64 uint64_uin = 2;
	optional string str_body   = 5;
}

message example
{
	required uint32 uint32_num   = 1;
	optional string str_content  = 2;
	repeated uint32 uint32_id    = 3;
	repeated body   msg_body     = 4;
}
*/

typedef struct {
	uint64_t uin;
	uint8_t  body[50];
	uint32_t body_len;
} msg_t;

typedef struct {
	uint32_t num;
	uint8_t  content[50];
	uint32_t content_len;
	uint32_t id[20];
	uint32_t id_len;
	msg_t    msg[20];
	uint32_t msg_len;
} example_t;

bool pb_encode_example(const example_t *src, pb_buffer_t *enc_pb_buf) {
	uint32_t i = 0;

	if (!pb_encode_varint(enc_pb_buf, 1, src->num)) return false;
	if (!pb_encode_string(enc_pb_buf, 2, src->content, src->content_len)) return false;
	for (i = 0; i < src->id_len; ++i) {
		if (!pb_encode_varint(enc_pb_buf, 3, src->id[i])) return false;
	}
	for (i = 0; i < src->msg_len; ++i) {
		ENCODE_SUB_PB_MSG_BEGIN(enc_pb_buf, 4)
		if (!pb_encode_varint(enc_pb_buf, 2, src->msg[i].uin)) return false;
		if (!pb_encode_string(enc_pb_buf, 5, src->msg[i].body, src->msg[i].body_len)) return false;
		ENCODE_SUB_PB_MSG_END(enc_pb_buf)
	}

	return true;
}

bool pb_decode_example(pb_buffer_t *dec_pb_buf, example_t *dst) {
	uint32_t id_i = 0;
	uint32_t msg_i = 0;
	pb_buffer_t sub_pb_buf;
	pb_buffer_t *psub_pb_buf = &sub_pb_buf;

	DECODE_PB_MSG_BEGIN(dec_pb_buf)      // begin decode example_t

	DECODE_PB_MSG_FIELD_BEGIN(dec_pb_buf, 1)
	if (!pb_decode_uint32(dec_pb_buf, &dst->num)) return false;
	DECODE_PB_MSG_FIELD_END

	DECODE_PB_MSG_FIELD_BEGIN(dec_pb_buf, 2)
	if (!pb_decode_string(dec_pb_buf, dst->content, sizeof(dst->content), &dst->content_len)) return false;
	DECODE_PB_MSG_FIELD_END

	DECODE_PB_MSG_FIELD_BEGIN(dec_pb_buf, 3)
	if (!pb_decode_uint32(dec_pb_buf, &dst->id[id_i])) return false;
	++id_i;
	DECODE_PB_MSG_FIELD_END

	DECODE_PB_MSG_FIELD_BEGIN(dec_pb_buf, 4)   // begin decode msg_t[]
	if (!pb_decode_submsg(dec_pb_buf, psub_pb_buf)) return false;

	DECODE_PB_MSG_BEGIN(psub_pb_buf)  // begin decode msg_t

	DECODE_PB_MSG_FIELD_BEGIN(psub_pb_buf, 2)
	if (!pb_decode_uint64(psub_pb_buf, &dst->msg[msg_i].uin)) return false;
	DECODE_PB_MSG_FIELD_END

	DECODE_PB_MSG_FIELD_BEGIN(psub_pb_buf, 5)
	if (!pb_decode_string(psub_pb_buf, dst->msg[msg_i].body, sizeof(dst->msg[msg_i].body), &dst->msg[msg_i].body_len)) return false;
	++msg_i;
	DECODE_PB_MSG_FIELD_END

	DECODE_PB_MSG_END(psub_pb_buf)  // end decode msg_t

	DECODE_PB_MSG_FIELD_END         // end decode msg_t[]

	DECODE_PB_MSG_END(dec_pb_buf)  // end decode example_t

	dst->id_len = id_i;
	dst->msg_len = msg_i;

	return true;
}

void print_buf(pb_buffer_t *pb_buf) {
	printf("buf_cap: %d, offset: %d\n", pb_buf->buf_cap, pb_buf->offset);
	uint32_t i = 0;
	for (i = 0; i < pb_buf->offset; ++i) {
		printf("%d,", pb_buf->buf[i]);
	}
	puts("");
}

int main() {
	// encode
	example_t src;
	pb_buffer_t enc_pb_buf;
	uint8_t out_buf[200];
	enc_pb_buf.buf = out_buf;
	enc_pb_buf.buf_cap = sizeof(out_buf);
	enc_pb_buf.offset = 0;

	src.num = 10;
	src.content_len = 5; memcpy(src.content, "abcde", 5);
	
	src.id[0] = 3, src.id[1] = 4; src.id_len = 2;
	src.msg[0].uin = 88; src.msg[0].body_len = 6; memcpy(src.msg[0].body, "aabbcc", 6);
	src.msg[1].uin = 99, src.msg[1].body_len = 6; memcpy(src.msg[1].body, "ddeeff", 6);
	src.msg_len = 2;

	bool ret = pb_encode_example(&src, &enc_pb_buf);

	printf("\nencode ret = %d\n", ret);
	print_buf(&enc_pb_buf);


	// decode
	example_t dst;
	pb_buffer_t dec_pb_buf;
	dec_pb_buf.buf = enc_pb_buf.buf;
	dec_pb_buf.buf_cap = enc_pb_buf.offset;
	dec_pb_buf.offset = 0;

	ret = pb_decode_example(&dec_pb_buf, &dst);
	printf("\ndecode ret = %d\n", ret);
	printf("num[%d] content_len[%d] id_len[%d] msg_len[%d]\n", dst.num, dst.content_len, dst.id_len, dst.msg_len);
	
	uint32_t i = 0;
	for (i = 0; i < dst.content_len; ++i) {
		printf("%c,", dst.content[i]);
	}
	puts("");
	
	for (i = 0; i < dst.id_len; ++i) {
		printf("%d,", dst.id[i]);
	}
	puts("");

	for (i = 0; i < dst.msg_len; ++i) {
		printf("uin[%lld]\n", dst.msg[i].uin);
		uint32_t j = 0;
		for (j = 0; j < dst.msg[i].body_len; ++j) {
			printf("%c ", dst.msg[i].body[j]);
		}
		puts("");
	}

	return 0;
}
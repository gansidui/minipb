# minipb
A mini codec for protobuf. Implemented in C.

https://developers.google.com/protocol-buffers/


### example:

``` C
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pb_codec.h"

/*
message msg
{
	optional uint32 uint32_id = 1;
	optional string str_name  = 2;
}
*/

typedef struct {
	uint32_t id;
	uint8_t  name[30];
	uint32_t name_len;
} msg_t;

bool pb_encode_msg(const msg_t *msg, pb_buffer_t *enc_pb_buf) {
	if (!pb_encode_varint(enc_pb_buf, 1, msg->id)) return false;
	if (!pb_encode_string(enc_pb_buf, 2, msg->name, msg->name_len)) return false;
	return true;
}

bool pb_decode_msg(pb_buffer_t *dec_pb_buf, msg_t *msg) {
	DECODE_PB_MSG_BEGIN(dec_pb_buf)

	DECODE_PB_MSG_FIELD_BEGIN(dec_pb_buf, 1)
	if (!pb_decode_uint32(dec_pb_buf, &msg->id)) return false;
	DECODE_PB_MSG_FIELD_END

	DECODE_PB_MSG_FIELD_BEGIN(dec_pb_buf, 2)
	if (!pb_decode_string(dec_pb_buf, msg->name, sizeof(msg->name), &msg->name_len)) return false;
	DECODE_PB_MSG_FIELD_END

	DECODE_PB_MSG_END(dec_pb_buf)

	return true;
}

int main() {
	msg_t msgA;
	msgA.id = 211314;
	msgA.name_len = 11;
	memcpy(msgA.name, "hello,world", msgA.name_len);

	uint8_t buf[100];
	pb_buffer_t enc_pb_buf;
	enc_pb_buf.buf = buf;
	enc_pb_buf.buf_cap = sizeof(buf);
	enc_pb_buf.offset = 0;

	printf("encode ret = %d\n", pb_encode_msg(&msgA, &enc_pb_buf));

	msg_t msgB;
	pb_buffer_t dec_pb_buf;
	dec_pb_buf.buf = enc_pb_buf.buf;
	dec_pb_buf.buf_cap = enc_pb_buf.offset;
	dec_pb_buf.offset = 0;

	printf("decode ret = %d\n", pb_decode_msg(&dec_pb_buf, &msgB));
	printf("%d ", msgB.id);
	uint32_t i = 0;
	for (i = 0; i < msgB.name_len; ++i) {
		printf("%c", msgB.name[i]);
	}
	puts("");

	return 0;
}
```
/*
 * Cyclops_Message.h
 *
 *  Created on: Dec 8, 2017
 *      Author: xz
 */
#ifndef CYCLOPS_MESSAGE_H
#define CYCLOPS_MESSAGE_H

typedef enum {
	MSG_TYPE_INVALID,

	MSG_TYPE_YUANJING_DATA1 = 0X01,     //远景十选一，CSI2
	MSG_TYPE_YUANJING_DATA2,     //远景十选一，CSI3
	MSG_TYPE_JINJING_DATA1,     //近景十选一，CSI2
	MSG_TYPE_JINJING_DATA2,     //近景十选一，CSI3
	MSG_TYPE_JINJING_DATA3,     //近景四选一，CSI4
	MSG_COUNT
} MSG_TYPE;

void SpiSet();
void WriteMessage(MSG_TYPE n, char buf);

#endif /* CYCLOPS_MESSAGE_H_ */

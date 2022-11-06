/*
 * assert.h
 *
 *  Created on: 2022��7��22��
 *      Author: Administrator
 */

#ifndef INC_ASSERT_H_
#define INC_ASSERT_H_

#define MKRTOS_ASSERT(EX) \
	do{\
		if(!(EX)){\
			kprint(#EX);\
			while(1);\
		}\
	\
	}while(0)


#endif /* INC_ASSERT_H_ */

/*
 * wrapper.hpp
 *
 *  Created on: Oct 10, 2023
 *      Author: ohya
 */

#ifndef INC_WRAPPER_HPP_
#define INC_WRAPPER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

void init(void);
void loop(void);

//void sbusReceiveCallback(DMA_HandleTypeDef *_hdma);

#ifdef __cplusplus
};
#endif

#endif /* INC_WRAPPER_HPP_ */

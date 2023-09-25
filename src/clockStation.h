//  @Author: sherlava@gmail.com
// 
#ifndef __CHSBKGPSM4K__
#define __CHSBKGPSM4K__

#include <inttypes.h>

#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"





class CHSBKGPSM4K {

public:
  //! Initialize a CHSBKGPSM4K object, setting the UART pins, uart speed and UART_num
  
  //! Serial0.begin(57600, SERIAL_8N1, 20, 21);
  //!
  //! @param uart_config - Set config parametrs baud_rate = 115200,
  // data_bits 
  // parity 
  // stop_bits 
  // flow_ctrl 
  // source_clk
  
  CHSBKGPSM4K(uart_config_t uart_config);

  // 

  int sendData(const char* data);

  static void tx_task(void *arg);

  static void rx_task(void *arg);

};

#endif //__CHSBKGPSM4K__

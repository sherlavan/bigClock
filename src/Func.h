#if !defined(HELPFUNC)
#define HELPFUNC
#include <sstream>
#include <iomanip>
#include <HardwareSerial.h>




//return string f.e. 0x0a 0x14 0x1e 0x28 0x32 from unsigned char bytearr[5] ={10,20,30,40,50};
//@param data ссылка на массив байт
//@param len длина массива
std::string hexStr(unsigned char* data, int len);

//читает ответ из port до Terminator в answer массив байт и в answerLen длину массива
// возвращает true если таймаут
// в answer[0] хранится длина ответа
// @param port  ссылка на UART
// @param Terminator байт окончания
// @param answer  массив байт ответа
// @param TimeOut время ожидания ответа в мс
static bool readUartUntil(HardwareSerial & port, unsigned char Terminator, unsigned char * answer, unsigned int TimeOut = 100); 



#endif // HELPFUNC

#if !defined(HELPFUNC)
#define HELPFUNC
#include <sstream>
#include <iomanip>


//return string f.e. 0x0a 0x14 0x1e 0x28 0x32 from unsigned char bytearr[5] ={10,20,30,40,50};
//@param data 
std::string hexStr(unsigned char* data, int len);



#endif // HELPFUNC

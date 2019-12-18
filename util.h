#ifndef _UTIL_H_
#define _UTIL_H_
bool    CheckInput(void);   // 检查是否有输入
char    *ReadInput(void);   // 读取一行
int     ReadDigit(const char *LineStr, int MaxValue);   // 读取某串字符中的数字
#endif

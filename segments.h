//задаем светящиеся сегменты в соответствующих символах

#define DIGIT_0 (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_DP)
#define DIGIT_1 (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_DP)
#define DIGIT_2 (1<<SEG_A)|(1<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_3 (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_4 (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_5 (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_6 (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_7 (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_DP)
#define DIGIT_8 (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_9 (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_A (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_B (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_C (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_D (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_E (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define DIGIT_F (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)

#define SYM_DP (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(1<<SEG_DP)
#define SYM_UP (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_DP)
#define SYM_DN (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_DP)
#define SYM_MINUS (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)
#define SYM_h (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_DP)


//массив символов для светодиодного индикатора
__flash unsigned char led_digits[]  ={DIGIT_0,DIGIT_1,DIGIT_2,DIGIT_3,DIGIT_4,DIGIT_5,DIGIT_6,DIGIT_7,DIGIT_8,DIGIT_9,DIGIT_A,DIGIT_B,DIGIT_C,DIGIT_D,DIGIT_E,DIGIT_F,SYM_DP,SYM_UP,SYM_DN,SYM_MINUS,SYM_h};
// отображаемый символ  	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F	point	up	dn	minus	h
// адрес в массиве              0x00    0x01    0x02    0x03    0x04    0x05    0x06    0x07    0x08    0x09    0x0A    0x0B    0x0C    0x0D    0x0E    0x0F    0x10    0x11    0x12    0x13    0x14

unsigned char ArrayKathodes[3] = {(1<<Kathode_1), (1<<Kathode_2),(1<<Kathode_3)};
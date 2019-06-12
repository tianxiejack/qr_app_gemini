#ifndef ZODIAC_GPIO_MESSAGE_H
#define ZODIAC_GPIO_MESSAGE_H


#define GPIO_NUM152  (152)
#define ENABLE_SDI1  (0)
#define ENABLE_SDI2  (1)

#define GPIO_FAR_0_NUM	152
#define GPIO_FAR_1_NUM	187
#define GPIO_FAR_2_NUM	186
#define GPIO_FAR_3_NUM	65
#define GPIO_FAR_4_NUM	63
#define GPIO_FAR_5_NUM 	62
#define GPIO_FAR_6_NUM 	85
#define GPIO_FAR_7_NUM 	84
#define GPIO_FAR_8_NUM 	86
#define GPIO_FAR_9_NUM 	38

#define GPIO_NEAR_0_NUM	20
#define GPIO_NEAR_1_NUM	20
#define GPIO_NEAR_2_NUM	20
#define GPIO_NEAR_3_NUM	20

typedef enum
{
    DX_GPIO_DIRECTION_IN   = 0,
    /**< GPIO pin direction to in */
    DX_GPIO_DIRECTION_OUT   = 1
    /**< GPIO pin direction to out */
} DX_GPIO_DIRECTION;


typedef struct SET_Gpio
{
	long  msgtype;
	struct DATA
	{
		unsigned int gpioNum;                    //type 0
		//unsigned int direction;  //0 in  1 out         type1
		unsigned int value; //type2
	}Data;
}Set_Gpio;

enum
{
	// GPIONUM,                 //type 0
	 NUM_VALUE=1,  //0 in  1 out         type1
	// VALUE  //type2
};

int gpio_init();

int gpio_open(unsigned int nPin, DX_GPIO_DIRECTION direction);
void set_gpioNum_Value(int gpio_num,int gpio_value);
int get_gpioNum_Value(unsigned int gpio_num);
//void set_gpioDirection(int gpio_direction);
//void set_gpioValue(int gpio_value);

void init_GPIO_IPCMessage();
void delete_GPIO_IPCMessage();
void SendMessage(Set_Gpio* value);

#endif

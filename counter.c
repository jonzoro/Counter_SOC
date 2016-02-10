#include<reg52.h>
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
uchar key_num;

sbit DIG_DATA=P0^2;
sbit DIG_SHCP=P0^4;
sbit DIG_STCP=P2^3;
sbit led0 = P1^0;
sbit led1 = P1^1;
sbit led2 = P1^2;
sbit led3 = P1^3;
sbit led4 = P1^4;

code unsigned char Segment[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
code unsigned char Select[] = {0xff,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

//延时函数
void delay(uchar x)
{
	uchar i,j;
	for(i=x;i>0;i--)
		for(j=100;j>0;j--);
}

/**********************************************
功能：
    输出位选字节和段码字节
输入参数：
    SelectByte:  位选字节
    SegmentByte: 段码字节
输出参数：
    无
返回值：
**********************************************/
void DigOutput(unsigned char SelectByte,unsigned char SegmentByte)
{
	unsigned char i;
	DIG_SHCP=0;
	DIG_STCP=0;

	for(i=0;i<8;i++)
	{
		if(SegmentByte & 0x80)
		{
			DIG_DATA=1;
		}
		else
		{
			DIG_DATA=0;
		}
		_nop_();

		DIG_SHCP=1;
		_nop_();
		DIG_SHCP=0;
		_nop_();

		SegmentByte <<=1;
	}

	for(i=0;i<8;i++)
	{
		if(SelectByte & 0x80)
		{
			DIG_DATA=1;
		}
		else
		{
			DIG_DATA=0;
		}
		_nop_();
		DIG_SHCP=1;
		_nop_();
		DIG_SHCP=0;
		_nop_();
		SelectByte <<=1;
	}

	DIG_STCP=1;
	_nop_();
	DIG_STCP=0;
	_nop_();
}

/**********************************************
功能：
    在某位显示数字
输入参数：
    i: 数码管位选择（1——8，即最右到最左）
    j: 数字（0——9）
输出参数：
    无
返回值：
**********************************************/
void DigShowNumber(int i,int j)
{
	DigOutput(Select[i],Segment[j]);
}

/*键盘键值显示*/
void display(void)
{	
	DigShowNumber(1,key_num%10);	
}

//LED提醒
void LED1(void)
{
	led0=0;
	delay(2000);

	led1=0;
	delay(2000);

	led2=0;
	delay(2000);

	led3=0;
	delay(2000);

	led4=0;
	delay(2000);
	
}

void LED2(void)
{
	led4=0;
	delay(2000);

	led3=0;
	delay(2000);

	led2=0;
	delay(2000);

	led1=0;
	delay(2000);

	led0=0;
	delay(2000);
	
}

//分计时开始
void count_min(key_num)
{
	int k;
	for(key_num=key_num*60;key_num>0;key_num--)
	{
		for(k=0;k<1000;k++)
		{
			DigShowNumber(1,key_num%10);
			DigShowNumber(2,key_num/10%10);
			DigShowNumber(3,key_num/100);
			DigShowNumber(4,key_num/1000);
		}
		delay(2000);
	}

	LED1(); 	
}

//秒计时开始
void count_sec(key_num)
{
	int k;
	for(key_num=key_num*10;key_num>0;key_num--)
	{
		for(k=0;k<1000;k++)
		{
			DigShowNumber(1,key_num%10);
			DigShowNumber(2,key_num/10);
		}
		delay(2000);
	}
	LED2(); 	
}


/*键盘扫描*/
void keyboard(void)
{
	uchar temp;

	/*扫描第1列*/
	P1=0xef;
	temp=P1;
	temp=temp&0x0f;
	if(temp!=0x0f)
	{
		delay(10);
		temp=P1;
		temp=temp&0x0f;
		if(temp!=0x0f)
		{
			temp=P1;
			switch(temp)
			{
				case 0xee:
					key_num=7;
					break;
				case 0xed:
					key_num=4;
					break;
				case 0xeb:
					key_num=1;
					break;
				case 0xe7:
					key_num=0;
					break;
			}
		}
		
		while(temp!=0x0f)
		{
			temp=P1;
			temp=temp&0x0f;
			display();
		}
	}

	/*扫描第2列*/
	P1=0xdf;
	temp=P1;
	temp=temp&0x0f;
	if(temp!=0x0f)
	{
		delay(10);
		temp=P1;
		temp=temp&0x0f;
		if(temp!=0x0f)
		{
			temp=P1;
			switch(temp)
			{
				case 0xde:
					key_num=8;
					break;
				case 0xdd:
					key_num=5;
					break;
				case 0xdb:
					key_num=2;
					break;
				case 0xd7:
					//key_num=10;
					count_min(key_num);
					break;
			}
		}
		while(temp!=0x0f)
		{
			temp=P1;
			temp=temp&0x0f;
			display();
		}
	}

	/*扫描第3列*/
	P1=0xbf;
	temp=P1;
	temp=temp&0x0f;
	if(temp!=0x0f)
	{
		delay(10);
		temp=P1;
		temp=temp&0x0f;
		if(temp!=0x0f)
		{
			temp=P1;
			switch(temp)
			{
				case 0xbe:
					key_num=9;
					break;
				case 0xbd:
					key_num=6;
					break;
				case 0xbb:
					key_num=3;
					break;
				case 0xb7:
					//key_num=11;
					count_sec(key_num);
					break;
			}
		}
		while(temp!=0x0f)
		{
			temp=P1;
			temp=temp&0x0f;
			display();
		}
	}

	
}

void main(void)
{
	while(1)
	{
		keyboard();
		display();  
	}
}


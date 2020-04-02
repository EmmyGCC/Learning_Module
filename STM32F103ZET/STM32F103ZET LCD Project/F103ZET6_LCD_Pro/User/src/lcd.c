#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "font.h"
#include "usart.h"
#include "delay.h"
#include "snake.h"


//LCD的画笔颜色和背景色
uint16_t POINT_COLOR = RED;	//画笔颜色
uint16_t BACK_COLOR = WHITE;  //背景色(白)

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(uint16_t regval)
{
	LCD->LCD_REG=regval;//写入要写的寄存器序号
}
//写LCD数据
//data:要写入的值
void LCD_WR_DATA(uint16_t data)
{
	LCD->LCD_RAM=data;
}
//读LCD数据
//返回值:读到的值
uint16_t LCD_RD_DATA(void)
{
	uint16_t ram;			//防止被优化
	ram=LCD->LCD_RAM;
	return ram;
}
//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{
	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号
	LCD->LCD_RAM = LCD_RegValue;//写入数据
}
//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	LCD_WR_REG(LCD_Reg);		//写入要读的寄存器序号
	delay_us(5);
	return LCD_RD_DATA();		//返回读到的值
}
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;
}
//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(uint16_t RGB_Code)
{
	LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
uint16_t LCD_BGR2RGB(uint16_t c)
{
	uint16_t  r,g,b,rgb;
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;
	rgb=(b<<11)+(g<<5)+(r<<0);
	return(rgb);
}
//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(uint8_t i)
{
	while(i--);
}
//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
 	uint16_t r=0;
    
	if(x>=lcddev.width||y>=lcddev.height)
        return 0;	//超过了范围,直接返回
    
	LCD_SetCursor(x,y);
    LCD_WR_REG(0X22);      		 			//其他IC发送读GRAM指令
    opt_delay(2);				//FOR 9320,延时2us
 	r=LCD_RD_DATA();								//dummy Read
	opt_delay(2);
 	r=LCD_RD_DATA();  		  						//实际坐标颜色
    
    return LCD_BGR2RGB(r);						//其他IC
}
//LCD开启显示
void LCD_DisplayOn(void)
{
    LCD_WriteReg(0X07,0x0173); 				 	//开启显示
}
//LCD关闭显示
void LCD_DisplayOff(void)
{
    LCD_WriteReg(0X07,0x0);//关闭显示
}
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_WriteReg(lcddev.setxcmd, Xpos);
    LCD_WriteReg(lcddev.setycmd, Ypos);
}
//设置LCD的自动扫描方向
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
void LCD_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
    
    switch(dir)
    {
        case L2R_U2D://从左到右,从上到下
            regval|=(1<<5)|(1<<4)|(0<<3);
            break;
        default:
            break;
    }
    dirreg=0X03;
    regval|=1<<12;
    LCD_WriteReg(dirreg,regval);
}
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_SetCursor(x,y);		//设置光标位置
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD->LCD_RAM=POINT_COLOR;
}
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
    LCD_WriteReg(lcddev.setxcmd,x);
    LCD_WriteReg(lcddev.setycmd,y);
    
	LCD->LCD_REG=lcddev.wramcmd;
	LCD->LCD_RAM=color;
}
//SSD1963 背光设置
//pwm:背光等级,0~100.越大越亮.
void LCD_SSD_BackLightSet(uint8_t pwm)
{
	LCD_WR_REG(0xBE);	//配置PWM输出
	LCD_WR_DATA(0x05);	//1设置PWM频率
	LCD_WR_DATA(pwm*2.55);//2设置PWM占空比
	LCD_WR_DATA(0x01);	//3设置C
	LCD_WR_DATA(0xFF);	//4设置D
	LCD_WR_DATA(0x00);	//5设置E
	LCD_WR_DATA(0x00);	//6设置F
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(uint8_t dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
        
        lcddev.wramcmd=0X22;
        lcddev.setxcmd=0X20;
        lcddev.setycmd=0X21;
	}
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint8_t hsareg,heareg,vsareg,veareg;
	uint16_t hsaval,heaval,vsaval,veaval;
	uint16_t twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
    
    if(lcddev.dir==1)//横屏
    {
        //窗口值
        hsaval=sy;
        heaval=theight;
        vsaval=lcddev.width-twidth-1;
        veaval=lcddev.width-sx-1;
    }else
    {
        hsaval=sx;
        heaval=twidth;
        vsaval=sy;
        veaval=theight;
    }
    hsareg=0X50;heareg=0X51;//水平方向窗口寄存器
    vsareg=0X52;veareg=0X53;//垂直方向窗口寄存器
    //设置寄存器值
    LCD_WriteReg(hsareg,hsaval);
    LCD_WriteReg(heareg,heaval);
    LCD_WriteReg(vsareg,vsaval);
    LCD_WriteReg(veareg,veaval);
    LCD_SetCursor(sx,sy);	//设置光标位置
}
//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试!
void LCD_Init(void)
{
  	lcddev.id=LCD_ReadReg(0x0000);	//读ID（9320/9325/9328/4531/4535等IC）
 	printf(" LCD ID:%x\r\n",lcddev.id); //打印LCD ID
    
	if(lcddev.id==0x9320)//测试OK.
	{
		LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0100);	//Driver Output Contral.
		LCD_WriteReg(0x02,0x0700);	//LCD Driver Waveform Contral.
		LCD_WriteReg(0x03,0x1030);//Entry Mode Set.
		//LCD_WriteReg(0x03,0x1018);	//Entry Mode Set.

		LCD_WriteReg(0x04,0x0000);	//Scalling Contral.
		LCD_WriteReg(0x08,0x0202);	//Display Contral 2.(0x0207)
		LCD_WriteReg(0x09,0x0000);	//Display Contral 3.(0x0000)
		LCD_WriteReg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		LCD_WriteReg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		LCD_WriteReg(0x0d,0x0000);	//Frame Maker Position.
		LCD_WriteReg(0x0f,0x0000);	//Extern Display Interface Contral 2.
		delay_ms(50);
		LCD_WriteReg(0x07,0x0101);	//Display Contral.
		delay_ms(50);
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		LCD_WriteReg(0x11,0x0007);								//Power Control 2.(0x0001)
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		LCD_WriteReg(0x13,0x0b00);								//Power Control 4.
		LCD_WriteReg(0x29,0x0000);								//Power Control 7.

		LCD_WriteReg(0x2b,(1<<14)|(1<<4));
		LCD_WriteReg(0x50,0);	//Set X Star
		//水平GRAM终止位置Set X End.
		LCD_WriteReg(0x51,239);	//Set Y Star
		LCD_WriteReg(0x52,0);	//Set Y End.t.
		LCD_WriteReg(0x53,319);	//

		LCD_WriteReg(0x60,0x2700);	//Driver Output Control.
		LCD_WriteReg(0x61,0x0001);	//Driver Output Control.
		LCD_WriteReg(0x6a,0x0000);	//Vertical Srcoll Control.

		LCD_WriteReg(0x80,0x0000);	//Display Position? Partial Display 1.
		LCD_WriteReg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		LCD_WriteReg(0x82,0x0000);	//RAM Address End-Partial Display 1.
		LCD_WriteReg(0x83,0x0000);	//Displsy Position? Partial Display 2.
		LCD_WriteReg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		LCD_WriteReg(0x85,0x0000);	//RAM Address End? Partial Display 2.

		LCD_WriteReg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		LCD_WriteReg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		LCD_WriteReg(0x93,0x0001);	//Panel Interface Contral 3.
		LCD_WriteReg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		LCD_WriteReg(0x97,(0<<8));	//
		LCD_WriteReg(0x98,0x0000);	//Frame Cycle Contral.
		LCD_WriteReg(0x07,0x0173);	//(0x0173)
	}
	LCD_Display_Dir(0);		//默认为竖屏
	LCD_LED=1;				//点亮背光
	LCD_Clear(WHITE);
}
//清屏函数
//color:要清屏的填充色
void LCD_Clear(uint16_t color)
{
	uint32_t index=0;
	uint32_t totalpoint = lcddev.width;
	totalpoint *= lcddev.height; 			//得到总点数
    
    LCD_SetCursor(0x00,0x0000);	//设置光标位置
	LCD_WriteRAM_Prepare();     		//开始写入GRAM
    
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;
	}
}
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
	uint16_t i,j;
	uint16_t xlen=0;

    xlen=ex-sx+1;
    for(i=sy;i<=ey;i++)
    {
        LCD_SetCursor(sx,i);      				//设置光标位置
        LCD_WriteRAM_Prepare();     			//开始写入GRAM
        for(j=0;j<xlen;j++)LCD->LCD_RAM=color;	//显示颜色
    }
}
//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];//写入数据
	}
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
    
	delta_x = x2-x1; //计算坐标增量
	delta_y = y2-y1;
	uRow = x1;
	uCol = y1;
    
	if(delta_x>0)
        incx = 1; //设置单步方向
	else if(delta_x==0)
        incx = 0;//垂直线
	else 
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
	if(delta_y>0)
        incy = 1;
	else if(delta_y==0)
        incy = 0;//水平线
	else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
	if( delta_x>delta_y)
        distance=delta_x; //选取基本增量坐标轴
	else 
        distance=delta_y;
    
	for(t=0;t<=distance+1;t++ )//画线输出
	{
		LCD_DrawPoint(uRow,uCol);//画点
		xerr += delta_x;
		yerr += delta_y;
		if(xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if(yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}
//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0
		LCD_DrawPoint(x0+b,y0+a);             //4
		LCD_DrawPoint(x0+a,y0+b);             //6
		LCD_DrawPoint(x0-a,y0+b);             //1
 		LCD_DrawPoint(x0-b,y0+a);
		LCD_DrawPoint(x0-a,y0-b);             //2
  		LCD_DrawPoint(x0-b,y0-a);             //7
		a++;
		//使用Bresenham算法画圆
		if(di<0)di +=4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		}
	}
}
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{
    uint8_t temp,t1,t;
	uint16_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
                LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)
                LCD_Fast_DrawPoint(x,y,BACK_COLOR);
            
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}
	}
}
//m^n函数
//返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}
//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
{
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0);
	}
}

//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);
 				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01);
	}
}
//显示字符串
//x,y:起点坐标
//width:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint8_t size, uint8_t *p)
{
    uint8_t height = size;
	uint8_t x0 = x;
	width += x;
	height += y;
    
    while((*p<='~') && (*p>=' '))//判断是不是非法字符!
    {
        if(x>=width){x=x0;y+=size;}
        
        if(y>=height)break;//退出
        
        LCD_ShowChar(x,y,*p,size,0);
        
        x += size/2;
        p++;
    }
}


void LCD_GameOverShow(void)
{
    uint8_t str_gameover[] = "GAME OVER!";
    
    LCD_ShowString((MAX_ROW - strlen((char *)str_gameover)*(FONT_SIZE_24/2))/2, 200,
                    200, FONT_SIZE_24, str_gameover);
    delay_ms(500);
    
    LCD_ShowString((MAX_ROW - strlen((char *)str_gameover)*(FONT_SIZE_24/2))/2, 200,
                    200, FONT_SIZE_24, "          ");
    delay_ms(500);
    
    LCD_ShowString((MAX_ROW - strlen((char *)str_gameover)*(FONT_SIZE_24/2))/2, 200,
                    200, FONT_SIZE_24, str_gameover);
    delay_ms(2000);
}


void LCD_BorderShow(void)
{
    LCD_Fill(0, 0, BORDER_WIDTH, 280, MAGENTA);
	LCD_Fill(MAX_ROW-BORDER_WIDTH, 0, MAX_ROW, 280, MAGENTA);
	LCD_Fill(0, 0, MAX_ROW, BORDER_WIDTH, MAGENTA);
	LCD_Fill(0, 272, MAX_ROW, 280, MAGENTA);
}


//=================================================================
//函数名称：Trun_On_Point(u8 x,u8 y)
//函数功能：点亮一个点 用于显示蛇的身体8*8像素的点
//入口参数：蛇的起点坐标
//出口参数：无
//=================================================================
void Trun_On_Point(uint16_t x, uint16_t y)
{
	LCD_Fill(x*SNAKE_WIDTH, y*SNAKE_WIDTH, x*SNAKE_WIDTH+SNAKE_WIDTH, y*SNAKE_WIDTH+SNAKE_WIDTH, RED);
}


void Trun_Off_Point(uint16_t x, uint16_t y)
{
	LCD_Fill(x*SNAKE_WIDTH, y*SNAKE_WIDTH, x*SNAKE_WIDTH+SNAKE_WIDTH, y*SNAKE_WIDTH+SNAKE_WIDTH, WHITE);
}


#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "font.h"
#include "usart.h"
#include "delay.h"
#include "snake.h"


//LCD�Ļ�����ɫ�ͱ���ɫ
uint16_t POINT_COLOR = RED;	//������ɫ
uint16_t BACK_COLOR = WHITE;  //����ɫ(��)

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

//д�Ĵ�������
//regval:�Ĵ���ֵ
void LCD_WR_REG(uint16_t regval)
{
	LCD->LCD_REG=regval;//д��Ҫд�ļĴ������
}
//дLCD����
//data:Ҫд���ֵ
void LCD_WR_DATA(uint16_t data)
{
	LCD->LCD_RAM=data;
}
//��LCD����
//����ֵ:������ֵ
uint16_t LCD_RD_DATA(void)
{
	uint16_t ram;			//��ֹ���Ż�
	ram=LCD->LCD_RAM;
	return ram;
}
//д�Ĵ���
//LCD_Reg:�Ĵ�����ַ
//LCD_RegValue:Ҫд�������
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{
	LCD->LCD_REG = LCD_Reg;		//д��Ҫд�ļĴ������
	LCD->LCD_RAM = LCD_RegValue;//д������
}
//���Ĵ���
//LCD_Reg:�Ĵ�����ַ
//����ֵ:����������
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	LCD_WR_REG(LCD_Reg);		//д��Ҫ���ļĴ������
	delay_us(5);
	return LCD_RD_DATA();		//���ض�����ֵ
}
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;
}
//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(uint16_t RGB_Code)
{
	LCD->LCD_RAM = RGB_Code;//дʮ��λGRAM
}
//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
uint16_t LCD_BGR2RGB(uint16_t c)
{
	uint16_t  r,g,b,rgb;
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;
	rgb=(b<<11)+(g<<5)+(r<<0);
	return(rgb);
}
//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(uint8_t i)
{
	while(i--);
}
//��ȡ��ĳ�����ɫֵ
//x,y:����
//����ֵ:�˵����ɫ
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
 	uint16_t r=0;
    
	if(x>=lcddev.width||y>=lcddev.height)
        return 0;	//�����˷�Χ,ֱ�ӷ���
    
	LCD_SetCursor(x,y);
    LCD_WR_REG(0X22);      		 			//����IC���Ͷ�GRAMָ��
    opt_delay(2);				//FOR 9320,��ʱ2us
 	r=LCD_RD_DATA();								//dummy Read
	opt_delay(2);
 	r=LCD_RD_DATA();  		  						//ʵ��������ɫ
    
    return LCD_BGR2RGB(r);						//����IC
}
//LCD������ʾ
void LCD_DisplayOn(void)
{
    LCD_WriteReg(0X07,0x0173); 				 	//������ʾ
}
//LCD�ر���ʾ
void LCD_DisplayOff(void)
{
    LCD_WriteReg(0X07,0x0);//�ر���ʾ
}
//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_WriteReg(lcddev.setxcmd, Xpos);
    LCD_WriteReg(lcddev.setycmd, Ypos);
}
//����LCD���Զ�ɨ�跽��
//����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
void LCD_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
    
    switch(dir)
    {
        case L2R_U2D://������,���ϵ���
            regval|=(1<<5)|(1<<4)|(0<<3);
            break;
        default:
            break;
    }
    dirreg=0X03;
    regval|=1<<12;
    LCD_WriteReg(dirreg,regval);
}
//����
//x,y:����
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_SetCursor(x,y);		//���ù��λ��
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	LCD->LCD_RAM=POINT_COLOR;
}
//���ٻ���
//x,y:����
//color:��ɫ
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
    LCD_WriteReg(lcddev.setxcmd,x);
    LCD_WriteReg(lcddev.setycmd,y);
    
	LCD->LCD_REG=lcddev.wramcmd;
	LCD->LCD_RAM=color;
}
//SSD1963 ��������
//pwm:����ȼ�,0~100.Խ��Խ��.
void LCD_SSD_BackLightSet(uint8_t pwm)
{
	LCD_WR_REG(0xBE);	//����PWM���
	LCD_WR_DATA(0x05);	//1����PWMƵ��
	LCD_WR_DATA(pwm*2.55);//2����PWMռ�ձ�
	LCD_WR_DATA(0x01);	//3����C
	LCD_WR_DATA(0xFF);	//4����D
	LCD_WR_DATA(0x00);	//5����E
	LCD_WR_DATA(0x00);	//6����F
}

//����LCD��ʾ����
//dir:0,������1,����
void LCD_Display_Dir(uint8_t dir)
{
	if(dir==0)			//����
	{
		lcddev.dir=0;	//����
		lcddev.width=240;
		lcddev.height=320;
        
        lcddev.wramcmd=0X22;
        lcddev.setxcmd=0X20;
        lcddev.setycmd=0X21;
	}
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}
//���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
//sx,sy:������ʼ����(���Ͻ�)
//width,height:���ڿ�Ⱥ͸߶�,�������0!!
//�����С:width*height.
void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint8_t hsareg,heareg,vsareg,veareg;
	uint16_t hsaval,heaval,vsaval,veaval;
	uint16_t twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
    
    if(lcddev.dir==1)//����
    {
        //����ֵ
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
    hsareg=0X50;heareg=0X51;//ˮƽ���򴰿ڼĴ���
    vsareg=0X52;veareg=0X53;//��ֱ���򴰿ڼĴ���
    //���üĴ���ֵ
    LCD_WriteReg(hsareg,hsaval);
    LCD_WriteReg(heareg,heaval);
    LCD_WriteReg(vsareg,vsaval);
    LCD_WriteReg(veareg,veaval);
    LCD_SetCursor(sx,sy);	//���ù��λ��
}
//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ������ILI93XXҺ��,�������������ǻ���ILI9320��!!!
//�������ͺŵ�����оƬ��û�в���!
void LCD_Init(void)
{
  	lcddev.id=LCD_ReadReg(0x0000);	//��ID��9320/9325/9328/4531/4535��IC��
 	printf(" LCD ID:%x\r\n",lcddev.id); //��ӡLCD ID
    
	if(lcddev.id==0x9320)//����OK.
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
		//ˮƽGRAM��ֹλ��Set X End.
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
	LCD_Display_Dir(0);		//Ĭ��Ϊ����
	LCD_LED=1;				//��������
	LCD_Clear(WHITE);
}
//��������
//color:Ҫ���������ɫ
void LCD_Clear(uint16_t color)
{
	uint32_t index=0;
	uint32_t totalpoint = lcddev.width;
	totalpoint *= lcddev.height; 			//�õ��ܵ���
    
    LCD_SetCursor(0x00,0x0000);	//���ù��λ��
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM
    
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;
	}
}
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
//color:Ҫ������ɫ
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
	uint16_t i,j;
	uint16_t xlen=0;

    xlen=ex-sx+1;
    for(i=sy;i<=ey;i++)
    {
        LCD_SetCursor(sx,i);      				//���ù��λ��
        LCD_WriteRAM_Prepare();     			//��ʼд��GRAM
        for(j=0;j<xlen;j++)LCD->LCD_RAM=color;	//��ʾ��ɫ
    }
}
//��ָ�����������ָ����ɫ��
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
//color:Ҫ������ɫ
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			//�õ����Ŀ��
	height=ey-sy+1;			//�߶�
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//���ù��λ��
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM
		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];//д������
	}
}
//����
//x1,y1:�������
//x2,y2:�յ�����
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
    
	delta_x = x2-x1; //������������
	delta_y = y2-y1;
	uRow = x1;
	uCol = y1;
    
	if(delta_x>0)
        incx = 1; //���õ�������
	else if(delta_x==0)
        incx = 0;//��ֱ��
	else 
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
	if(delta_y>0)
        incy = 1;
	else if(delta_y==0)
        incy = 0;//ˮƽ��
	else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
	if( delta_x>delta_y)
        distance=delta_x; //ѡȡ��������������
	else 
        distance=delta_y;
    
	for(t=0;t<=distance+1;t++ )//�������
	{
		LCD_DrawPoint(uRow,uCol);//����
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
//������
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ
		if(di<0)di +=4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		}
	}
}
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{
    uint8_t temp,t1,t;
	uint16_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{
		if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
                LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)
                LCD_Fast_DrawPoint(x,y,BACK_COLOR);
            
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//��������
				break;
			}
		}
	}
}
//m^n����
//����ֵ:m^n�η�.
uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}
//��ʾ����,��λΪ0,����ʾ
//x,y :�������
//len :���ֵ�λ��
//size:�����С
//color:��ɫ
//num:��ֵ(0~4294967295);
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

//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
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
//��ʾ�ַ���
//x,y:�������
//width:�����С
//size:�����С
//*p:�ַ�����ʼ��ַ
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint8_t size, uint8_t *p)
{
    uint8_t height = size;
	uint8_t x0 = x;
	width += x;
	height += y;
    
    while((*p<='~') && (*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {
        if(x>=width){x=x0;y+=size;}
        
        if(y>=height)break;//�˳�
        
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
//�������ƣ�Trun_On_Point(u8 x,u8 y)
//�������ܣ�����һ���� ������ʾ�ߵ�����8*8���صĵ�
//��ڲ������ߵ��������
//���ڲ�������
//=================================================================
void Trun_On_Point(uint16_t x, uint16_t y)
{
	LCD_Fill(x*SNAKE_WIDTH, y*SNAKE_WIDTH, x*SNAKE_WIDTH+SNAKE_WIDTH, y*SNAKE_WIDTH+SNAKE_WIDTH, RED);
}


void Trun_Off_Point(uint16_t x, uint16_t y)
{
	LCD_Fill(x*SNAKE_WIDTH, y*SNAKE_WIDTH, x*SNAKE_WIDTH+SNAKE_WIDTH, y*SNAKE_WIDTH+SNAKE_WIDTH, WHITE);
}


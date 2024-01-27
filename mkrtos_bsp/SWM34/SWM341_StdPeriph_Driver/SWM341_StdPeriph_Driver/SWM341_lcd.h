#ifndef __SWM341_LCD_H__
#define __SWM341_LCD_H__


typedef struct {
	uint8_t  ClkDiv;		//ȡֵ��Χ2--65
	uint8_t  Format;		//LCD_FMT_RGB565��LCD_FMT_RGB888��LCD_FMT_SRGB565��LCD_FMT_SRGB888
	uint16_t HnPixel;		//ˮƽ�������ظ��������ȡֵ1024
	uint16_t VnPixel;		//��ֱ�������ظ��������ȡֵ1024
	uint8_t  Hfp;			//horizonal front porch�����ȡֵ64
	uint16_t Hbp;			//horizonal back porch�� ���ȡֵ256
	uint8_t  Vfp;			//vertical front porch�� ���ȡֵ64
	uint16_t Vbp;			//vertical back porch��  ���ȡֵ256
	uint16_t HsyncWidth;	//HSYNC�͵�ƽ�������ٸ�DOTCLK�����ȡֵ256
	uint16_t VsyncWidth;	//VSYNC�͵�ƽ�������ٸ���ʱ�䣬���ȡֵ256
	
	uint32_t DataSource;	//��ʾ���ݵ�ַ
	uint32_t Background;	//������ɫ
	uint8_t  SampleEdge;	//��Ļ��DOTCLK���ĸ����ز������ݣ�LCD_SAMPLE_RISE��LCD_SAMPLE_FALL
	
	uint8_t  IntEOTEn;		//End of Transter��������ɣ��ж�ʹ��
} LCD_InitStructure;


typedef struct {
	uint8_t  Alpha;
	uint16_t HStart;		//ˮƽ������ʼλ�ã�ȡֵ��Χ0 ~ HnPixel-1
	uint16_t HStop;			//ˮƽ������ֹλ�ã�������������С��HStart���� HStop - HStart �������Ϊ����
	uint16_t VStart;
	uint16_t VStop;
	uint32_t DataSource;	//��ʾ���ݵ�ַ
} LCD_LayerInitStructure;


#define LCD_FMT_RGB565	0
#define LCD_FMT_RGB888	1
#define LCD_FMT_SRGB565	2	//Serial RGB
#define LCD_FMT_SRGB888	3

#define LCD_SAMPLE_RISE	0	//��Ļ��DOTCLK�������ز�������
#define LCD_SAMPLE_FALL	1	//��Ļ��DOTCLK���½��ز�������

#define LCD_LAYER_1	  	0
#define LCD_LAYER_2     1


void LCD_Init(LCD_TypeDef * LCDx, LCD_InitStructure * initStruct);
void LCD_LayerInit(LCD_TypeDef * LCDx, uint32_t layerx, LCD_LayerInitStructure * initStruct);
void LCD_SetLayerPos(LCD_TypeDef * LCDx, uint32_t layerx, uint16_t hstart, uint16_t hstop, uint16_t vstart, uint16_t vstop);
void LCD_Start(LCD_TypeDef * LCDx);
uint32_t LCD_IsBusy(LCD_TypeDef * LCDx);

void LCD_INTEn(LCD_TypeDef * LCDx);
void LCD_INTDis(LCD_TypeDef * LCDx);
void LCD_INTClr(LCD_TypeDef * LCDx);
uint32_t LCD_INTStat(LCD_TypeDef * LCDx);



typedef struct {
	uint8_t  RDHoldTime;	//LCD_RD�͵�ƽ����ʱ��,ȡֵ1--32
	uint8_t  WRHoldTime;	//LCD_WR�͵�ƽ����ʱ��,ȡֵ1--16
	uint8_t  CSFall_WRFall;	//LCD_CS�½��ص�LCD_WR�½�����ʱ��ȡֵ1--4
	uint8_t  WRRise_CSRise;	//LCD_WR�����ص�LCD_CS��������ʱ��ȡֵ1--4
	uint8_t  RDCSRise_Fall;	//������ʱ��LCD_CS�����ص��½�����ʱ��ȡֵ1--32
	uint8_t  WRCSRise_Fall;	//д����ʱ��LCD_CS�����ص��½�����ʱ��ȡֵ1--16
} MPULCD_InitStructure;


void MPULCD_Init(LCD_TypeDef * LCDx, MPULCD_InitStructure * initStruct);

void LCD_WR_REG(LCD_TypeDef * LCDx, uint16_t reg);
void LCD_WR_DATA(LCD_TypeDef * LCDx, uint16_t val);
void LCD_WriteReg(LCD_TypeDef * LCDx, uint16_t reg, uint16_t val);
uint16_t LCD_ReadReg(LCD_TypeDef * LCDx, uint16_t reg);

void MPULCD_DMAStart(LCD_TypeDef * LCDx, uint32_t * buff, uint16_t hpix, uint16_t vpix);
uint32_t MPULCD_DMABusy(LCD_TypeDef * LCDx);


#endif //__SWM341_LCD_H__

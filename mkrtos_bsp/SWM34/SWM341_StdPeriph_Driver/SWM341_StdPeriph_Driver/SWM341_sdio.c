/****************************************************************************************************************************************** 
* �ļ�����: SWM341_sdio.c
* ����˵��:	SWM341��Ƭ����SDIO�ӿ�������
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������: Ϊ��ͨ���ԡ������ԡ������ԣ�ֻ֧����512�ֽ�Ϊ��λ�Ķ�д
* �汾����:	V1.1.0		2017��10��25��
* ������¼:  
*
*
*******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/
#include "SWM341.h"
#include "SWM341_sdio.h"


SD_CardInfo SD_cardInfo;

/****************************************************************************************************************************************** 
* ��������: SDIO_Init()
* ����˵��:	SDIO��дSD����ʼ������ʼ���ɸ���4��ģʽ����д��512�ֽڴ�С����
* ��    ��: uint32_t freq			SDIO_CLKʱ��Ƶ��
* ��    ��: uint32_t				SD_RES_OK �����ɹ�    SD_RES_ERR ����ʧ��    SD_RES_TIMEOUT ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t SDIO_Init(uint32_t freq)
{
	uint32_t res;
	uint32_t resp, resps[4];
	
	SYS->CLKSEL &= ~SYS_CLKSEL_SDIO_Msk;
	if(SystemCoreClock > 80000000)		//SDIOʱ����ҪС��52MHz
		SYS->CLKSEL |= (2 << SYS_CLKSEL_SDIO_Pos);	//SDCLK = SYSCLK / 4
	else
		SYS->CLKSEL |= (0 << SYS_CLKSEL_SDIO_Pos);	//SDCLK = SYSCLK / 2
	
	SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_SDIO_Pos);
	
	SDIO->CR2 = (1 << SDIO_CR2_RSTALL_Pos);
	
	SDIO->CR1 = (1 << SDIO_CR1_CDSRC_Pos) |
				(0 << SDIO_CR1_8BIT_Pos)  |
				(0 << SDIO_CR1_4BIT_Pos)  |
				(1 << SDIO_CR1_PWRON_Pos) |
				(7 << SDIO_CR1_VOLT_Pos);
	
	SDIO->CR2 = (1 << SDIO_CR2_CLKEN_Pos) |
				(1 << SDIO_CR2_SDCLKEN_Pos) |
				(calcSDCLKDiv(100000) << SDIO_CR2_SDCLKDIV_Pos) |
				(0xC << SDIO_CR2_TIMEOUT_Pos);		// 2**25 SDIO_CLK
	
	while((SDIO->CR2 & SDIO_CR2_CLKRDY_Msk) == 0);
	
	SDIO->IM = 0xFFFFFFFF;
	
	
	SDIO_SendCmd(SD_CMD_GO_IDLE_STATE, 0x00, SD_RESP_NO, 0);				//CMD0: GO_IDLE_STATE
	
	res = SDIO_SendCmd(SD_CMD_SEND_IF_COND, 0x1AA, SD_RESP_32b, &resp);		//CMD8: SEND_IF_COND, ��⹤����ѹ������Ƿ�֧��SD 2.0
	if(res != SD_RES_OK)
		return res;
	
	if(resp == 0x1AA) SD_cardInfo.CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;
	else			  SD_cardInfo.CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;
	
	do																		//ACMD41: SD_CMD_SD_APP_OP_COND
	{
		res = SDIO_SendCmd(SD_CMD_APP_CMD, 0x00, SD_RESP_32b, &resp);
		if(res != SD_RES_OK)
			return res;
		
		if(resp != 0x120) return SD_RES_ERR;	//����SD����������MMC��
		
		if(SD_cardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0)
			SDIO_SendCmd(SD_CMD_SD_APP_OP_COND, 0x80100000|0x40000000, SD_RESP_32b, &resp);
		else
			SDIO_SendCmd(SD_CMD_SD_APP_OP_COND, 0x80100000|0x00000000, SD_RESP_32b, &resp);
	} while(((resp >> 31) & 0x01) == 0);		//�ϵ�û���ʱresp[31] == 0
	
	if(((resp >> 30) & 0x01) == 1) SD_cardInfo.CardType = SDIO_HIGH_CAPACITY_SD_CARD;
	
	
	SDIO_SendCmd(SD_CMD_ALL_SEND_CID, 0x00, SD_RESP_128b, resps);			//CMD2: SD_CMD_ALL_SEND_CID����ȡCID
	
	parseCID(resps);
	
	
	SDIO_SendCmd(SD_CMD_SET_REL_ADDR, 0x00, SD_RESP_32b, &resp);			//CMD3: SD_CMD_SET_REL_ADDR������RCA
	
	SD_cardInfo.RCA = resp >> 16;
	
	
	SDIO_SendCmd(SD_CMD_SEND_CSD, SD_cardInfo.RCA << 16, SD_RESP_128b, resps);	//CMD9: SD_CMD_SEND_CSD����ȡCSD
	
	parseCSD(resps);
	
	if(SD_cardInfo.CardBlockSize < 0x200) return SD_RES_ERR;	//������ֻ֧����512�ֽ�Ϊ��λ�Ķ�д����������д��λ���벻С��512
	
	
	SDIO->CR2 &= ~(SDIO_CR2_SDCLKEN_Msk | SDIO_CR2_SDCLKDIV_Msk);
	SDIO->CR2 |= (1 << SDIO_CR2_SDCLKEN_Pos) |
				 (calcSDCLKDiv(freq) << SDIO_CR2_SDCLKDIV_Pos);	//��ʼ����ɣ�SDCLK�л�������
	
	
	SDIO_SendCmd(SD_CMD_SEL_DESEL_CARD, SD_cardInfo.RCA << 16, SD_RESP_32b_busy, &resp);	//CMD7: ѡ�п�����Standyģʽ����Transferģʽ
	SDIO->IF = SDIO_IF_TRXDONE_Msk;
	
	SDIO_SendCmd(SD_CMD_APP_CMD, SD_cardInfo.RCA << 16, SD_RESP_32b, &resp);
	
	SDIO_SendCmd(SD_CMD_APP_SD_SET_BUSWIDTH, SD_BUSWIDTH_4b, SD_RESP_32b, &resp);	//�л���4λ����ģʽ
	
	SDIO->CR1 |= (1 << SDIO_CR1_4BIT_Pos);
	
	
	SDIO_SendCmd(SD_CMD_SET_BLOCKLEN, 512, SD_RESP_32b, &resp);		//�̶����Сλ512�ֽ�
	
	SD_cardInfo.CardBlockSize = 512;
	
	SDIO->BLK = 512;
	
	return SD_RES_OK;
}

/****************************************************************************************************************************************** 
* ��������: SDIO_BlockWrite()
* ����˵��:	��SD��д������
* ��    ��: uint32_t block_addr		SD�����ַ��ÿ��512�ֽ�
*			uint32_t buff[]			Ҫд�������
* ��    ��: uint32_t				SD_RES_OK �����ɹ�    SD_RES_ERR ����ʧ��    SD_RES_TIMEOUT ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t SDIO_BlockWrite(uint32_t block_addr, uint32_t buff[])
{
	uint32_t res, i;
	uint32_t addr, resp;
	
	if(SD_cardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)	addr = block_addr;
	else													addr = block_addr * 512;
	
	res = SDIO_SendCmdWithData(SD_CMD_WRITE_SINGLE_BLOCK, addr, SD_RESP_32b, &resp, 0, 1);
	if(res != SD_RES_OK)
		return res;
	
    while((SDIO->IF & SDIO_IF_BUFWRRDY_Msk) == 0) __NOP();
    SDIO->IF = SDIO_IF_BUFWRRDY_Msk;		
    
    for(i = 0; i < 512/4; i++) SDIO->DATA = buff[i];
	
    while((SDIO->IF & SDIO_IF_TRXDONE_Msk) == 0) __NOP();
	SDIO->IF = SDIO_IF_TRXDONE_Msk;
	
	return SD_RES_OK;
}

/****************************************************************************************************************************************** 
* ��������: SDIO_MultiBlockWrite()
* ����˵��:	��SD��д��������
* ��    ��: uint32_t block_addr		SD�����ַ��ÿ��512�ֽ�
*			uint16_t block_cnt		Ҫд��Ŀ���
*			uint32_t buff[]			Ҫд�������
* ��    ��: uint32_t				SD_RES_OK �����ɹ�    SD_RES_ERR ����ʧ��    SD_RES_TIMEOUT ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t SDIO_MultiBlockWrite(uint32_t block_addr, uint16_t block_cnt, uint32_t buff[])
{
	uint32_t res, i, j;
	uint32_t addr, resp;
	
	if(SD_cardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)	addr = block_addr;
	else													addr = block_addr * 512;
	
	res = SDIO_SendCmdWithData(SD_CMD_WRITE_MULT_BLOCK, addr, SD_RESP_32b, &resp, 0, block_cnt);
	if(res != SD_RES_OK)
		return res;
	
	for(i = 0; i < block_cnt; i++)
	{
		while((SDIO->IF & SDIO_IF_BUFWRRDY_Msk) == 0) __NOP();
		SDIO->IF = SDIO_IF_BUFWRRDY_Msk;
		
		for(j = 0; j < 512/4; j++) SDIO->DATA = buff[i*(512/4) + j];
	}
	
	while((SDIO->IF & SDIO_IF_TRXDONE_Msk) == 0) __NOP();
	SDIO->IF = SDIO_IF_TRXDONE_Msk;
	
	return SD_RES_OK;
}

/****************************************************************************************************************************************** 
* ��������: SDIO_DMABlockWrite()
* ����˵��:	ͨ��DMA��SD��д��������
* ��    ��: uint32_t block_addr		SD�����ַ��ÿ��512�ֽ�
*			uint16_t block_cnt		Ҫд��Ŀ���
*			uint32_t buff[]			Ҫд�������
* ��    ��: uint32_t				SD_RES_OK �����ɹ�    SD_RES_ERR ����ʧ��    SD_RES_TIMEOUT ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t SDIO_DMABlockWrite(uint32_t block_addr, uint16_t block_cnt, uint32_t buff[])
{
	uint32_t res;
	uint32_t addr, resp;
	
	if(SD_cardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)	addr = block_addr;
	else													addr = block_addr * 512;
	
	SDIO->DMA_MEM_ADDR = (uint32_t) buff;
	
	res = SDIO_SendCmdWithDataByDMA(SD_CMD_WRITE_MULT_BLOCK, addr, SD_RESP_32b, &resp, 0, block_cnt);
	if(res != SD_RES_OK)
		return res;
	
	while((SDIO->IF & SDIO_IF_TRXDONE_Msk) == 0) __NOP();
	SDIO->IF = SDIO_IF_TRXDONE_Msk;
	
	return SD_RES_OK;
}

/****************************************************************************************************************************************** 
* ��������: SDIO_BlockRead()
* ����˵��:	��SD����������
* ��    ��: uint32_t block_addr		SD�����ַ��ÿ��512�ֽ�
*			uint32_t buff[]			����������
* ��    ��: uint32_t				SD_RES_OK �����ɹ�    SD_RES_ERR ����ʧ��    SD_RES_TIMEOUT ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t SDIO_BlockRead(uint32_t block_addr, uint32_t buff[])
{
	uint32_t res, i;
    uint32_t addr, resp;
	
	if(SD_cardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)	addr = block_addr;
	else													addr = block_addr * 512;
	
	res = SDIO_SendCmdWithData(SD_CMD_READ_SINGLE_BLOCK, addr, SD_RESP_32b, &resp, 1, 1);
	if(res != SD_RES_OK)
		return res;
	
    while((SDIO->IF & SDIO_IF_BUFRDRDY_Msk) == 0) __NOP();
	SDIO->IF = SDIO_IF_BUFRDRDY_Msk;
    
    for(i = 0; i < 512/4; i++) buff[i] = SDIO->DATA;
    
	while((SDIO->IF & SDIO_IF_TRXDONE_Msk) == 0) __NOP();
	SDIO->IF = SDIO_IF_TRXDONE_Msk;
	
	return SD_RES_OK;
}

/****************************************************************************************************************************************** 
* ��������: SDIO_MultiBlockRead()
* ����˵��:	��SD�������������
* ��    ��: uint32_t block_addr		SD�����ַ��ÿ��512�ֽ�
*			uint16_t block_cnt		Ҫ�����Ŀ���
*			uint32_t buff[]			����������
* ��    ��: uint32_t				SD_RES_OK �����ɹ�    SD_RES_ERR ����ʧ��    SD_RES_TIMEOUT ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t SDIO_MultiBlockRead(uint32_t block_addr, uint16_t block_cnt, uint32_t buff[])
{
	uint32_t res, i, j;
    uint32_t addr, resp;
	
	if(SD_cardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)	addr = block_addr;
	else													addr = block_addr * 512;
	
	res = SDIO_SendCmdWithData(SD_CMD_READ_MULT_BLOCK, addr, SD_RESP_32b, &resp, 1, block_cnt);
	if(res != SD_RES_OK)
		return res;
	
	for(i = 0; i < block_cnt; i++)
	{
		while((SDIO->IF & SDIO_IF_BUFRDRDY_Msk) == 0) __NOP();
		SDIO->IF = SDIO_IF_BUFRDRDY_Msk;
		
		for(j = 0; j < 512/4; j++) buff[i*(512/4) + j] = SDIO->DATA;
	}
	
	while((SDIO->IF & SDIO_IF_TRXDONE_Msk) == 0) __NOP();
	SDIO->IF = SDIO_IF_TRXDONE_Msk;
	
	return SD_RES_OK;
}

/****************************************************************************************************************************************** 
* ��������: SDIO_DMABlockRead()
* ����˵��:	ͨ��DMA��SD�������������
* ��    ��: uint32_t block_addr		SD�����ַ��ÿ��512�ֽ�
*			uint16_t block_cnt		Ҫ�����Ŀ���
*			uint32_t buff[]			����������
* ��    ��: uint32_t				SD_RES_OK �����ɹ�    SD_RES_ERR ����ʧ��    SD_RES_TIMEOUT ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t SDIO_DMABlockRead(uint32_t block_addr, uint16_t block_cnt, uint32_t buff[])
{
	uint32_t res;
    uint32_t addr, resp;
	
	if(SD_cardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)	addr = block_addr;
	else													addr = block_addr * 512;
	
	SDIO->DMA_MEM_ADDR = (uint32_t)buff;
	
	res = SDIO_SendCmdWithDataByDMA(SD_CMD_READ_MULT_BLOCK, addr, SD_RESP_32b, &resp, 1, block_cnt);
	if(res != SD_RES_OK)
		return res;
	
	while((SDIO->IF & SDIO_IF_TRXDONE_Msk) == 0) __NOP();
	SDIO->IF = SDIO_IF_TRXDONE_Msk;
	
	return SD_RES_OK;
}

/****************************************************************************************************************************************** 
* ��������: _SDIO_SendCmd()
* ����˵��:	SDIO��SD����������
* ��    ��: uint32_t cmd			��������
*			uint32_t arg			�������
*			uint32_t resp_type		��Ӧ���ͣ�ȡֵSD_RESP_NO��SD_RESP_32b��SD_RESP_128b��SD_RESP_32b_busy
*			uint32_t *resp_data		��Ӧ����
*			uint32_t have_data		�Ƿ������ݴ���
*			uint32_t data_read		1 ��SD��    0 дSD��
*			uint16_t block_cnt		��д�����
*			uint32_t use_dma		1 ʹ��DMA��������
* ��    ��: uint32_t				SD_RES_OK �����ɹ�    SD_RES_ERR ����ʧ��    SD_RES_TIMEOUT ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t _SDIO_SendCmd(uint32_t cmd, uint32_t arg, uint32_t resp_type, uint32_t *resp_data, uint32_t have_data, uint32_t data_read, uint16_t block_cnt, uint32_t use_dma)
{	
	SDIO->BLK &= ~SDIO_BLK_COUNT_Msk;
	SDIO->BLK |= (block_cnt << SDIO_BLK_COUNT_Pos);
	
	SDIO->ARG = arg;
	SDIO->CMD = (cmd             << SDIO_CMD_CMDINDX_Pos)   |
				(0               << SDIO_CMD_CMDTYPE_Pos)   |
				(0               << SDIO_CMD_IDXCHECK_Pos)  |
				(0               << SDIO_CMD_CRCCHECK_Pos)  |
				(resp_type       << SDIO_CMD_RESPTYPE_Pos)  |
				(have_data       << SDIO_CMD_HAVEDATA_Pos)  |
				(data_read       << SDIO_CMD_DIRREAD_Pos)   |
				((block_cnt > 1) << SDIO_CMD_MULTBLK_Pos)   |
				((block_cnt > 1) << SDIO_CMD_BLKCNTEN_Pos)  |
				((block_cnt > 1) << SDIO_CMD_AUTOCMD12_Pos) |
				(use_dma         << SDIO_CMD_DMAEN_Pos);
	
	while((SDIO->IF & SDIO_IF_CMDDONE_Msk) == 0)
	{
		if(SDIO->IF & SDIO_IF_CMDTIMEOUT_Msk)
		{
			SDIO->IF = SDIO_IF_CMDTIMEOUT_Msk;
			
			return SD_RES_TIMEOUT;
		}
		else if(SDIO->IF & SDIO_IF_ERROR_Msk)
		{
			SDIO->IF = 0xFFFFFFFF;
			
			return SD_RES_ERR;
		}
	}
	SDIO->IF = SDIO_IF_CMDDONE_Msk;
	
	if(resp_type == SD_RESP_32b)
	{
		resp_data[0] = SDIO->RESP[0];
	}
	else if(resp_type == SD_RESP_128b)
	{
		//�Ĵ����н�CID/CSD[127-8]���δ������RESP3-0[119-0]�����λ��CRC������
		//��������ʱ������˳�򣬽�CID/CSD[127-8]�����resp_data0-3[127-8]�����8λ���0x00
		resp_data[0] = (SDIO->RESP[3] << 8) + ((SDIO->RESP[2] >> 24) & 0xFF);
		resp_data[1] = (SDIO->RESP[2] << 8) + ((SDIO->RESP[1] >> 24) & 0xFF);
		resp_data[2] = (SDIO->RESP[1] << 8) + ((SDIO->RESP[0] >> 24) & 0xFF);
		resp_data[3] = (SDIO->RESP[0] << 8) + 0x00;
	}
	
	return SD_RES_OK;
}


void parseCID(uint32_t CID_Tab[4])
{
	uint8_t tmp = 0;
	
	/*!< Byte 0 */
	tmp = (uint8_t)((CID_Tab[0] & 0xFF000000) >> 24);
	SD_cardInfo.SD_cid.ManufacturerID = tmp;
	
	/*!< Byte 1 */
	tmp = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16);
	SD_cardInfo.SD_cid.OEM_AppliID = tmp << 8;
	
	/*!< Byte 2 */
	tmp = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8);
	SD_cardInfo.SD_cid.OEM_AppliID |= tmp;
	
	/*!< Byte 3 */
	tmp = (uint8_t)(CID_Tab[0] & 0x000000FF);
	SD_cardInfo.SD_cid.ProdName1 = tmp << 24;
	
	/*!< Byte 4 */
	tmp = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24);
	SD_cardInfo.SD_cid.ProdName1 |= tmp << 16;
	
	/*!< Byte 5 */
	tmp = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16);
	SD_cardInfo.SD_cid.ProdName1 |= tmp << 8;
	
	/*!< Byte 6 */
	tmp = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8);
	SD_cardInfo.SD_cid.ProdName1 |= tmp;
	
	/*!< Byte 7 */
	tmp = (uint8_t)(CID_Tab[1] & 0x000000FF);
	SD_cardInfo.SD_cid.ProdName2 = tmp;
	
	/*!< Byte 8 */
	tmp = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24);
	SD_cardInfo.SD_cid.ProdRev = tmp;
	
	/*!< Byte 9 */
	tmp = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16);
	SD_cardInfo.SD_cid.ProdSN = tmp << 24;
	
	/*!< Byte 10 */
	tmp = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8);
	SD_cardInfo.SD_cid.ProdSN |= tmp << 16;
	
	/*!< Byte 11 */
	tmp = (uint8_t)(CID_Tab[2] & 0x000000FF);
	SD_cardInfo.SD_cid.ProdSN |= tmp << 8;
	
	/*!< Byte 12 */
	tmp = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24);
	SD_cardInfo.SD_cid.ProdSN |= tmp;
	
	/*!< Byte 13 */
	tmp = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16);
	SD_cardInfo.SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
	SD_cardInfo.SD_cid.ManufactDate = (tmp & 0x0F) << 8;
	
	/*!< Byte 14 */
	tmp = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8);
	SD_cardInfo.SD_cid.ManufactDate |= tmp;
}

void parseCSD(uint32_t CSD_Tab[4])
{
	uint8_t tmp = 0;
	
	/*!< Byte 0 */
	tmp = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);
	SD_cardInfo.SD_csd.CSDStruct = (tmp & 0xC0) >> 6;
	SD_cardInfo.SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;
	SD_cardInfo.SD_csd.Reserved1 = tmp & 0x03;
	
	/*!< Byte 1 */
	tmp = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);
	SD_cardInfo.SD_csd.TAAC = tmp;
	
	/*!< Byte 2 */
	tmp = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);
	SD_cardInfo.SD_csd.NSAC = tmp;
	
	/*!< Byte 3 */
	tmp = (uint8_t)(CSD_Tab[0] & 0x000000FF);
	SD_cardInfo.SD_csd.MaxBusClkFrec = tmp;
	
	/*!< Byte 4 */
	tmp = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);
	SD_cardInfo.SD_csd.CardComdClasses = tmp << 4;
	
	/*!< Byte 5 */
	tmp = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);
	SD_cardInfo.SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4;
	SD_cardInfo.SD_csd.RdBlockLen = tmp & 0x0F;
	
	/*!< Byte 6 */
	tmp = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);
	SD_cardInfo.SD_csd.PartBlockRead = (tmp & 0x80) >> 7;
	SD_cardInfo.SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
	SD_cardInfo.SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
	SD_cardInfo.SD_csd.DSRImpl = (tmp & 0x10) >> 4;
	SD_cardInfo.SD_csd.Reserved2 = 0; /*!< Reserved */
	
	if ((SD_cardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || 
		(SD_cardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0))
	{
		SD_cardInfo.SD_csd.DeviceSize = (tmp & 0x03) << 10;
		
		/*!< Byte 7 */
		tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
		SD_cardInfo.SD_csd.DeviceSize |= (tmp) << 2;
		
		/*!< Byte 8 */
		tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);
		SD_cardInfo.SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;
		
		SD_cardInfo.SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
		SD_cardInfo.SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);
		
		/*!< Byte 9 */
		tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);
		SD_cardInfo.SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
		SD_cardInfo.SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
		SD_cardInfo.SD_csd.DeviceSizeMul = (tmp & 0x03) << 1;
		/*!< Byte 10 */
		tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
		SD_cardInfo.SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
		
		SD_cardInfo.CardCapacity = (SD_cardInfo.SD_csd.DeviceSize + 1) ;
		SD_cardInfo.CardCapacity *= (1 << (SD_cardInfo.SD_csd.DeviceSizeMul + 2));
		SD_cardInfo.CardBlockSize = 1 << (SD_cardInfo.SD_csd.RdBlockLen);
		SD_cardInfo.CardCapacity *= SD_cardInfo.CardBlockSize;
	}
	else if (SD_cardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)
	{
		/*!< Byte 7 */
		tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
		SD_cardInfo.SD_csd.DeviceSize = (tmp & 0x3F) << 16;
		
		/*!< Byte 8 */
		tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);
		
		SD_cardInfo.SD_csd.DeviceSize |= (tmp << 8);
		
		/*!< Byte 9 */
		tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);
		
		SD_cardInfo.SD_csd.DeviceSize |= (tmp);
		
		/*!< Byte 10 */
		tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
		
		SD_cardInfo.CardCapacity = (uint64_t)(SD_cardInfo.SD_csd.DeviceSize + 1) * 512 * 1024;
		SD_cardInfo.CardBlockSize = 512;
	}
	
	SD_cardInfo.SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
	SD_cardInfo.SD_csd.EraseGrMul = (tmp & 0x3F) << 1;
	
	/*!< Byte 11 */
	tmp = (uint8_t)(CSD_Tab[2] & 0x000000FF);
	SD_cardInfo.SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;
	SD_cardInfo.SD_csd.WrProtectGrSize = (tmp & 0x7F);
	
	/*!< Byte 12 */
	tmp = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24);
	SD_cardInfo.SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
	SD_cardInfo.SD_csd.ManDeflECC = (tmp & 0x60) >> 5;
	SD_cardInfo.SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
	SD_cardInfo.SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;
	
	/*!< Byte 13 */
	tmp = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16);
	SD_cardInfo.SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
	SD_cardInfo.SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
	SD_cardInfo.SD_csd.Reserved3 = 0;
	SD_cardInfo.SD_csd.ContentProtectAppli = (tmp & 0x01);
	
	/*!< Byte 14 */
	tmp = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8);
	SD_cardInfo.SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
	SD_cardInfo.SD_csd.CopyFlag = (tmp & 0x40) >> 6;
	SD_cardInfo.SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
	SD_cardInfo.SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
	SD_cardInfo.SD_csd.FileFormat = (tmp & 0x0C) >> 2;
	SD_cardInfo.SD_csd.ECC = (tmp & 0x03);
}

uint32_t calcSDCLKDiv(uint32_t freq)
{
	uint32_t prediv;
	switch((SYS->CLKSEL & SYS_CLKSEL_SDIO_Msk) >> SYS_CLKSEL_SDIO_Pos)
	{
	case 0: prediv = 1; break;
	case 1: prediv = 3; break;
	case 2: prediv = 2; break;
	case 3: prediv = 0; break;
	}
	
	uint32_t clkdiv = (SystemCoreClock / (1 << prediv)) / freq;
	uint32_t regdiv = 0;
	
	if(clkdiv > 128)     regdiv = 0x80;
	else if(clkdiv > 64) regdiv = 0x40;
	else if(clkdiv > 32) regdiv = 0x20;
	else if(clkdiv > 16) regdiv = 0x10;
	else if(clkdiv >  8) regdiv = 0x08;
	else if(clkdiv >  4) regdiv = 0x04;
	else if(clkdiv >  2) regdiv = 0x02;
	else if(clkdiv >  1) regdiv = 0x01;
	else                 regdiv = 0x00;
	
	return regdiv;
}

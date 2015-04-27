#include "ds18b20.h"
#include "chprintf.h"

static uint8_t DS18B20_SERIAL_NUMBER[MAX_SENSORS][8];
volatile uint8_t SensorCount;

void OneWire_DataPinInput(void)
{
    palSetPadMode(DS18B20_GPIO, DS18B20_DATAPIN, PAL_MODE_INPUT);
}

void OneWire_DataPinOutput(void)
{
    palSetPadMode(DS18B20_GPIO, DS18B20_DATAPIN, PAL_MODE_OUTPUT_PUSHPULL);
}

void OneWire_SetBusLine(uint8_t state)
{
	if(state)
	{
		OneWire_DataPinInput();
		palWritePad(DS18B20_GPIO, DS18B20_DATAPIN, 1);
	}
	else
	{
		OneWire_DataPinOutput();
		palWritePad(DS18B20_GPIO, DS18B20_DATAPIN, 0);
	}
}

uint8_t OneWire_GetBusLine(void)
{
	return palReadPad(DS18B20_GPIO, DS18B20_DATAPIN);
}

static void Delay_us(uint32_t us)
{
    chSysLock();

    halrtcnt_t start = halGetCounterValue();
    halrtcnt_t timeout  = start + US2RTT(us);
    while (halIsCounterWithin(start, timeout))
        asm("nop");

    chSysUnlock();
}

uint8_t OneWire_ReadBit(void)
{
	uint8_t bit = 0;

	OneWire_SetBusLine(0);
	Delay_us(2);
	OneWire_SetBusLine(1);
	Delay_us(15);
	if(OneWire_GetBusLine())
		bit = 1;
	Delay_us(38);

	return bit;
}

void OneWire_WriteBit(uint8_t bit)
{
	OneWire_SetBusLine(0);
	Delay_us(2);
	if(bit) 
		OneWire_SetBusLine(1);
	else
		OneWire_SetBusLine(0);
	Delay_us(58);
	OneWire_DataPinInput();
	OneWire_SetBusLine(1);
}

void OneWire_WriteByte(uint8_t data)
{
	uint8_t _data = data;
	int i;
	for(i=0; i<8; i++)
	{
		OneWire_WriteBit(_data & 0x01);
		_data>>= 1;
	}
	}

uint8_t OneWire_ReadByte(void)
{
	uint8_t data = 0;
	int i;
	for(i=0; i<7; i++)
	{
		if(OneWire_ReadBit())
			data|= 0x80;
		data>>= 1;
	}
	if(OneWire_ReadBit())
		data|= 0x80;

	return data;
}	  	  

uint8_t OneWire_Reset(void)
{      
	OneWire_SetBusLine(0);
	Delay_us(500);
	OneWire_SetBusLine(1); 
	Delay_us(80);
	if(!OneWire_GetBusLine()) 
	{      
		Delay_us(400);
		return 1;
	}
    else return 0;
}

void DS18B20_Init(void)
{
    palSetPadMode(DS18B20_GPIO, DS18B20_DATAPIN, PAL_MODE_INPUT);

	OneWire_DataPinInput();
	OneWire_Reset();
}

float DS18B20_GetTemp(uint8_t SensorNum)
{
	uint8_t lsb, msb;
	float Temp = 0;

	OneWire_Reset();
	if(SensorNum==0)
	{
		OneWire_WriteByte(0xCC);
	}
	else
	{
		OneWire_WriteByte(DS18B20_MATCHROM_CMD);	
		unsigned char i;
		for(i=0; i<8; i++)
		{
			OneWire_WriteByte(DS18B20_SERIAL_NUMBER[SensorNum-1][i]);
		}
	}
	OneWire_WriteByte(0xBE);

	lsb = OneWire_ReadByte();
	msb = OneWire_ReadByte();

	if(lsb==0 && msb==0) return 0;

	Temp = (int16_t)(lsb)|((int16_t)msb<<8);
	Temp*= 0.0625;

	DS18B20_StartConversion(SensorNum);

	return Temp;
}

void DS18B20_StartConversion(uint8_t SensorNum)
{
	OneWire_Reset();
	if(SensorNum==0)
	{
		OneWire_WriteByte(0xCC);
	}
	else
	{
		OneWire_WriteByte(DS18B20_MATCHROM_CMD);
		unsigned char i;
		for(i=0; i<8; i++)
		{
			OneWire_WriteByte(DS18B20_SERIAL_NUMBER[SensorNum-1][i]);
		}
	}
	OneWire_WriteByte(0x44);	
}

uint8_t OneWire_SearchROM(uint8_t diff, uint8_t *id)
{ 	
	uint8_t DataBit, DataByte, next_diff, bit;

	if(!OneWire_Reset()) 
		return 0xFF;

	OneWire_WriteByte(DS18B20_SEARCH_ROM_CMD);
	next_diff = 0x00;
	
	DataBit = 64;
	do 
	{	
		DataByte = 8;
		do 
		{ 
			bit = OneWire_ReadBit();
			if(OneWire_ReadBit()) 
			{
				if(bit)
				return 0xFE;
			}
			else 
			{ 
				if(!bit) 
				{
					if(diff>DataBit || ((*id&1) && diff!=DataBit)) 
					{ 
						bit = 1; 
						next_diff = DataBit; 
					}
				}
			}
         OneWire_WriteBit(bit);
         *id>>= 1;
         if(bit)
		 	*id |= 0x80;
         DataBit--;
		} 
		while(--DataByte);
		id++;
    } 
	while(DataBit);
	return next_diff;
}

void OneWire_FindROM(uint8_t *diff, uint8_t id[])
{
	while(1)
    {
		*diff = OneWire_SearchROM(*diff, &id[0]);
		if(*diff==0xFF || *diff==0xFE || *diff==0x00) return;
		return;
    }
}

void DS18B20_GetROM(uint8_t SensorNum, uint8_t *buffer)
{
	if(SensorNum>SensorCount || SensorNum==0)return;

	int i;
	for(i=0; i<8; i++)
		buffer[i] = DS18B20_SERIAL_NUMBER[SensorNum-1][i];		
}

uint8_t DS18B20_ScanBus(void)
{ 
	uint8_t i;
    uint8_t id[8];
    uint8_t diff;

	SensorCount = 0;

	for(diff=0xFF; diff!=0 && (SensorCount<MAX_SENSORS);)
    { 
		OneWire_FindROM(&diff, &id[0]);
		if(diff==0xFF || diff==0xFE) break;

		for (i=0;i<8;i++)
			DS18B20_SERIAL_NUMBER[SensorCount][i] = id[i];
  
		SensorCount++;
	}
	return SensorCount;
}

void cmd_ds18b20_values(BaseSequentialStream *chp, int argc, char *argv[]) {
  
  int i, j;

  (void)argc;
  (void)argv;
  chprintf(chp, "\x1B\x63");
  chprintf(chp, "\x1B[2J");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
    chprintf(chp, "\x1B[%d;%dH", 0, 0);
    for(i = 0; i < SensorCount; i++)
    {
    	chprintf(chp,"One-vire bus %d. sensor id\r\n", SensorCount);
    	for(j = 0; j < 8; j++)
    	{
    		chprintf(chp,"%x ", DS18B20_SERIAL_NUMBER[i][j]);
    	}
    	chprintf(chp,"\r\n");
    }

    chThdSleepMilliseconds(1000);
  }
}

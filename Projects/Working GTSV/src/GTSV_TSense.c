
#include "GTSV_TSense.h"

#define TSENSE_NUM_OF_KEYS	NUMBER_OF_SINGLE_CHANNEL_KEYS
#define TSENSE_DELAY_HOLD_DETECT	8

//keylevel =1 --> key falling  --> key level = 0  --> key rising --> keylevel =1
//---___---

struct Tsense_Key_Detect_t{
	unsigned rising_edge:1;
	unsigned falling_edge:1;
	unsigned high_level:1;
	unsigned key_hold:1;
	uint8_t  delay_hold_detect;

};

struct Tsense_Key_Detect_t tsense_keys[TSENSE_NUM_OF_KEYS];


//this should be called in main loop to detect key falling, rising, level
void Tsense_key_detect(void)
{
	int i;

	
	for(i=0; i<TSENSE_NUM_OF_KEYS; i++){ //key detected
		//after risng edge -> high_level =1
		if(tsense_keys[i].rising_edge){
			tsense_keys[i].rising_edge = 0;
			tsense_keys[i].high_level = 1;
			Buzzer_bip();
		}
		//after falling edge -> high_level =0
		if(tsense_keys[i].falling_edge){
			tsense_keys[i].falling_edge =0;
			tsense_keys[i].high_level = 0;
		}

		//rising edge is key detected while high level =0
		if((sSCKeyInfo[i].Setting.b.DETECTED)
			  && (!tsense_keys[i].high_level)){
			tsense_keys[i].rising_edge = 1;
		}
		//falling edge is when key not detected & high_level =1
		if((!sSCKeyInfo[i].Setting.b.DETECTED)
			  && (tsense_keys[i].high_level)){
			tsense_keys[i].falling_edge = 1;
			
		}

	}

}


void Tsense_key_hold_detect_tick125ms(void)
{
	int i;

	for(i=0; i<TSENSE_NUM_OF_KEYS; i++){
		//detect key hold
		if(tsense_keys[i].high_level){
			if(tsense_keys[i].delay_hold_detect>TSENSE_DELAY_HOLD_DETECT){
				tsense_keys[i].key_hold = 1;
			}else{
				tsense_keys[i].delay_hold_detect++;
			}
		}else{
			tsense_keys[i].delay_hold_detect = 0;
			tsense_keys[i].key_hold = 0;
		}
	}
}

/*
//this should be called in 50ms tick to detect key hold
void Tsense_key_hold_detect_tick50ms(void)
{
	int i;
	for(i=0; i<TSENSE_NUM_OF_KEYS; i++){
		if(tsense_keys[i].high_level){
			if(tsense_keys[i].delay_hold_detect>TSENSE_DELAY_HOLD_DETECT){
				tsense_keys[i].key_hold = 1;
			}else{
				tsense_keys[i].delay_hold_detect++;
			}
		}else{
			tsense_keys[i].delay_hold_detect = 0;
			tsense_keys[i].key_hold = 0;
		}
	}
}
*/

bool Tsense_check_key(enum Tsense_key_enum_t key)
{
	return tsense_keys[key].rising_edge;
}

bool Tsense_check_key_down(enum Tsense_key_enum_t key)
{
	return tsense_keys[key].rising_edge;
}
bool Tsense_check_key_up(enum Tsense_key_enum_t key)
{
	return tsense_keys[key].falling_edge;
}

bool Tsense_check_key_touching(enum Tsense_key_enum_t key)
{
	return tsense_keys[key].high_level;
}

bool Tsense_check_key_holding(enum Tsense_key_enum_t key)
{
	return tsense_keys[key].key_hold;
}



#define TS_KEY01	sSCKeyInfo[0]
#define TS_KEY02	sSCKeyInfo[1]
#define TS_KEY03	sSCKeyInfo[2]
#define TS_KEY04	sSCKeyInfo[3]
#define TS_KEY05	sSCKeyInfo[4]

void Tsense_to_default_config(void)
{
	uint8_t i;
	// Enable Comparator clock
  	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, ENABLE);
	/* Init TSL RC */
	TSL_Init();

	#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 0
	for (i = 0; i < NUMBER_OF_SINGLE_CHANNEL_KEYS; i++)
	{
		sSCKeyInfo[i].Setting.b.IMPLEMENTED = 1;
		sSCKeyInfo[i].Setting.b.ENABLED = 1;
		sSCKeyInfo[i].DxSGroup = 0x01;
	}
	#endif
	sSCKeyInfo[2].DetectThreshold = 38;
	sSCKeyInfo[2].EndDetectThreshold = 38;
	sSCKeyInfo[2].RecalibrationThreshold = -22;
	sSCKeyInfo[4].DetectThreshold = 58;
	sSCKeyInfo[4].EndDetectThreshold = 58;
	sSCKeyInfo[4].RecalibrationThreshold = -22;
	// Change thresholds of specific keys
	/*
	sSCKeyInfo[0].DetectThreshold = 99;
	sSCKeyInfo[0].EndDetectThreshold = 96;
	sSCKeyInfo[0].RecalibrationThreshold = -22;
	  
	sSCKeyInfo[1].DetectThreshold = 99;
	sSCKeyInfo[1].EndDetectThreshold = 96;
	sSCKeyInfo[1].RecalibrationThreshold = -22;
	
	sSCKeyInfo[2].DetectThreshold = 88;
	sSCKeyInfo[2].EndDetectThreshold = 86;
	sSCKeyInfo[2].RecalibrationThreshold = -22;

	sSCKeyInfo[3].DetectThreshold = 99;
	sSCKeyInfo[3].EndDetectThreshold = 96;
	sSCKeyInfo[3].RecalibrationThreshold = -22;

	sSCKeyInfo[4].DetectThreshold = 79;
	sSCKeyInfo[4].EndDetectThreshold = 76;
	sSCKeyInfo[4].RecalibrationThreshold = -22;
	*/
}

void Tsense_action(void)
{
	/* Run TSL RC state machine */
	  TSL_Action();
}

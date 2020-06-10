/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "ds18b20.h"
#include "math.h"
#define Black 0x00
#define White 0x01
#define srartchasov  12
#define startminset   51
#define startsecset 0
#define time_poliv  5   //����� ������ � ������ ������ �� ������� � ���
#define ON 1
#define OFF 0
#define vkluchitna_min 5    //�������� ������� �� ��������� ��� ���������� ������
//����� ������ ������  18-00
//#define hoursstartpolivavto 20    //����� ������ ������
//#define minutstartpolivavto 00
#define hoursresetflag  23     //����� �������� ��� ������ ����� �����
#define minutresetflag  00
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef DateToUpdate = {0};

char trans_str[64] = {0,};
char data_str[64]={0,};
uint32_t adcResult = 0;   //���������� ��� ���������� ���������� ���������
char str1[60];                //��� ds18b20
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


//--------------------------------------------------------------------------------
//������� ��������� �����
static void MX_RTC_Init2(uint8_t hoursmi,uint8_t minmi)
{

  //RTC_TimeTypeDef sTime = {0};
 // RTC_DateTypeDef DateToUpdate = {0};

 	//����� ��������� ������� ��� �������� ������� RTC
	//����� 1. ������� � ����������� ���������� RTC_TimeTypeDef sTime = {0};
	  	  	  	  	  	  	  	  	  	  	  //RTC_DateTypeDef DateToUpdate = {0};

	//        2.������������   -if (HAL_RTC_SetTime    �
	//        				 -if (HAL_RTC_SetDate      ��� ���� ����� ����� ����������� ����� �� ������������������

//------------------------------------------------------------------------------------
  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = hoursmi;
  sTime.Minutes = minmi;
  sTime.Seconds = 0;

 if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
   Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_MAY;
  DateToUpdate.Date = 11;
  DateToUpdate.Year = 20;

 //if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
 // {
 //   Error_Handler();
// }


}
//---------------------------------------------------------------------------------
//��������� ��� ������������� ���� �� �������
struct current_line
{
	char* line1;
	char* line2;
	char* line3;
	char* line4;
};

//---------------------------------------------------------
//������� ���������� �������
void update_LCD(struct current_line *setline){
	//HAL_Delay(100);
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0,0);
	ssd1306_WriteString(setline->line1,Font_11x18,White);
	ssd1306_SetCursor(0,15);
	ssd1306_WriteString(setline->line2,Font_11x18,White);
	ssd1306_SetCursor(0,30);
	ssd1306_WriteString(setline->line3,Font_11x18,White);
	ssd1306_SetCursor(0,45);
	ssd1306_WriteString(setline->line4,Font_11x18,White);
	ssd1306_UpdateScreen();

}
//------------------------------------------------------------------
//������� ������ ������ �� ������ ����������
//4 ������ ����� �� ����� 1,2,3,4
//���� ������ ������ �� ���������� ����� ������ � 1
        //  ������ ������ 1-> �������  11 �������� ������� 12 ������ �������
//���� ������ �� ������ �� ������� 0
uint8_t press_Button(){
	uint8_t code;
	uint32_t time;
	uint32_t limit=100;
	uint32_t press_count=200000;
	time=0;
	code=0;

        //��� ������ ������
		if(HAL_GPIO_ReadPin(GPIOB,first_button_Pin)==GPIO_PIN_RESET){ //������ ������ ������
			HAL_Delay(25);
			if(HAL_GPIO_ReadPin(GPIOB,first_button_Pin)==GPIO_PIN_RESET){
									code=11;
			}
		}

		//��� ������ ������
		if(HAL_GPIO_ReadPin(GPIOB,second_button_Pin)==GPIO_PIN_RESET){ //������ ������ ������
					HAL_Delay(25);
					if(HAL_GPIO_ReadPin(GPIOB,second_button_Pin)==GPIO_PIN_RESET){
						code=21;
					}
				}

		//��� ������� ������
		if(HAL_GPIO_ReadPin(GPIOB,third_button_Pin)==GPIO_PIN_RESET){ //������ ������ ������
					HAL_Delay(25);
					if(HAL_GPIO_ReadPin(GPIOB,third_button_Pin)==GPIO_PIN_RESET){
						code=31;
					}
				}
         //��� ��������� ������
		if(HAL_GPIO_ReadPin(GPIOC,fourth_button_Pin)==GPIO_PIN_RESET){ //������ ������ ������
					HAL_Delay(25);
					if(HAL_GPIO_ReadPin(GPIOC,fourth_button_Pin)==GPIO_PIN_RESET){
						code=41;
					}
				}

	return code;
}
//----------------------------------------------------------------------------------
//������� ������� ������� ����� �� �������� ���������� ����
uint16_t nextLevel(uint16_t currentlevel,uint8_t numer_button){
	uint16_t next;
		if(currentlevel==10){    //��������� � ���� �� ���������
			next=20;      //menu  ���� � ������� ����
		}else if(currentlevel==20){  //�� �������� ���� ����� ������
								  if(numer_button==1){
									  next=211;     //Ruchnoi time
								  }else if(numer_button==2){
									  next=221;      //Ruchnoi
								  }else if(numer_button==3){
									  next=231;      //Ustanovki
								  }else if(numer_button==4){
									  next=10;      //menu
								  }
		}else if(currentlevel==211){   //������ �� �������

									if(numer_button==4){
									     next=10;
								  }else{
									  next=currentlevel;
								  }

		}else if(currentlevel==221){  //������ � ������ ��������� �������
			  if(numer_button==4){
				  next=10;
			  }else{
				  next=currentlevel;
			  }
		}else if(currentlevel==231){  //���������
								  if(numer_button==1){
									  next=2311;   //   �������� ����
								  }else if(numer_button==2){
									  next=2312;    //  ������ ����
									}else if(numer_button==3){
										 next=2320;   // ��������� ���������� ���
									}else  if(numer_button==4){
									  next=10;

								  }
		}else if(currentlevel==232){
								  if(numer_button==4){
									  next=10;
								  }
		}else if(currentlevel==233){
								  if(numer_button==4){
									  next=10;
								  }
		}else if(currentlevel==2320){
									if(numer_button==1){
											next=2321;   //   ����� ������
									}else if(numer_button==2){
											next=2322;    //  ����� ��������
									}else if(numer_button==3){
										next=2323;    //  ��������� �������
									}else  if(numer_button==4){
										next=231;

									}
		}else if(currentlevel==2323){
									if(numer_button==1){
											next=23231;   //   ��������� ������� �������
									}else if(numer_button==2){
											next=23232;    //  �������� ������� ������ ������
									}else if(numer_button==3){
											next=23233;    //  �������� ����������
									}else  if(numer_button==4){
											next=2320;    //�������  � ���������� ����
									}
		}else if(currentlevel==23231){  //��������� ���������� �������
									if(numer_button==1){
										next=23234;   //   ��������� �����
									}else if(numer_button==2){
										next=23235;    //  ���������� ���
									}else if(numer_button==3){
										next=2323;    //  ������� � ���������� ����
									}else  if(numer_button==4){
										next=2323;    //�������  � ���������� ����
									}
		}else if(currentlevel==23232){   //���� ��������� ������� ������
									if(numer_button==1){
										next=23236;   //   ��������� �����
									}else if(numer_button==2){
										next=23237;    //  ���������� ���
									}else if(numer_button==3){
										next=2323;    //  ������� � ���������� ����
									}else  if(numer_button==4){
										next=2323;    //�������  � ���������� ����
									}
		}else {
			next=currentlevel;
		}

	return next;
}
//����� ������������� ������� ����
	     //      1.0 default
	     //        |
	     //      2.0 menu
	     //        |---2.1 Hand action time limit
	     //        |     |----2.1.1 Zapusk Timera
	     //        |---2.2 Hand action On/Off
	     //        |     |----2.2.1 Ruchnoe vkluchenie klapana
	     //        |---2.3 Settings
	     //        |     |----2.3.1 Nastroiki info
	     //        |     |      |-----2.3.1.1 �������� ���� ����� ��������� ������
		//			|	|		|			|----������� �������� ���
		//			|	|		|			|----���������� �������� ���
		 //		   |	 |		|-----2.3.1.2 ������ ���� ����� ��������� ������
		 //		   |	 |
	     //        |     |----2.3.2 ��������� ������//������� ��� ��������
		//						|------2.3.2.1 ����� ��� ������ ����
		//			|	|		|			|----������� ��������
		//			|	|		|			|----����������
		//						|------2.3.2.2 ����� ��� �������� ��������
		//			|	|		|			|---������� ��������
		//			|	|		|			|---���������� ��������
		//			|	|		|------2.3.2.3 ��������� �������
		//			|	|		|			|------2.3.2.3.1 ��������� �����
		//			|	|		|			|			|------2.3.2.3.4 ����
		//			|	|		|			|			|			|----������� �������� ���
		//			|	|		|			|			|			|----���������� �������� ���
		//			|	|		|			|			|------2.3.2.3.5 ���
		//			|	|		|			|						|----������� �������� ���
		//			|	|		|			|						|----���������� �������� ���
		//			|	|		|			|------2.3.2.3.2 ��������� ������� ������
		//			|	|		|			|			|------2.3.2.3.6 ����
		//			|	|		|			|			|			|----������� �������� ���
		//			|	|		|			|			|			|----���������� �������� ���
		//			|	|		|			|			|------2.3.2.3.7 ���
		//			|	|		|			|						|----������� �������� ���
		//			|	|		|			|						|----���������� �������� ���
		//			|	|		|			|-----2.3.2.3.3 �������� ������� ����� ������������ �����
		//			|	|		|			|------������
		//						|------Otmena
	     //        |     |----2.3.3 ��������� �����
	     //        |
	     //        |---2.4 Cancel
//----------------------------------------------------------------------------------
//������� ������ ����� ��������� ������
uint8_t getNewflagforpoliv(uint8_t currentflag){
	uint8_t flag;
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //������ �������
		if(sTime.Hours==hoursresetflag){
			if(sTime.Minutes==minutresetflag){
				if(currentflag!=0){
				flag=0;
				ssd1306_Fill(Black);
				ssd1306_SetCursor(0,0);
				ssd1306_WriteString(" FLAG",Font_11x18,White);
				ssd1306_SetCursor(0,15);
				ssd1306_WriteString("Sbroshen",Font_11x18,White);
				ssd1306_SetCursor(0,30);
				ssd1306_WriteString("ALL OK",Font_11x18,White);
				ssd1306_SetCursor(0,45);
				ssd1306_WriteString("Otmena",Font_11x18,White);
				ssd1306_UpdateScreen();
				HAL_Delay(3000);
				}else{
					flag=currentflag;
				}

			}else{
				flag=currentflag;
			}
		}else{
			flag=currentflag;
		}

	return flag;
}
//----------------------------------------------------------------------------------
//������� ��������������� ������
uint8_t autoRegim(uint16_t intervalmin,uint16_t pereriv_megdupolivami,uint8_t flagpolivcomplete,uint8_t hoursstartpolivavto,uint8_t minutstartpolivavto ){  //�� ���� ���� �� ������� ��� ��������
	 uint8_t status; // ������ �������� ���������� ������ ������� 1-����� ������� ��������
	                                                           // 0-�������� ����
	                                                           // 3-����� ������� � ������
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   // 4-����� ������ �� �������
	 uint16_t intervalzadan;
	 uint8_t flagstart=0;  //���� ��������� ������
	 uint8_t minstart;     //����� ������
	 uint8_t secstart;
	 uint16_t allsecstart;   //���������� ����� ������ � ��������
	 uint16_t timer_vision;  //�������� ������
	 uint8_t mincurrent;   //������� ��������� �������
	 uint8_t seccurrent;
	 uint16_t allseccurrent;  //���������� ����� ������� � ��������
	 char intervalchar[12];   //�������� ��� ������
	 char kolchar[12];
	 uint8_t kolvovklucheny;  //������� ��� �������� � ����������� �� ������ ������� � ��������
	 float kolvovkluchfloat;       //������������� ������� ����� ��������� �� �����������
	 uint8_t periodOnflag;    //���� ����� ������ ����� ��������  1- ����� 0 �������

	  //������� ������� ��������� ����� ���� ��������� 18-00 �� �������� ����� � �������� �����
	  //�� ��������� ������ ���� ������ �� �������

	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //������ �������
	  if(sTime.Hours==hoursstartpolivavto){ //���� ��������� 18 �����
		  if(sTime.Minutes==minutstartpolivavto){  //���� ��������� 00 ���
			  if(flagpolivcomplete==0){   //���� ��� �� �������� �� ���������� � ������
				  Poliv(ON);  //�������� �����

				  //intervalzadan=intervalmin*60;  //��������� �������� �������� �� ��� � �������

				  flagstart=1; //������ ���� ��� ����� ��������
				  periodOnflag=1;    //������ ���� ��� ������ �������� ������
				  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //������ �������
				  //��������� ����� ������ ������ �� �� ������� �� ���-��� ����� ��������
				  minstart=sTime.Minutes;
				  secstart=sTime.Seconds;
				  allsecstart=secstart+minstart*60; //��������� � �������
				  //allsecstart=allsecstart+intervalzadan;

				  //�������� ������� ��� ���� ���������� �� ������ ��� ������� ��� ������
				  kolvovkluchfloat=intervalmin/pereriv_megdupolivami;
				  if(kolvovkluchfloat<1){
					  kolvovklucheny=1;  //�������� ���� ���
				  }else if(kolvovkluchfloat==1){
					  kolvovklucheny=1;
				  }else if(kolvovkluchfloat>1){
					  kolvovklucheny=ceil(kolvovkluchfloat);

				  }
				  snprintf(kolchar,11,"%d_raz",kolvovklucheny);

				  intervalzadan=pereriv_megdupolivami;//��������� ����� ��������� ��������� � ���
				  intervalzadan=intervalzadan*60; //��������� � �������


				  while(flagstart==1){//������� � ����� ���� ���� ���������� �� �����





					  while(kolvovklucheny>0){

						  while(periodOnflag==1){    //��������� ����� ��������
							  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //������ �������
							  mincurrent=sTime.Minutes;
							  seccurrent=sTime.Seconds;
							  allseccurrent=seccurrent+mincurrent*60; //��������� �� ��� � ��� ������� ����� � �������
							  timer_vision=allsecstart+intervalzadan-allseccurrent;//�������� ������ ��� �������
							  snprintf(intervalchar,11,"%d_%d",timer_vision,intervalzadan);
							  snprintf(kolchar,11,"%d_raz",kolvovklucheny);
							  if((allsecstart+intervalzadan)<=allseccurrent){//����� �������� �������� ��������
								  //��
								  periodOnflag=0; //������� ��� ��� �������� �������� ������� ��� ����
							  }else{
								  //���������� �� ������� ��� ����������
								ssd1306_Fill(Black);
								ssd1306_SetCursor(0,0);
								ssd1306_WriteString(kolchar,Font_11x18,White);
								//ssd1306_WriteString(" AUTO",Font_11x18,White);
								ssd1306_SetCursor(0,15);
								ssd1306_WriteString(intervalchar,Font_11x18,White);
								ssd1306_SetCursor(0,30);
								ssd1306_WriteString("Poliv_ON",Font_11x18,White);
								ssd1306_SetCursor(0,45);
								ssd1306_WriteString("Otmena",Font_11x18,White);
								ssd1306_UpdateScreen();

							  }
							  if(HAL_GPIO_ReadPin(GPIOC,fourth_button_Pin)==GPIO_PIN_RESET){ //������� �� ������ 4
								  periodOnflag=0;   //��������� ��� ����� ��� ��� ��� �� ����������
								  kolvovklucheny=0;
								  flagstart=3;
								  //flagstart=3;
							  }
							  HAL_Delay(1000);
						  }
						  Poliv(OFF); //��������� �����
						  allsecstart=allsecstart+intervalzadan; //��������� ����� ������ ��� ���������� ����������
						  kolvovklucheny--;
						  //ssd1306_Fill(Black);
						  //ssd1306_UpdateScreen();


						  while((periodOnflag==0)&&(kolvovklucheny!=0)){ //��������� ����� ���� ������ �������� �� ����� ����������
							  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //������ �������
							  mincurrent=sTime.Minutes;
							  seccurrent=sTime.Seconds;
							  allseccurrent=seccurrent+mincurrent*60; //��������� �� ��� � ��� ������� ����� � �������
							  if((allsecstart+intervalzadan)<=allseccurrent){
								  periodOnflag=1;  //������������� ���� ��� ����� ����� ��������
							  }
							  timer_vision=allsecstart+intervalzadan-allseccurrent; //��������� ������� ��������
							  snprintf(kolchar,11,"%d_ostalos",timer_vision);

							  //���������� �� ������� ��� ����������
							  ssd1306_Fill(Black);
							  	ssd1306_SetCursor(0,0);
							  	ssd1306_WriteString(kolchar,Font_11x18,White);
							  	//ssd1306_WriteString(" AUTO",Font_11x18,White);
							  	ssd1306_SetCursor(0,15);
							  	ssd1306_WriteString("Pereriv",Font_11x18,White);
							  	ssd1306_SetCursor(0,30);
							  	ssd1306_WriteString("Poliv_OFF",Font_11x18,White);
							  	ssd1306_SetCursor(0,45);
							  	ssd1306_WriteString("Otmena",Font_11x18,White);
							  	ssd1306_UpdateScreen();


							  	if(HAL_GPIO_ReadPin(GPIOC,fourth_button_Pin)==GPIO_PIN_RESET){ //������� �� ������ 4
							  		  periodOnflag=0;    //��������� ����� ��� ��� ��� �� ����������
							  		kolvovklucheny=0;
							  		  flagstart=3;
  								  }
								  HAL_Delay(1000);
						  }
						  //����� ��������� �������� ������ ��������� ������ + ���������
						  //�������� ���� ���� ��������� �� ����� �������� ����� ������
						  if((flagstart!=3)&&(kolvovklucheny!=0)){
							  allsecstart=allsecstart+intervalzadan; //��������� ����� ������ ��� ���������� ����������
							  Poliv(ON);  //�������� �����
						  }else if(kolvovklucheny==0){   //  ���� ���������� ��������� ������ � ����
							  flagstart=2;  //���������� ���� � ������� �� ������� ����������
						  }


					  }



				  }
				  //�� while-------------------------
				  Poliv(OFF); //��������� �����
				  //����� �� �������
				  ssd1306_Fill(Black);
				  ssd1306_SetCursor(0,0);
				  ssd1306_WriteString("Regim Auto",Font_11x18,White);
				  ssd1306_SetCursor(0,15);
				  ssd1306_WriteString("Zavershen",Font_11x18,White);
				  ssd1306_SetCursor(0,30);
				  if(flagstart==2){
				  ssd1306_WriteString("Uspeshno",Font_11x18,White);
				  }else if(flagstart==3){
					  ssd1306_WriteString("po trebovaniu",Font_11x18,White);
				  }
				  ssd1306_SetCursor(0,45);
				  ssd1306_WriteString("Otmena",Font_11x18,White);
				  ssd1306_UpdateScreen();

			  }
		  }
	  }//�� if ������� ������� - ������ ������
	if(flagstart==0){//��������� ���� ������ ��� ��������� � �������
		status=0;   //������ �������� ����
	}else if(flagstart==2){
		status=1;   //��� ������ ����� ������ �������
	}else if(flagstart==3){
		 status=3;   //����� ������� � ������
	}
	return status;
}
//--------------------------------------------------------------------------------
//������� ������� ������� ��������� �������
void action_Valve(){
	uint8_t interval;
	uint8_t lastsec;
	uint8_t secun;
	uint8_t startt;
	char intervalchar[9];


	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //������ �������
	lastsec=sTime.Seconds;
	startt=lastsec;
	Poliv(ON);//�������� �����
	while(HAL_GPIO_ReadPin(GPIOB,second_button_Pin)==GPIO_PIN_RESET){//���� ����� ������ 2 ������� ����� ������� ���������
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //������ �������
		secun=sTime.Seconds;
		if(lastsec!=secun){
			interval=secun-startt;
		}
		snprintf(intervalchar,8,"time_%d",interval);
		//��������� ������� ������� ������� ������
					ssd1306_Fill(Black);
					ssd1306_SetCursor(0,0);
					ssd1306_WriteString("Rucn_regim",Font_11x18,White);
					ssd1306_SetCursor(0,15);
					ssd1306_WriteString(intervalchar,Font_11x18,White);
					ssd1306_SetCursor(0,30);
					ssd1306_WriteString("Poliv_ON",Font_11x18,White);
					ssd1306_SetCursor(0,45);
					ssd1306_WriteString("Otmena",Font_11x18,White);
					ssd1306_UpdateScreen();

	}
	Poliv(OFF);//��������� ����� ��� ������
}
//---------------------------------------------------------------------------------
//������� ��������� ������� ������
void Poliv(uint8_t status){  //�� ���� ��������  ������ ��� ����� ��� ��� ��������� ������
	if(status==1){
		HAL_GPIO_WritePin(GPIOC,valve_Pin,GPIO_PIN_SET);
	}
	if(status==0){
		HAL_GPIO_WritePin(GPIOC,valve_Pin,GPIO_PIN_RESET);
	}
}
//---------------------------------------------------------------------------------
//������� ��������� ������� � ������ ������ �� ������������ ����������� �������
void action_Valvetotime(uint8_t timeact){
	uint16_t secund_interval;
	uint16_t secund;
	uint16_t lastsecund;
	uint16_t countsec;   //������� ������
	uint8_t startsec;
	uint8_t startmin;
	char intreval[6];
	char ostatok[14];
	char countchar[6];

				 //0-60
	secund_interval=timeact*60;  //��������� ������ � ������� ��� ��������� ������
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //���������� ������� �������
	startsec=sTime.Seconds;   //���������� ����� ������ ���
	startmin=sTime.Minutes;   //���
	secund=secund_interval;
	lastsecund=startsec;
	Poliv(ON);          //�������� �����
	while(secund>1){    //���������  �������� ������
       // HAL_Delay(1000);
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //������ �������
		countsec=(sTime.Minutes*60+sTime.Seconds)-(startmin*60+startsec);//��������� ������� ������� ������
		if(lastsecund!=countsec){
			lastsecund=countsec;
			secund=secund_interval-countsec; //�������� ������ � ��������
		}

	    //��������� �������
		    //��������� ����
		    snprintf(intreval,5,"%d",secund_interval); //��� ������ ���������� ���������
		    snprintf(ostatok,13,"Ostalos_%d",secund);// ������� ��������
            snprintf(countchar,5,"_%d",countsec);
			ssd1306_Fill(Black);
			ssd1306_SetCursor(0,0);
			ssd1306_WriteString(intreval,Font_11x18,White);
			ssd1306_SetCursor(0,15);
			ssd1306_WriteString(ostatok,Font_11x18,White);
			ssd1306_SetCursor(0,30);
			ssd1306_WriteString("Poliv_ON",Font_11x18,White);
			ssd1306_SetCursor(0,45);
			ssd1306_WriteString("Otmena",Font_11x18,White);
			ssd1306_UpdateScreen();
            if(press_Button()==41){
            	secund=0;
            }

	}
	Poliv(OFF);          //��������� ����� ����� ������ �� ����� ������� �������
	    //����� ��� ������ ��������
				ssd1306_SetCursor(0,0);
				ssd1306_WriteString(intreval,Font_11x18,White);
				ssd1306_SetCursor(0,15);
				ssd1306_WriteString(ostatok,Font_11x18,White);
				ssd1306_SetCursor(0,30);
				ssd1306_WriteString("Poliv_OFF",Font_11x18,White);
				ssd1306_SetCursor(0,45);
				ssd1306_WriteString("Otmena",Font_11x18,White);
				ssd1306_UpdateScreen();
	HAL_Delay(2000);
}
//---------------------------------------------------------------------------------
//������� ��� ����������� ������� �����
char what_Time_ofday(uint8_t Hours){
	char* what_time;
	if((Hours>=6)&&(Hours<11)){
		what_time="Utro";
	}else if((Hours>=11)&&(Hours<17)){
		what_time="Den'";
	}else if((Hours>=17)&&(Hours<23)){
		what_time="Vecher";
	}else if((Hours>=23)&&(Hours<=24)){
		what_time="Noch";
	}else if((Hours>=0)&&(Hours<6)){
		what_time="Noch";
	}

	return what_time;
}
//------------------------------------------------------------------------------------
//������� �������� �����
uint8_t setHoursmethod(uint8_t hour){
	//��������� �������
	 //��������� ������
	 //�������� ������� ���
	  //��������� �������
	  //��� ������ ���������� �������� ����� �������� ���������
		 uint8_t new_hours;
	   	 char tekushee_znachenie[12];
		 char new_znachenie[12];


		 snprintf(tekushee_znachenie,12,"%d_Old_min",hour); //������������� � text
		 	 	 	 snprintf(new_znachenie,12,"%d_New_hour",hour); //�� ����� ���������� �������� ����
		 	 	 	new_hours=hour;  //�� ����� �������� ���� ����������
		 	 	 	 //��������� �������
		 	 	 	ssd1306_SetCursor(0,0);
		 	 	 	ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,15);
		 	 	 	ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,30);
		 	 	 	ssd1306_WriteString("1&2 Change",Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,45);
		 	 	 	ssd1306_WriteString("Exit&Save",Font_11x18,White);
		 	 	 	ssd1306_UpdateScreen();
		 while(press_Button()!=41){  //���� �� ������ ������ 4
		 	 	 	if(press_Button()==11){
		 	 	 		 	if(new_hours>0){
		 	 	 		 	new_hours--;
		 	 	 		 	}else{
		 	 	 		 	new_hours=24;
		 	 	 		 	}
		 	 	 	}else if(press_Button()==21){
		 	 	 		 	if(new_hours<24){
		 	 	 		 	new_hours++;
		 	 	 		 	}else{
		 	 	 		 	new_hours=0;
		 	 	 		 			}
		 	 	 		 	}
		 	 	 	snprintf(new_znachenie,12,"%d_New_hour",new_hours);//��������� ��������
		 	 	 	 // ������������� ����� �� �������
		 	 	 	ssd1306_SetCursor(0,0);
		 	 	 	ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,15);
		 	 	 	ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,30);
		 	 	 	ssd1306_WriteString("1&2 Change",Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,45);
		 	 	 	ssd1306_WriteString("Exit&Save",Font_11x18,White);
		 	 	 	ssd1306_UpdateScreen();

		 }//�� while
		           //�������� ����� ��� �������� ��������
		 	 		ssd1306_SetCursor(0,0);
		 	 		ssd1306_WriteString("Novoe znachen",Font_11x18,White);
		 	 		ssd1306_SetCursor(0,15);
		 	 		ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 		ssd1306_SetCursor(0,30);
		 	 		ssd1306_WriteString("Ustanovleno",Font_11x18,White);
		 	 		ssd1306_SetCursor(0,45);
		 	 		ssd1306_WriteString("Exit",Font_11x18,White);
		 	 		ssd1306_UpdateScreen();
		 	 		HAL_Delay(3000);


	return new_hours;
}
//-------------------------------------------------------------------------------------
//������� ��������� �����
uint8_t setMinutsmethod(uint8_t minuts){
	//��������� �������
	 //��������� ������
	 //�������� ������� ���
	  //��������� �������
	  //��� ������ ���������� �������� ����� �������� ���������
		 uint8_t new_minuts;
	   	 char tekushee_znachenie[12];
		 char new_znachenie[12];


		 snprintf(tekushee_znachenie,12,"%d_Old_min",minuts); //������������� � text
		 	 	 	 snprintf(new_znachenie,12,"%d_New_min",minuts); //�� ����� ���������� �������� ����
		 	 	 	new_minuts=minuts;  //�� ����� �������� ���� ����������
		 	 	 	 //��������� �������
		 	 	 	ssd1306_SetCursor(0,0);
		 	 	 	ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,15);
		 	 	 	ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,30);
		 	 	 	ssd1306_WriteString("1&2 Change",Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,45);
		 	 	 	ssd1306_WriteString("Exit&Save",Font_11x18,White);
		 	 	 	ssd1306_UpdateScreen();
		 while(press_Button()!=41){  //���� �� ������ ������ 4
		 	 	 	if(press_Button()==11){
		 	 	 		 	if(new_minuts>0){
		 	 	 		 	new_minuts--;
		 	 	 		 	}else{
		 	 	 		 	new_minuts=59;
		 	 	 		 	}
		 	 	 	}else if(press_Button()==21){
		 	 	 		 	if(new_minuts<59){
		 	 	 		 	new_minuts++;
		 	 	 		 	}else{
		 	 	 		 	new_minuts=0;
		 	 	 		 			}
		 	 	 		 	}
		 	 	 	snprintf(new_znachenie,12,"%d_New_min",new_minuts);//��������� ��������
		 	 	 	 // ������������� ����� �� �������
		 	 	 	ssd1306_SetCursor(0,0);
		 	 	 	ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,15);
		 	 	 	ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,30);
		 	 	 	ssd1306_WriteString("1&2 Change",Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,45);
		 	 	 	ssd1306_WriteString("Exit&Save",Font_11x18,White);
		 	 	 	ssd1306_UpdateScreen();

		 }//�� while
		           //�������� ����� ��� �������� ��������
		 	 		ssd1306_SetCursor(0,0);
		 	 		ssd1306_WriteString("Novoe znachen",Font_11x18,White);
		 	 		ssd1306_SetCursor(0,15);
		 	 		ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 		ssd1306_SetCursor(0,30);
		 	 		ssd1306_WriteString("Ustanovleno",Font_11x18,White);
		 	 		ssd1306_SetCursor(0,45);
		 	 		ssd1306_WriteString("Exit",Font_11x18,White);
		 	 		ssd1306_UpdateScreen();
		 	 		HAL_Delay(3000);


	return new_minuts;
}
//-----------------------------------------------------------------------------------------
//������� ������� ��������� ��������� ������ ��� ��������� ���
uint16_t setHolod2311(uint16_t current_min){//
	  //��������� �������
	  //��������� ������
	  //�������� ������� ���
	  //��������� �������
	  //��� ������ ���������� �������� ����� �������� ���������
	 uint16_t new_min_interval;
	 char tekushee_znachenie[12];
	 char new_znachenie[12];

	 	 	 snprintf(tekushee_znachenie,12,"%d_Old_min",current_min); //������������� � text
	 	 	 snprintf(new_znachenie,12,"%d_New_min",current_min); //�� ����� ���������� �������� ���
	 	 	new_min_interval=current_min;  //�� ����� �������� ��� ����������
	 	 	 //��������� �������
	 	 	 	 	ssd1306_SetCursor(0,0);
	 				ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
	 				ssd1306_SetCursor(0,15);
	 				ssd1306_WriteString(new_znachenie,Font_11x18,White);
	 				ssd1306_SetCursor(0,30);
	 				ssd1306_WriteString("1&2 Change",Font_11x18,White);
	 				ssd1306_SetCursor(0,45);
	 				ssd1306_WriteString("Exit&Save",Font_11x18,White);
	 				ssd1306_UpdateScreen();
	 		while(press_Button()!=41){  //���� �� ������ ������ 4
	 				if(press_Button()==11){
	 					if(new_min_interval>0){
	 					new_min_interval--;
	 					}else{
	 						new_min_interval=0;
	 					}
	 				}else if(press_Button()==21){
	 					if(new_min_interval<719){
	 					new_min_interval++;
	 					}else{
	 						new_min_interval=720;
	 					}
	 				}
	 				 snprintf(new_znachenie,12,"%d_New_min",new_min_interval);//��������� ��������
	 			     // ������������� ����� �� �������
	 				 	 	 	 	 	ssd1306_SetCursor(0,0);
	 					 				ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
	 					 				ssd1306_SetCursor(0,15);
	 					 				ssd1306_WriteString(new_znachenie,Font_11x18,White);
	 					 				ssd1306_SetCursor(0,30);
	 					 				ssd1306_WriteString("1&2 Change",Font_11x18,White);
	 					 				ssd1306_SetCursor(0,45);
	 					 				ssd1306_WriteString("Exit&Save",Font_11x18,White);
	 					 				ssd1306_UpdateScreen();
	 		}

	 		//�������� ����� ��� �������� ��������
	 		ssd1306_SetCursor(0,0);
	 		ssd1306_WriteString("Novoe znachen",Font_11x18,White);
	 		ssd1306_SetCursor(0,15);
	 		ssd1306_WriteString(new_znachenie,Font_11x18,White);
	 		ssd1306_SetCursor(0,30);
	 		ssd1306_WriteString("Ustanovleno",Font_11x18,White);
	 		ssd1306_SetCursor(0,45);
	 		ssd1306_WriteString("Exit",Font_11x18,White);
	 		ssd1306_UpdateScreen();
	 		HAL_Delay(3000);

	return new_min_interval;
}
//-------------------------------------------------------------------------------------
//����������� ����������� ���������� ��������������� ������ ������
//���������� ����� �������� �����
//������� �������� ������� �������
void soberiMenu23233(uint8_t hour,uint8_t min,uint16_t timeperiodOn,uint16_t pererivOFF){
	char time_start[12];
	char param_start[12];
	 	 	 	 snprintf(time_start,12,"%d:%d",hour,min);
		int i=3;
	 	 	while(i>0){
	 	 	    ssd1306_SetCursor(0,0);
		 		ssd1306_WriteString(time_start,Font_11x18,White);
		 		ssd1306_SetCursor(0,15);
		 		ssd1306_WriteString("Vremya starta",Font_11x18,White);
		 		ssd1306_SetCursor(0,30);
		 		ssd1306_WriteString("Ustanovleno",Font_11x18,White);
		 		ssd1306_SetCursor(0,45);
		 		ssd1306_WriteString("Exit",Font_11x18,White);
		 		ssd1306_UpdateScreen();
		 		HAL_Delay(2000);
		 		ssd1306_Fill(Black);
		 		ssd1306_UpdateScreen();
		 		HAL_Delay(1000);
		 		i--;
	 	 	}
	 	 	i=3;
	 	 	 snprintf(param_start,12,"%d:%d",timeperiodOn,pererivOFF);
	 	 	while(i>0){
	 	 		 	 	    ssd1306_SetCursor(0,0);
	 	 			 		ssd1306_WriteString(param_start,Font_11x18,White);
	 	 			 		ssd1306_SetCursor(0,15);
	 	 			 		ssd1306_WriteString("poliv&ogidanie",Font_11x18,White);
	 	 			 		ssd1306_SetCursor(0,30);
	 	 			 		ssd1306_WriteString("Ustanovleno",Font_11x18,White);
	 	 			 		ssd1306_SetCursor(0,45);
	 	 			 		ssd1306_WriteString("Exit",Font_11x18,White);
	 	 			 		ssd1306_UpdateScreen();
	 	 			 		HAL_Delay(2000);
	 	 			 		ssd1306_Fill(Black);
	 	 			 		ssd1306_UpdateScreen();
	 	 			 		HAL_Delay(1000);
	 	 			 		i--;
	 	 		 	 	}
}
//--------------------------------------------------------------------------------------
void soberiMenu23232(struct current_line *setline){
		setline->line1="Hours  ";
		setline->line2="Min  ";
		setline->line3="----------";
		setline->line4="Otmena";
}

//-------------------------------------------------------------------------------------
void soberiMenu23231(struct current_line *setline){
	setline->line1="Hours  ";
	setline->line2="Min  ";
	setline->line3="----------";
	setline->line4="Otmena";

}
//-------------------------------------------------------------------------------------
void soberiMenu2323(struct current_line *setline){
						setline->line1="Time current ";
						setline->line2="Time start ";
						setline->line3="Look tStart";
						setline->line4="Otmena";
}
//------------------------------------------------------------------------------------
//������� ������ ���� ��� ��������� ��� ����� ��� �������������� ������
void soberiMenu2320(struct current_line *setline){
					setline->line1="Podacha ";
					setline->line2="Pereriv ";
					setline->line3="Time settings";
					setline->line4="Otmena";
}
//-------------------------------------------------------------------------------------
//������� ������ ���� 231
void soberiMenu231(struct current_line *setline){
	            setline->line1="Holodnyi ";
				setline->line2="Garkiy ";
				//setline->line3="Skolko raz";
				setline->line3="Skolko min";
				setline->line4="Otmena";
}

//-------------------------------------------------------------------------------------
//������� ������ ���� 221
void soberiMenu221(struct current_line *setline){
			setline->line1="Ruchnoy ";
			setline->line2="regim ";
			setline->line3="Poliv_ON";
			setline->line4="Otmena";
}
//--------------------------------------------------------------------------------------
//������� ������ ���� 211
void soberiMenu211(struct current_line *setline){
		setline->line1="Min_";
		setline->line2="Ostalos ";
		setline->line3="Poliv_ON";
		setline->line4="Otmena";
}
//------------------------------------------------------------------------------------
//������� ������ ��������� ���� 20
void soberiMenu20(struct current_line *setline){

	 setline->line1="Rucnoy time";
	 setline->line2="Rucnoy ";
	 setline->line3="Ustanovki";
	 setline->line4="Otmena";
}
 //---------------------------------------------------------
//��������� ������� ��� �������� �����������
void migni(uint8_t count_blink,uint16_t time_action,uint16_t time_sleep){


	while(count_blink>0){
		HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_SET);
		HAL_Delay(time_action);
		HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_RESET);
		HAL_Delay(time_sleep);
		count_blink--;
	}
}


//-----------------------------------------------------------------------------
void startBlinkLed(){
	//������� ������� ���������� ��� ������
	uint8_t m,n,t1,t2,t3;
	m=4;
	n=5;
	t1=50;
    t2=70;
    t3=2000;
	while(m>0){
			 HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_SET);
			 HAL_Delay(t1);
			 HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_RESET);
			 HAL_Delay(t1);
		m--;
	}
	while (n>0){
			 HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_SET);
			 HAL_Delay(t2);
			 HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_RESET);
			 HAL_Delay(t2);
		n--;
	}
	HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_SET);
    HAL_Delay(t3);
    HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_RESET);
    HAL_Delay(t3);
    HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_SET);
        HAL_Delay(t3);
        HAL_GPIO_WritePin(GPIOC,LED1_Pin,GPIO_PIN_RESET);
        HAL_Delay(t3);

}
void blinktext(){
	    uint16_t t1,t2;
	    t1=1000;
	    t2=500;
		ssd1306_Fill(Black);                             //�����
	    ssd1306_SetCursor(2, 0);
	    ssd1306_WriteString("Yahuuu", Font_16x26, White);
	    ssd1306_UpdateScreen();
	    HAL_Delay(t1);                                 //����� �������
	    ssd1306_Fill(Black);                           //�������
	    ssd1306_UpdateScreen();
	    HAL_Delay(t2);                                //������

	    ssd1306_Fill(Black);                                     //�����
	    ssd1306_SetCursor(2, 0);
	    ssd1306_WriteString("Yahuuu", Font_16x26, White);
	    ssd1306_UpdateScreen();
	    HAL_Delay(t1);                                                 //����� �������
	    ssd1306_Fill(Black);                                             //�������
	    ssd1306_UpdateScreen();                                         //������
	    HAL_Delay(t2);

	    ssd1306_Fill(Black);                                           //����� � �������� ������
	    ssd1306_SetCursor(2, 26);
	    ssd1306_WriteString("Yahuuu", Font_16x26, White);
	    ssd1306_UpdateScreen();
	    HAL_Delay(t1);
	    ssd1306_Fill(Black);                                             //�������
	    ssd1306_UpdateScreen();                                         //������
	    HAL_Delay(t2);
	    ssd1306_Fill(Black);                                           //����� � �������� ������
	    ssd1306_SetCursor(2, 26);
	    ssd1306_WriteString("Yahuuu", Font_16x26, White);
	    ssd1306_UpdateScreen();
	    HAL_Delay(t1);
	    ssd1306_Fill(Black);                                             //�������
	   	ssd1306_UpdateScreen();                                         //������
	    HAL_Delay(t2);
	   /* ssd1306_SetCursor(2, 26);
	    ssd1306_WriteString("Font 11x18", Font_11x18, White);
	    ssd1306_SetCursor(2, 26+18);
	    ssd1306_WriteString("Font 7x10", Font_7x10, White);
	    ssd1306_SetCursor(2, 26+18+10);
	    ssd1306_WriteString("Font 6x8", Font_6x8, White);
	    ssd1306_UpdateScreen();*/
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char *textMainmenu[4]={">  Set1     ",">  Set2",">  Set3",">  Set3"};
    char *buildMainmenu[4]={"Time","Time_D","Temp_25C","Status_OFF"};
    char *buildmenu2[4]={"Ruchnoy t","Ruchnoy","Ustanovki","Otmena"};
    char *buildmenu={"s","s","s","s"};
    uint8_t error_code=0;   //������� ����� ������  0- ��� ������ 1-���� ������ �������
    float Upitanija=3.195;       //��� ������� ���������� ���
    uint16_t razradnostAcp=4096;    //����������� ���
    float udatchika_temp=0;   //���������� �� ����� � ������� �����������  tmp36gz
    uint16_t Udispint=0;
    float ktmp36gz=34.78; //����������� ������� �����������
    float temp=0;    //����������� � �������
    uint8_t status;      //��� ds18b20
    uint8_t dt[8];      //��� ds18b20
    uint16_t raw_temper;   //��� ds18b20
    float temper;        //��� ds18b20
    char c;
    uint8_t state_button;   //������ ������� ������  1- ������ ��1 2 -������ ��2 3 ������ ��3 4 ������ ��4 0- �� ������ ��
    uint16_t level_menu;     //������� ���� ������������ �� LCD 10(1.0) ���� �� ���������
                                            //  20(2.0) ����
    										//  21(2.1)
                                            //  22(2.2)

     char *metka;
     uint8_t polivcomlete_flag=0;  //���� ��� ������� �������� (0,1) ������������ � 0 � 11-59 �������� � ������� autoRegim
     uint8_t sostoyaniepoliva;    //��������� ������ ������� ����������
     uint16_t holodny_periodmin=5;  //����� ������ � �������� �����
     uint16_t garky_periodmin=7;    //����� ������ � ������ ������
     uint16_t interval_autoregMin=10;   //  ����� ���������� ��� ������� ������ ���� ����������� ����
     uint16_t pereriv_intervalMinforauto=5;  //����� ��������� ���������� ��� ��������� ������ ����� ���� ��������� � �� �������
	 uint8_t hoursstartpolivavto=18;    //����� ������ ������  �����
	 uint8_t minutstartpolivavto=10;    //���
	 uint8_t hours_systemtime;    //���������� ��� ��������� ���������� �������
	 uint8_t minut_systemtime;    //���������� ��� ��������� ���������� �������
     /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  startBlinkLed();
  ssd1306_Init();
  //HAL_Delay(3000);
  //startBlinkLed();
  ssd1306_Fill(White);
  ssd1306_UpdateScreen();
  startBlinkLed();
 // ssd1306_Fill(White);
  //ssd1306_UpdateScreen();
 // HAL_Delay(3000);
 // startBlinkLed();
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
 // HAL_Delay(3000);
 // ssd1306_TestFPS();
  //HAL_Delay(3000);
 // startBlinkLed();
 // ssd1306_TestFonts();
 //ssd1306_TestFPS();

  //HAL_Delay(3000);
  //ssd1306_TestFonts();
  //�������� ������� ���� �������
  struct current_line mainmenu;

  mainmenu.line1="> set1";
  mainmenu.line2="  set2";
  mainmenu.line3="  set3";
  mainmenu.line4="  set4";

  update_LCD(&mainmenu);

  port_init();  //������������� ���� ��� DS18B20 ���� � ���� 3
  level_menu=10; //������� ����������� ���� -�� ���������
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	     //� ������ ���� ������������ 30 ���
	     //� �������� ��� 15 - 10 ���
	     //-------------------------------------
         //������������� ������� ����
	     //      1.0 default
	     //        |
	     //      2.0 menu
	     //        |---2.1 Hand action time limit
 	     //        |     |----2.1.1 Zapusk Timera
	     //        |---2.2 Hand action On/Off
	     //        |     |----2.2.1 Ruchnoe vkluchenie klapana
	     //        |---2.3 Settings
	     //        |     |----2.3.1 Nastroiki info
	     //        |     |      |-----2.3.1.1 �������� ���� ����� ��������� ������
		 //		   |	 |		|-----2.3.1.2 ������ ���� ����� ��������� ������
		 //		   |	 |
	     //        |     |----2.3.2 ������� ��� ��������
	     //        |     |----2.3.3 ��������� �����
	     //        |
	     //        |---2.4 Cancel
	     //
	    //----------------------------------------------
	  // � ������ �� ���������   1.0 ������� ����
	                        // 1.�����
	                        // 2.����� �����
	                        // 3.�����������
	                        // 4.������  ��� ������
	                        //
	  //��� ������� �� ������ ��������� � ����� ����     2.0 ������� ����
	                        // 1.������ ������ �� ����� t
	                        // 2.������ ������ �� �������
	                        // 3.���������
	                        // 4.������� � ���� �� ���������
	  //----------------------------------------------------------------------------------------------------------------------
	  //������ ������ �� ����� t   2.1   ������� ����
	                       //��� ����� ����� ��������� �������
	                       //LCD 1.������ ������� ��������
	                          // 2.����������� ������ ������� t ����
	                          // 3.������ ������ ��.��� �������
	                          // 4.������
	                       //�� ��������� ������� � ���� �� �������� ����� ����� �� ������������
	  //������ ������ �� �������  2.2  ������� ����
	                      //��� ����� ����� ���������� ������
	                      //LCD  1. ������ ������� ������
	                        //   2. ����� "������"
	                        //   3. ������ ������ ��.���
	                        //   4. �����������
	                      //��� ���������� ������ ������� � ���� �� ���������// � ����� ����?
	  //���������                2.3  ������� ����
	                     //LCD
	                     //1.��������� ����� ������� ������         2.3.1  ������� ����
	                                                       //1.������ ��������     2.3.1.1
	                                                                          //1.������� �� �������������
	                                                                          //2.�������������
	                                                                          //3.��1 ++    ��2--   ��3 ���������
	                                                                          //4.������
	                                                                          //������ 3 ��������� ����� ���������� ��� ������� �������� �������� � �����������
	                                                       //2.������ ������    2.3.1.2 ������� ����
	                                                                               //1.������� �� �������������
	  	                                                                          //2.�������������
	  	                                                                          //3.��1 ++    ��2--   ��3 ���������
	  	                                                                          //4.������
	  	                                                                          //������ 3 ��������� ����� ���������� ��� ������� �������� �������� � �����������
	                                                       //3.                 2.3.1.3 ������� ����
	                                                       //4.������           2.3.1.4 ������� ����
	                     //2.��������� ���-�� ������        2.3.2
	                                                //1.������� �� �������������
	  	                                            //2.�������������
	  	                                            //3.��1 ++    ��2--   ��3 ���������
	  	                                            //4.������
	  	                                            //������ 3 ��������� ����� ���������� ��� ������� �������� �������� � �����������
	                     //3.��������� �������              2.3.3
	                                          //1.�����  ����:���
	                                          //2.�����  ����:���  �������������
	  	                                      //3.��1 ++    ��2--   ��3 ����������� ��������� ���� -���
	  	                                      //4.������ ��� ����������
	  	                                   //���������� ��� ������ �� ��4
	                     //4.������                        2.3.4
	                  //�� ������ ��� ����� ������ ������� � ���� �� ���������

	  //�������� ����
	       //structura �� ����������
	       //������ � ��� ������ ����

	  //�������� ������� ������
//--------------------------------------------------------------------------------
	  //������ ���������
//����� ������
	   int lii=50;
	  if(press_Button()==11){
		  state_button=1;
		  migni(1,lii,lii);
	  }else if(press_Button()==21){
		  state_button=2;
		  migni(2,lii,lii);
	  }else if(press_Button()==31){
		  state_button=3;
		  migni(3,lii,lii);
	  }else if(press_Button()==41){
		  state_button=4;
		  migni(4,lii,lii);
	  }else{
		  state_button=0;
	  }


     //���� ���� ������ ������ �� ��������� ��� ������� ���� � ��������� �� ��������
	  if(state_button!=0){
		  //�������� ������� ������� ������ ����� �� ������ ��� �������
		  level_menu=nextLevel(level_menu,state_button);
		  state_button=0;
	  }





	  if(level_menu==10){  //���������� ���� �� default
//���� �� ��������� 1.0
//1.������� ������� ��������� mainmenu
//2.�������� �����
	  //��� ��������� ����������: ����� ����-���-���   1� �������
	                        //    ����� ����� ����-����-�����-����    2� ������
	                        //    ��������� ������� �����������       3� �������
	                        //    ������ ������ �� ������   ON-OFF   ��� ��� ������ 4� ������

	  //��������� �������� ������� ��� ���������������� ������ �������
	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); // RTC_FORMAT_BIN , RTC_FORMAT_BCD
	  snprintf(trans_str, 63, " %d:%d:%d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
	  mainmenu.line1=trans_str;
	  //update_LCD(mainmenu);
	  //��������� ������ ������ ���� ����
	   //�����  �����  ���� � 6-00 �� 11-00
	                  //���� � 11-00 �� 17-00
	                  //����� � 17-00 �� 23-00
	                  //����  � 23-00 �� 6-00
	  if((sTime.Hours>=6)&&(sTime.Hours<11)){
		  mainmenu.line2="  Utro";
	  	}else if((sTime.Hours>=11)&&(sTime.Hours<17)){
	  		mainmenu.line2=" Den'";
	  	}else if((sTime.Hours>=17)&&(sTime.Hours<23)){
	  		mainmenu.line2="  Vecher";
	  	}else if((sTime.Hours>=23)&&(sTime.Hours<=24)){
	  		mainmenu.line2="  Noch";
	  	}else if((sTime.Hours>=0)&&(sTime.Hours<6)){
	  		mainmenu.line2="  Noch";
	  	}
	  //mainmenu.line2=what_Time_ofday(sTime.Hours);
	  //update_LCD(mainmenu);

	  //��������� ��������� ������� �����������

	  mainmenu.line3="T'C_23.8C";
/*	  ��� �������� �������
	  ds18b20_MeasureTemperCmd(SKIP_ROM, 0);
	  HAL_Delay(800);
	  ds18b20_ReadStratcpad(SKIP_ROM, dt, 0);
	  sprintf(str1,"STRATHPAD: %02X %02X %02X %02X %02X %02X %02X %02X; ",
	  dt[0], dt[1], dt[2], dt[3], dt[4], dt[5], dt[6], dt[7]);
	//  HAL_UART_Transmit(&huart1,(uint8_t*)str1,strlen(str1),0x1000);
	  raw_temper = ((uint16_t)dt[1]<<8)|dt[0];
	  if(ds18b20_GetSign(raw_temper)) c='-';
	  else c='+';
	  temper = ds18b20_Convert(raw_temper);
	  Udispint=temper*100;
	  snprintf(data_str,63,"%d\n",Udispint);
	  mainmenu.line3=data_str;
	//sprintf(str1,"Raw t: 0x%04X; t: %c%.2frn", raw_temper, c, temper);
	//  HAL_UART_Transmit(&huart1,(uint8_t*)str1,strlen(str1),0x1000);
	  HAL_Delay(150);
*/



/*
	  HAL_ADC_Start(&hadc1);      //��������� ��������� ����������� �����
	  HAL_ADC_PollForConversion(&hadc1, 100);    //���������� ��������� ��������
	  adcResult = HAL_ADC_GetValue(&hadc1);     //�������� ��������
	  HAL_ADC_Stop(&hadc1);                   //������������� ���������
	  udatchika_temp=adcResult*Upitanija/razradnostAcp;
	  Udispint=(int)(udatchika_temp*1000-500);
	  temp=Udispint/ktmp36gz;
	  Udispint=temp*100;
	  snprintf(data_str,63,"%d\n",Udispint);
	  // snprintf(data_str,63,"%d\n",adcResult);
	  mainmenu.line3=data_str;
*/

	  //��������� ������� ������
	  if(error_code==0){
		  if(HAL_GPIO_ReadPin(GPIOC,valve_Pin)==GPIO_PIN_SET){//
			  //mainmenu.line4="Poliv_ON,";
			  snprintf(data_str, 63, "Poliv_ON_%d",polivcomlete_flag );
			  mainmenu.line4=data_str;
		  }else{
			  //mainmenu.line4="Poliv_OFF,OK";
			  snprintf(data_str, 63, "Poliv_OFF_%d",polivcomlete_flag );
			  mainmenu.line4=data_str;
		  }
	  }else{
		 // snprintf(data_str, 63, " %d-%d-20%d\n", DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year);
		  snprintf(data_str, 63, "Code-%d\n",error_code );
		  mainmenu.line4=data_str;
	  }
	  update_LCD(&mainmenu);
	  //����������� ������ � ��������
	  //������� ����� ��� ���������� 18-00 �������� �� 8 ���

	  sostoyaniepoliva=autoRegim(interval_autoregMin,pereriv_intervalMinforauto,polivcomlete_flag,hoursstartpolivavto,minutstartpolivavto); //������� � ������� ���� ������� ��� �������� � ���� �������� ������� ��� ���
	  if(sostoyaniepoliva==1){  //
		  polivcomlete_flag=1;      //������ ���� ��� ����� ���������� �������� ������ �� ����
		   }else if(sostoyaniepoliva==3){
		  polivcomlete_flag=1;      //������ ���� ��� ����� ������� ������� ������ �� ��������
	  }
	  //������� �������� ������� 11-59 ��� ������  ����� ������ �� ������
	  polivcomlete_flag=getNewflagforpoliv(polivcomlete_flag);
	  //HAL_Delay(500);
//---------------------------------------------------------------------------------------------------------------
	  //������ ������� �������
   //blinktext();
/*
            HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); // RTC_FORMAT_BIN , RTC_FORMAT_BCD
          //  snprintf(trans_str, 63, "Time %d:%d:%d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
         //   HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
            snprintf(trans_str, 63, " %d:%d:%d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
            		ssd1306_Fill(Black);                             //�����
            	    ssd1306_SetCursor(2, 0);
            	    ssd1306_WriteString("Time", Font_11x18, White);
            	    ssd1306_SetCursor(2, 18);
            	    ssd1306_WriteString(trans_str, Font_11x18, White);
            	    //ssd1306_WriteString("Yahuuu", Font_16x26, White);
            	    //ssd1306_UpdateScreen();


            HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
        //    snprintf(trans_str, 63, "Date %d-%d-20%d\n", DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year);
         //   HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
            snprintf(data_str, 63, " %d-%d-20%d\n", DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year);
            ssd1306_SetCursor(2, 36);
            ssd1306_WriteString(data_str, Font_11x18, White);
            ssd1306_UpdateScreen();
            HAL_Delay(100);
             */
	  }//�� if level_menu==10 ��������� ����
//--------------------------------------------------------
	  //���� � ���� 2.0
	  // 1.������ ������ �� ����� t
	  // 2.������ ������ �� �������
	  // 3.���������
	  // 4.������� � ���� �� ���������
	  if(level_menu==20){ //���� � ����
		  metka=&mainmenu;
		  soberiMenu20(metka);
		  update_LCD(metka);
	  }



	  //���� �  2.1.1 ������ ������
	  if(level_menu==211){
		  soberiMenu211(metka);
		  update_LCD(metka);
		  action_Valvetotime(time_poliv);  //��������� ������ �� �� ���������� ����� �������� � ���
		  level_menu=10;
	  }

	  //���� � 2.2.1
	  if(level_menu==221){
		  	  	  soberiMenu221(metka);
		  		  update_LCD(metka);
		  		  action_Valve();   //
		  		  level_menu=20;
	  	  }

	  //���� � 2.3.1
	   if(level_menu==231){
		   	   soberiMenu231(metka);
		   	   update_LCD(metka);
	  	  	  }
     //���� � 2.3.1.1     //��������� ��� ��� ������ � �������� ����
	   if(level_menu==2311){
		   holodny_periodmin=setHolod2311(holodny_periodmin);   //������� � ������� ��������� ��� ��������� ������� ������
		   level_menu=231;  // ������������� � ���� ����������
	   }
	   //���� � 2.3.1.2  //��������� ��� ��� ������ � ������ ����
	   if(level_menu==2312){
		   garky_periodmin=setHolod2311(garky_periodmin);   //������� � ������� ��������� ��� ��������� ������� ������
		   level_menu=231;  // ������������� � ���� ����������
	   }
	   //���� � 2.3.2.0  //��������� ������� ��� �������� � ������������� ������ ��� �����������
	   if(level_menu==2320){
	   		   	   soberiMenu2320(metka);
	   		   	   update_LCD(metka);
	   	  	  	  }
	   //���� � 2.3.2.1.   //��������� ��� ��� ������ � �������������� ������ ��� �����������
	   if(level_menu==2321){
		   interval_autoregMin=setHolod2311(interval_autoregMin);   //������� � ������� ��������� ��� ��������� ������� ������
	   		   level_menu=231;  // ������������� � ���� ����������
	   	   }
	   //���� � 2.3.2.1.   //��������� ��� ��������� ������� � �������������� ������ ��� �����������
	   	   if(level_menu==2322){
	   		pereriv_intervalMinforauto=setHolod2311(pereriv_intervalMinforauto);   //������� � ������� ��������� ��� ��������� ������� ������
	   	   		   level_menu=231;  // ������������� � ���� ����������
	   	   	   }
	   //���� � 2.3.2.3	 //��������� ����� ��������� ������� ������ ����� ����
	   	if(level_menu==2323){
	   				soberiMenu2323(metka);
	   		  	    update_LCD(metka);
	   	}
	   	//���� � 2.3.2.3.1.   //��������� ������� �������
	   	if(level_menu==23231){
	   			soberiMenu23231(metka);
	   			update_LCD(metka);
	   	}
		//���� � 2.3.2.3.2.   //��������� ������� ������ ������
	   	if(level_menu==23232){
	   		   			soberiMenu23232(metka);
	   		   			update_LCD(metka);
	   		   	}
        //���� � 23236  ����� ������ ������ //���������� �������� ����
	   	if(level_menu==23236){
	   		hoursstartpolivavto=setHoursmethod(hoursstartpolivavto);
	   		level_menu=2323;
	   	}
	   	////���� � 23237  ����� ������ ������ //���������� �������� �������� �����
	   	if(level_menu==23237){
	   		minutstartpolivavto=setMinutsmethod(minutstartpolivavto);
	   		level_menu=2323;
	   	}
	   	//���� � 23234 ��� ��������� ����� ���������� �������
	   	if(level_menu==23234){
	   				hours_systemtime=sTime.Hours;  // ��������� � ����������  ������� �������� ����
	   		   		hours_systemtime=setHoursmethod(hours_systemtime); //������� � ������� ��������� ���������� ����� ������� ������ ������� ����
	   		   		minut_systemtime=sTime.Minutes; //����������� ������ � ����������
	   		   		MX_RTC_Init2(hours_systemtime,minut_systemtime);//�������������� ��������� �����
	   		   		level_menu=2323;
	   		   	}
	   	//���� � 23235 ��� ��������� ����� ���������� �������
	   	if(level_menu==23235){
	   						minut_systemtime=sTime.Minutes; // ��������� � ����������  ������� �������� �����
	   						minut_systemtime=setMinutsmethod(minut_systemtime); //������� � ������� ��������� ���������� ����� ������� ������ ������� ����
	   		   		   		hours_systemtime=sTime.Hours; //����������� ������ � ����������
	   		   		   		MX_RTC_Init2(hours_systemtime,minut_systemtime);//�������������� ��������� �����
	   		   		   		level_menu=2323;
	   		   		   	}
	   	//���� � 23233 �������� ���������� ����������
	   	if(level_menu==23233){
	   		soberiMenu23233(hoursstartpolivavto,minutstartpolivavto,interval_autoregMin,pereriv_intervalMinforauto);
	   		level_menu=2323;
	   	}
 //--------------------------------------------------------------------------------------------------------------
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  //RTC_TimeTypeDef sTime = {0};
 // RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */
//--------------------------------------------------------------------------------------
	//����� ��������� ������� ��� �������� ������� RTC
	//����� 1. ������� � ����������� ���������� RTC_TimeTypeDef sTime = {0};
	  	  	  	  	  	  	  	  	  	  	  //RTC_DateTypeDef DateToUpdate = {0};

	//        2.������������   -if (HAL_RTC_SetTime    �
	//        				 -if (HAL_RTC_SetDate      ��� ���� ����� ����� ����������� ����� �� ������������������

//------------------------------------------------------------------------------------
  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
    
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 15;
  sTime.Minutes = 55;
  sTime.Seconds = 0;

 // if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
 // {
  //  Error_Handler();
 // }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_MAY;
  DateToUpdate.Date = 11;
  DateToUpdate.Year = 20;

 // if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
 // {
 //   Error_Handler();
 // }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED1_Pin|valve_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : temperatura_Pin */
  GPIO_InitStruct.Pin = temperatura_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(temperatura_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : fourth_button_Pin */
  GPIO_InitStruct.Pin = fourth_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(fourth_button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : third_button_Pin second_button_Pin first_button_Pin */
  GPIO_InitStruct.Pin = third_button_Pin|second_button_Pin|first_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : valve_Pin */
  GPIO_InitStruct.Pin = valve_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(valve_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

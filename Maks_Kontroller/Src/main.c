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
#define time_poliv  5   //время полива в ручном режиме по таймеру в мин
#define ON 1
#define OFF 0
#define vkluchitna_min 5    //интервал времени по умолчанию для ывечернего полива
//время начала полива  18-00
//#define hoursstartpolivavto 20    //время старта полива
//#define minutstartpolivavto 00
#define hoursresetflag  23     //время суточное для сброса флага полиа
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
uint32_t adcResult = 0;   //переменная для считывания реультатов измерения
char str1[60];                //для ds18b20
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
//функция установки часов
static void MX_RTC_Init2(uint8_t hoursmi,uint8_t minmi)
{

  //RTC_TimeTypeDef sTime = {0};
 // RTC_DateTypeDef DateToUpdate = {0};

 	//После генерации проекта куб затирает функцию RTC
	//нужно 1. Вынести в голобальные переменные RTC_TimeTypeDef sTime = {0};
	  	  	  	  	  	  	  	  	  	  	  //RTC_DateTypeDef DateToUpdate = {0};

	//        2.Закомментить   -if (HAL_RTC_SetTime    и
	//        				 -if (HAL_RTC_SetDate      для того чтобы после перзагрузки время не инициализировалось

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
//структура для инициализации меню на дисплее
struct current_line
{
	char* line1;
	char* line2;
	char* line3;
	char* line4;
};

//---------------------------------------------------------
//функиця обновления дисплея
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
//функция опроса кнопок на панели операптора
//4 кнопки слева на право 1,2,3,4
//Если кнопка нажата то возвращает номер кнопки и 1
        //  пример кнопка 1-> возврат  11 короткое нажатие 12 длиное нажатие
//Если ничего не нажато то возврат 0
uint8_t press_Button(){
	uint8_t code;
	uint32_t time;
	uint32_t limit=100;
	uint32_t press_count=200000;
	time=0;
	code=0;

        //для первой кнопки
		if(HAL_GPIO_ReadPin(GPIOB,first_button_Pin)==GPIO_PIN_RESET){ //чтение первой кнопки
			HAL_Delay(25);
			if(HAL_GPIO_ReadPin(GPIOB,first_button_Pin)==GPIO_PIN_RESET){
									code=11;
			}
		}

		//для второй кнопки
		if(HAL_GPIO_ReadPin(GPIOB,second_button_Pin)==GPIO_PIN_RESET){ //чтение первой кнопки
					HAL_Delay(25);
					if(HAL_GPIO_ReadPin(GPIOB,second_button_Pin)==GPIO_PIN_RESET){
						code=21;
					}
				}

		//для третьей кнопки
		if(HAL_GPIO_ReadPin(GPIOB,third_button_Pin)==GPIO_PIN_RESET){ //чтение первой кнопки
					HAL_Delay(25);
					if(HAL_GPIO_ReadPin(GPIOB,third_button_Pin)==GPIO_PIN_RESET){
						code=31;
					}
				}
         //для четвертой кнопки
		if(HAL_GPIO_ReadPin(GPIOC,fourth_button_Pin)==GPIO_PIN_RESET){ //чтение первой кнопки
					HAL_Delay(25);
					if(HAL_GPIO_ReadPin(GPIOC,fourth_button_Pin)==GPIO_PIN_RESET){
						code=41;
					}
				}

	return code;
}
//----------------------------------------------------------------------------------
//Функция которая смотрит нужно ли изменить подуровень меню
uint16_t nextLevel(uint16_t currentlevel,uint8_t numer_button){
	uint16_t next;
		if(currentlevel==10){    //находмися в меню по умолчанию
			next=20;      //menu  идем в главное меню
		}else if(currentlevel==20){  //из главного меню можно перйти
								  if(numer_button==1){
									  next=211;     //Ruchnoi time
								  }else if(numer_button==2){
									  next=221;      //Ruchnoi
								  }else if(numer_button==3){
									  next=231;      //Ustanovki
								  }else if(numer_button==4){
									  next=10;      //menu
								  }
		}else if(currentlevel==211){   //Ручной по времени

									if(numer_button==4){
									     next=10;
								  }else{
									  next=currentlevel;
								  }

		}else if(currentlevel==221){  //Ручной в режиме реального времени
			  if(numer_button==4){
				  next=10;
			  }else{
				  next=currentlevel;
			  }
		}else if(currentlevel==231){  //Установки
								  if(numer_button==1){
									  next=2311;   //   Холодный день
								  }else if(numer_button==2){
									  next=2312;    //  Жакрий день
									}else if(numer_button==3){
										 next=2320;   // Установка интервалов мин
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
											next=2321;   //   Длина подачи
									}else if(numer_button==2){
											next=2322;    //  Длина перерыва
									}else if(numer_button==3){
										next=2323;    //  Настройки времени
									}else  if(numer_button==4){
										next=231;

									}
		}else if(currentlevel==2323){
									if(numer_button==1){
											next=23231;   //   Установка времени системы
									}else if(numer_button==2){
											next=23232;    //  Утановка времени старта полива
									}else if(numer_button==3){
											next=23233;    //  Просмотр параметров
									}else  if(numer_button==4){
											next=2320;    //возврат  в предидущее меню
									}
		}else if(currentlevel==23231){  //изменение системного времени
									if(numer_button==1){
										next=23234;   //   Установка часов
									}else if(numer_button==2){
										next=23235;    //  уставновка мин
									}else if(numer_button==3){
										next=2323;    //  возврат в предидущее меню
									}else  if(numer_button==4){
										next=2323;    //возврат  в предидущее меню
									}
		}else if(currentlevel==23232){   //меню изменения времени старта
									if(numer_button==1){
										next=23236;   //   Установка часов
									}else if(numer_button==2){
										next=23237;    //  уставновка мин
									}else if(numer_button==3){
										next=2323;    //  возврат в предидущее меню
									}else  if(numer_button==4){
										next=2323;    //возврат  в предидущее меню
									}
		}else {
			next=currentlevel;
		}

	return next;
}
//Карта распределение уровней меню
	     //      1.0 default
	     //        |
	     //      2.0 menu
	     //        |---2.1 Hand action time limit
	     //        |     |----2.1.1 Zapusk Timera
	     //        |---2.2 Hand action On/Off
	     //        |     |----2.2.1 Ruchnoe vkluchenie klapana
	     //        |---2.3 Settings
	     //        |     |----2.3.1 Nastroiki info
	     //        |     |      |-----2.3.1.1 Холодный день длина интервала полива
		//			|	|		|			|----Текущее значение мин
		//			|	|		|			|----Изменяемое занчение мин
		 //		   |	 |		|-----2.3.1.2 Жаркий день длина интервала полива
		 //		   |	 |
	     //        |     |----2.3.2 Интервалы полива//Сколько раз поливать
		//						|------2.3.2.1 Колво мин подачи воды
		//			|	|		|			|----Текущее значение
		//			|	|		|			|----Изменяемое
		//						|------2.3.2.2 Колво мин интервал перерыва
		//			|	|		|			|---Текущее значение
		//			|	|		|			|---Изменяемое значение
		//			|	|		|------2.3.2.3 Установка времени
		//			|	|		|			|------2.3.2.3.1 Установка часов
		//			|	|		|			|			|------2.3.2.3.4 Часы
		//			|	|		|			|			|			|----Текущее значение мин
		//			|	|		|			|			|			|----Изменяемое занчение мин
		//			|	|		|			|			|------2.3.2.3.5 Мин
		//			|	|		|			|						|----Текущее значение мин
		//			|	|		|			|						|----Изменяемое занчение мин
		//			|	|		|			|------2.3.2.3.2 Установка времени старта
		//			|	|		|			|			|------2.3.2.3.6 Часы
		//			|	|		|			|			|			|----Текущее значение мин
		//			|	|		|			|			|			|----Изменяемое занчение мин
		//			|	|		|			|			|------2.3.2.3.7 Мин
		//			|	|		|			|						|----Текущее значение мин
		//			|	|		|			|						|----Изменяемое занчение мин
		//			|	|		|			|-----2.3.2.3.3 Просмотр времени когда запланирован старт
		//			|	|		|			|------Отмена
		//						|------Otmena
	     //        |     |----2.3.3 Настройка часов
	     //        |
	     //        |---2.4 Cancel
//----------------------------------------------------------------------------------
//функция сброса флага суточного полива
uint8_t getNewflagforpoliv(uint8_t currentflag){
	uint8_t flag;
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //запрос времени
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
//функция автоматического полива
uint8_t autoRegim(uint16_t intervalmin,uint16_t pereriv_megdupolivami,uint8_t flagpolivcomplete,uint8_t hoursstartpolivavto,uint8_t minutstartpolivavto ){  //на вход даем на сколько мин включить
	 uint8_t status; // статус возврата результата работы функции 1-полив успешно завершен
	                                                           // 0-поливать рано
	                                                           // 3-полив прерван в ручную
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   // 4-полив перван на перекур
	 uint16_t intervalzadan;
	 uint8_t flagstart=0;  //флаг активации полива
	 uint8_t minstart;     //время старта
	 uint8_t secstart;
	 uint16_t allsecstart;   //абсолютное время старта в секундах
	 uint16_t timer_vision;  //обратный отсчет
	 uint8_t mincurrent;   //текущие показания времени
	 uint8_t seccurrent;
	 uint16_t allseccurrent;  //абсолютное время текущее в секунтах
	 char intervalchar[12];   //тестовая для вывода
	 char kolchar[12];
	 uint8_t kolvovklucheny;  //сколько раз включить в зависимости от общего времени и перерыва
	 float kolvovkluchfloat;       //промежуточный вариант колва включений не округленный
	 uint8_t periodOnflag;    //флаг когда период нужно включить  1- полив 0 перекур

	  //заходим функцию проверяем время если наступило 18-00 то включаем полив и крутимся здесь
	  //по окончании ставим флаг полива на сегодня

	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //запрос времени
	  if(sTime.Hours==hoursstartpolivavto){ //если наступило 18 часов
		  if(sTime.Minutes==minutstartpolivavto){  //если наступило 00 мин
			  if(flagpolivcomplete==0){   //если еще не поливали то приступаем к поливу
				  Poliv(ON);  //включаем полив

				  //intervalzadan=intervalmin*60;  //переводим заданный интервал из мин в секунды

				  flagstart=1; //стваим флаг что нужно поливать
				  periodOnflag=1;    //ставим флаг для первой половины полива
				  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //запрос времени
				  //сохраняем время начала полива те во столько то мин-сек полив стартует
				  minstart=sTime.Minutes;
				  secstart=sTime.Seconds;
				  allsecstart=secstart+minstart*60; //переводим в секунды
				  //allsecstart=allsecstart+intervalzadan;

				  //вычиляем сколько раз надо прерваться на прерыв или сколько раз полить
				  kolvovkluchfloat=intervalmin/pereriv_megdupolivami;
				  if(kolvovkluchfloat<1){
					  kolvovklucheny=1;  //поливать один раз
				  }else if(kolvovkluchfloat==1){
					  kolvovklucheny=1;
				  }else if(kolvovkluchfloat>1){
					  kolvovklucheny=ceil(kolvovkluchfloat);

				  }
				  snprintf(kolchar,11,"%d_raz",kolvovklucheny);

				  intervalzadan=pereriv_megdupolivami;//вычисляем длину короткого интервала В мин
				  intervalzadan=intervalzadan*60; //переводим в секунды


				  while(flagstart==1){//заходим в петлю если есть разрешение на полив





					  while(kolvovklucheny>0){

						  while(periodOnflag==1){    //половинка когда поливаем
							  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //запрос времени
							  mincurrent=sTime.Minutes;
							  seccurrent=sTime.Seconds;
							  allseccurrent=seccurrent+mincurrent*60; //переводим из мин и сек текущее время в секунды
							  timer_vision=allsecstart+intervalzadan-allseccurrent;//обратный отсчет для дисплея
							  snprintf(intervalchar,11,"%d_%d",timer_vision,intervalzadan);
							  snprintf(kolchar,11,"%d_raz",kolvovklucheny);
							  if((allsecstart+intervalzadan)<=allseccurrent){//когда заданный интервал исчерпан
								  //то
								  periodOnflag=0; //говорим что все полилось втекущем периоде все окей
							  }else{
								  //отображаем на дисплей что просиходит
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
							  if(HAL_GPIO_ReadPin(GPIOC,fourth_button_Pin)==GPIO_PIN_RESET){ //останов по кнопке 4
								  periodOnflag=0;   //скидываем все флаги так как все по требованию
								  kolvovklucheny=0;
								  flagstart=3;
								  //flagstart=3;
							  }
							  HAL_Delay(1000);
						  }
						  Poliv(OFF); //выключаем полив
						  allsecstart=allsecstart+intervalzadan; //переносим время старта для следующего промежутка
						  kolvovklucheny--;
						  //ssd1306_Fill(Black);
						  //ssd1306_UpdateScreen();


						  while((periodOnflag==0)&&(kolvovklucheny!=0)){ //половинка когда ждем поллив выключен но будем продложать
							  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //запрос времени
							  mincurrent=sTime.Minutes;
							  seccurrent=sTime.Seconds;
							  allseccurrent=seccurrent+mincurrent*60; //переводим из мин и сек текущее время в секунды
							  if((allsecstart+intervalzadan)<=allseccurrent){
								  periodOnflag=1;  //перебрасываем флаг что нужно снова включать
							  }
							  timer_vision=allsecstart+intervalzadan-allseccurrent; //вычисляем сколько осталось
							  snprintf(kolchar,11,"%d_ostalos",timer_vision);

							  //отображаем на дисплей что просиходит
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


							  	if(HAL_GPIO_ReadPin(GPIOC,fourth_button_Pin)==GPIO_PIN_RESET){ //останов по кнопке 4
							  		  periodOnflag=0;    //скидываем флаги так как все по требованию
							  		kolvovklucheny=0;
							  		  flagstart=3;
  								  }
								  HAL_Delay(1000);
						  }
						  //когда отработан миницикл равный интервалу полива + интервалу
						  //ожидания пока вода впитается то вновь сдвигаем время старта
						  if((flagstart!=3)&&(kolvovklucheny!=0)){
							  allsecstart=allsecstart+intervalzadan; //переносим время старта для следующего промежутка
							  Poliv(ON);  //включаем полив
						  }else if(kolvovklucheny==0){   //  если количество включений пришло к нулю
							  flagstart=2;  //сбрасываем флаг и выходим из функции автополива
						  }


					  }



				  }
				  //от while-------------------------
				  Poliv(OFF); //выключаем полив
				  //пишем на дисплей
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
	  }//от if запроса времени - старта полива
	if(flagstart==0){//проверяем флаг старта что произошло в функции
		status=0;   //значит поливать рано
	}else if(flagstart==2){
		status=1;   //все хорошо полив прошел успешно
	}else if(flagstart==3){
		 status=3;   //полив прерван в ручную
	}
	return status;
}
//--------------------------------------------------------------------------------
//Функция ручного прямого включения клапана
void action_Valve(){
	uint8_t interval;
	uint8_t lastsec;
	uint8_t secun;
	uint8_t startt;
	char intervalchar[9];


	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //запрос времени
	lastsec=sTime.Seconds;
	startt=lastsec;
	Poliv(ON);//включаем полив
	while(HAL_GPIO_ReadPin(GPIOB,second_button_Pin)==GPIO_PIN_RESET){//пока нажат кнопка 2 активен режим ручного включения
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //запрос времени
		secun=sTime.Seconds;
		if(lastsec!=secun){
			interval=secun-startt;
		}
		snprintf(intervalchar,8,"time_%d",interval);
		//заполняем дисплей выводим счетчик секунд
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
	Poliv(OFF);//выключаем полив при выходе
}
//---------------------------------------------------------------------------------
//Функция включения клапана полива
void Poliv(uint8_t status){  //на вход подается  статус что нужно вкл или выключить клапан
	if(status==1){
		HAL_GPIO_WritePin(GPIOC,valve_Pin,GPIO_PIN_SET);
	}
	if(status==0){
		HAL_GPIO_WritePin(GPIOC,valve_Pin,GPIO_PIN_RESET);
	}
}
//---------------------------------------------------------------------------------
//функция отработки клапана в ручном режиме на определенный промежукток времени
void action_Valvetotime(uint8_t timeact){
	uint16_t secund_interval;
	uint16_t secund;
	uint16_t lastsecund;
	uint16_t countsec;   //счетчик секунд
	uint8_t startsec;
	uint8_t startmin;
	char intreval[6];
	char ostatok[14];
	char countchar[6];

				 //0-60
	secund_interval=timeact*60;  //перевести минуты в секунды для интервала полива
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //спрашиваем сколько времени
	startsec=sTime.Seconds;   //записываем время старта сек
	startmin=sTime.Minutes;   //мин
	secund=secund_interval;
	lastsecund=startsec;
	Poliv(ON);          //включаем полив
	while(secund>1){    //запускаем  обратный таймер
       // HAL_Delay(1000);
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); //запрос времени
		countsec=(sTime.Minutes*60+sTime.Seconds)-(startmin*60+startsec);//вычисляем сколько времени прошло
		if(lastsecund!=countsec){
			lastsecund=countsec;
			secund=secund_interval-countsec; //обратный таймер в секундах
		}

	    //заполняем дисплей
		    //подгтовка цифр
		    snprintf(intreval,5,"%d",secund_interval); //для записи стартового интервала
		    snprintf(ostatok,13,"Ostalos_%d",secund);// сколько осталось
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
	Poliv(OFF);          //выключаем полив после выхода из петли расчета времени
	    //пишем что клапан выключен
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
//функция для определения времени суток
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
//Функция изменеия часов
uint8_t setHoursmethod(uint8_t hour){
	//заполняем дисплей
	 //считываем кнопки
	 //изменяем знчения мин
	  //Обновляем дисплей
	  //при выходе возвращаем значение новое значение интервала
		 uint8_t new_hours;
	   	 char tekushee_znachenie[12];
		 char new_znachenie[12];


		 snprintf(tekushee_znachenie,12,"%d_Old_min",hour); //перобразовние в text
		 	 	 	 snprintf(new_znachenie,12,"%d_New_hour",hour); //на входе одинаковые занчения часа
		 	 	 	new_hours=hour;  //на входе значения часа одинаковые
		 	 	 	 //заполняем дисплей
		 	 	 	ssd1306_SetCursor(0,0);
		 	 	 	ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,15);
		 	 	 	ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,30);
		 	 	 	ssd1306_WriteString("1&2 Change",Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,45);
		 	 	 	ssd1306_WriteString("Exit&Save",Font_11x18,White);
		 	 	 	ssd1306_UpdateScreen();
		 while(press_Button()!=41){  //пока не нажата кнопка 4
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
		 	 	 	snprintf(new_znachenie,12,"%d_New_hour",new_hours);//обновляем значение
		 	 	 	 // пересобирваем вывод на дисплей
		 	 	 	ssd1306_SetCursor(0,0);
		 	 	 	ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,15);
		 	 	 	ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,30);
		 	 	 	ssd1306_WriteString("1&2 Change",Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,45);
		 	 	 	ssd1306_WriteString("Exit&Save",Font_11x18,White);
		 	 	 	ssd1306_UpdateScreen();

		 }//от while
		           //навыходе пишем что занчение изменено
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
//функция изменения минут
uint8_t setMinutsmethod(uint8_t minuts){
	//заполняем дисплей
	 //считываем кнопки
	 //изменяем знчения мин
	  //Обновляем дисплей
	  //при выходе возвращаем значение новое значение интервала
		 uint8_t new_minuts;
	   	 char tekushee_znachenie[12];
		 char new_znachenie[12];


		 snprintf(tekushee_znachenie,12,"%d_Old_min",minuts); //перобразовние в text
		 	 	 	 snprintf(new_znachenie,12,"%d_New_min",minuts); //на входе одинаковые занчения часа
		 	 	 	new_minuts=minuts;  //на входе значения часа одинаковые
		 	 	 	 //заполняем дисплей
		 	 	 	ssd1306_SetCursor(0,0);
		 	 	 	ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,15);
		 	 	 	ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,30);
		 	 	 	ssd1306_WriteString("1&2 Change",Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,45);
		 	 	 	ssd1306_WriteString("Exit&Save",Font_11x18,White);
		 	 	 	ssd1306_UpdateScreen();
		 while(press_Button()!=41){  //пока не нажата кнопка 4
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
		 	 	 	snprintf(new_znachenie,12,"%d_New_min",new_minuts);//обновляем значение
		 	 	 	 // пересобирваем вывод на дисплей
		 	 	 	ssd1306_SetCursor(0,0);
		 	 	 	ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,15);
		 	 	 	ssd1306_WriteString(new_znachenie,Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,30);
		 	 	 	ssd1306_WriteString("1&2 Change",Font_11x18,White);
		 	 	 	ssd1306_SetCursor(0,45);
		 	 	 	ssd1306_WriteString("Exit&Save",Font_11x18,White);
		 	 	 	ssd1306_UpdateScreen();

		 }//от while
		           //навыходе пишем что занчение изменено
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
//функция подменю установки интервала полива для холодного дня
uint16_t setHolod2311(uint16_t current_min){//
	  //заполняем дисплей
	  //считываем кнопки
	  //изменяем знчения мин
	  //Обновляем дисплей
	  //при выходе возвращаем значение новое значение интервала
	 uint16_t new_min_interval;
	 char tekushee_znachenie[12];
	 char new_znachenie[12];

	 	 	 snprintf(tekushee_znachenie,12,"%d_Old_min",current_min); //перобразовние в text
	 	 	 snprintf(new_znachenie,12,"%d_New_min",current_min); //на входе одинаковые занчения мин
	 	 	new_min_interval=current_min;  //на входе значения мин одинаковые
	 	 	 //заполняем дисплей
	 	 	 	 	ssd1306_SetCursor(0,0);
	 				ssd1306_WriteString(tekushee_znachenie,Font_11x18,White);
	 				ssd1306_SetCursor(0,15);
	 				ssd1306_WriteString(new_znachenie,Font_11x18,White);
	 				ssd1306_SetCursor(0,30);
	 				ssd1306_WriteString("1&2 Change",Font_11x18,White);
	 				ssd1306_SetCursor(0,45);
	 				ssd1306_WriteString("Exit&Save",Font_11x18,White);
	 				ssd1306_UpdateScreen();
	 		while(press_Button()!=41){  //пока не нажата кнопка 4
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
	 				 snprintf(new_znachenie,12,"%d_New_min",new_min_interval);//обновляем значение
	 			     // пересобирваем вывод на дисплей
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

	 		//навыходе пишем что занчение изменено
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
//Отображения технических параметров автоматического режима полива
//Показывает когда назначен полив
//Сколько поливать сколько перерыв
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
//Функция сборки меню для утсанвоки мин плива без температурного режима
void soberiMenu2320(struct current_line *setline){
					setline->line1="Podacha ";
					setline->line2="Pereriv ";
					setline->line3="Time settings";
					setline->line4="Otmena";
}
//-------------------------------------------------------------------------------------
//функция сборки меню 231
void soberiMenu231(struct current_line *setline){
	            setline->line1="Holodnyi ";
				setline->line2="Garkiy ";
				//setline->line3="Skolko raz";
				setline->line3="Skolko min";
				setline->line4="Otmena";
}

//-------------------------------------------------------------------------------------
//функция сборки менб 221
void soberiMenu221(struct current_line *setline){
			setline->line1="Ruchnoy ";
			setline->line2="regim ";
			setline->line3="Poliv_ON";
			setline->line4="Otmena";
}
//--------------------------------------------------------------------------------------
//функция сборки меню 211
void soberiMenu211(struct current_line *setline){
		setline->line1="Min_";
		setline->line2="Ostalos ";
		setline->line3="Poliv_ON";
		setline->line4="Otmena";
}
//------------------------------------------------------------------------------------
//функция сборки основного меню 20
void soberiMenu20(struct current_line *setline){

	 setline->line1="Rucnoy time";
	 setline->line2="Rucnoy ";
	 setline->line3="Ustanovki";
	 setline->line4="Otmena";
}
 //---------------------------------------------------------
//сервисная функция для моргания светодиодом
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
	//функция запуска светодиода при старте
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
		ssd1306_Fill(Black);                             //пишем
	    ssd1306_SetCursor(2, 0);
	    ssd1306_WriteString("Yahuuu", Font_16x26, White);
	    ssd1306_UpdateScreen();
	    HAL_Delay(t1);                                 //стоит надпись
	    ssd1306_Fill(Black);                           //очищаем
	    ssd1306_UpdateScreen();
	    HAL_Delay(t2);                                //Черный

	    ssd1306_Fill(Black);                                     //пишем
	    ssd1306_SetCursor(2, 0);
	    ssd1306_WriteString("Yahuuu", Font_16x26, White);
	    ssd1306_UpdateScreen();
	    HAL_Delay(t1);                                                 //стоит надпись
	    ssd1306_Fill(Black);                                             //очищаем
	    ssd1306_UpdateScreen();                                         //черный
	    HAL_Delay(t2);

	    ssd1306_Fill(Black);                                           //пишем в следющей строке
	    ssd1306_SetCursor(2, 26);
	    ssd1306_WriteString("Yahuuu", Font_16x26, White);
	    ssd1306_UpdateScreen();
	    HAL_Delay(t1);
	    ssd1306_Fill(Black);                                             //очищаем
	    ssd1306_UpdateScreen();                                         //черный
	    HAL_Delay(t2);
	    ssd1306_Fill(Black);                                           //пишем в следющей строке
	    ssd1306_SetCursor(2, 26);
	    ssd1306_WriteString("Yahuuu", Font_16x26, White);
	    ssd1306_UpdateScreen();
	    HAL_Delay(t1);
	    ssd1306_Fill(Black);                                             //очищаем
	   	ssd1306_UpdateScreen();                                         //черный
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
    uint8_t error_code=0;   //таблица кодов ошибок  0- нет ошибок 1-есть ошибка какаято
    float Upitanija=3.195;       //для расчета напряжения ацп
    uint16_t razradnostAcp=4096;    //разрядность ацп
    float udatchika_temp=0;   //напряжение на входе с датчика температуры  tmp36gz
    uint16_t Udispint=0;
    float ktmp36gz=34.78; //коэффициент датчика температуры
    float temp=0;    //темепратура с датчика
    uint8_t status;      //для ds18b20
    uint8_t dt[8];      //для ds18b20
    uint16_t raw_temper;   //для ds18b20
    float temper;        //для ds18b20
    char c;
    uint8_t state_button;   //статус нажатия кнопок  1- нажата кн1 2 -нажата кн2 3 нажата кн3 4 нажата кн4 0- не нажата кн
    uint16_t level_menu;     //уровень меню отображаемый на LCD 10(1.0) меню по умолчанию
                                            //  20(2.0) меню
    										//  21(2.1)
                                            //  22(2.2)

     char *metka;
     uint8_t polivcomlete_flag=0;  //флаг что сегодня поливали (0,1) сбрасывается в 0 в 11-59 ставится в функции autoRegim
     uint8_t sostoyaniepoliva;    //результат работы функции автополива
     uint16_t holodny_periodmin=5;  //время полива в холодное время
     uint16_t garky_periodmin=7;    //время полива в жаркий период
     uint16_t interval_autoregMin=10;   //  общее количество мин которое должно быть поступление воды
     uint16_t pereriv_intervalMinforauto=5;  //время короткого промежутка для остановки полива чтобы вода впиталась и не заливал
	 uint8_t hoursstartpolivavto=18;    //время старта полива  часов
	 uint8_t minutstartpolivavto=10;    //мин
	 uint8_t hours_systemtime;    //переменная для утсановки системного времени
	 uint8_t minut_systemtime;    //переменная для установки системного времени
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
  //Создание первого меню дисплея
  struct current_line mainmenu;

  mainmenu.line1="> set1";
  mainmenu.line2="  set2";
  mainmenu.line3="  set3";
  mainmenu.line4="  set4";

  update_LCD(&mainmenu);

  port_init();  //инициализация ноги для DS18B20 Порт А вход 3
  level_menu=10; //уровень отображения меню -по умолчанию
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	     //в жаркий день распылителем 30 мин
	     //в холодный мин 15 - 10 мин
	     //-------------------------------------
         //распределение уровней меню
	     //      1.0 default
	     //        |
	     //      2.0 menu
	     //        |---2.1 Hand action time limit
 	     //        |     |----2.1.1 Zapusk Timera
	     //        |---2.2 Hand action On/Off
	     //        |     |----2.2.1 Ruchnoe vkluchenie klapana
	     //        |---2.3 Settings
	     //        |     |----2.3.1 Nastroiki info
	     //        |     |      |-----2.3.1.1 Холодный день длина интервала полива
		 //		   |	 |		|-----2.3.1.2 Жаркий день длина интервала полива
		 //		   |	 |
	     //        |     |----2.3.2 Сколько раз поливать
	     //        |     |----2.3.3 Настройка часов
	     //        |
	     //        |---2.4 Cancel
	     //
	    //----------------------------------------------
	  // в режиме по умолчанию   1.0 уровень меню
	                        // 1.Время
	                        // 2.Время суток
	                        // 3.Температура
	                        // 4.Статус  или ошибка
	                        //
	  //при нажатии на кнопку переходит в режим меню     2.0 уровень меню
	                        // 1.ручной запуск на время t
	                        // 2.ручной запуск по нажатию
	                        // 3.Установки
	                        // 4.Возврат в меню по умолчанию
	  //----------------------------------------------------------------------------------------------------------------------
	  //Ручной запуск на время t   2.1   уровень меню
	                       //при входе сразу активация клапана
	                       //LCD 1.Отсчет времени обратный
	                          // 2.Изначальный период времени t фикс
	                          // 3.Статус выхода эл.маг клапана
	                          // 4.Отмена
	                       //По окончании возврат в меню по молчанию сброс флага не производится
	  //Ручной запуск по нажатию  2.2  уровень меню
	                      //при входе сразу активируем клапан
	                      //LCD  1. Отсчет времени прямой
	                        //   2. РЕЖИМ "Ручной"
	                        //   3. Статус выхода эл.маг
	                        //   4. Отсутствует
	                      //При отпускании кнопки возврат в меню по умолчанию// в режим меню?
	  //Установки                2.3  уровень меню
	                     //LCD
	                     //1.Установка длины периода полива         2.3.1  уровень меню
	                                                       //1.Период холодный     2.3.1.1
	                                                                          //1.Текущий до редактировани
	                                                                          //2.Редактируемый
	                                                                          //3.кн1 ++    кн2--   кн3 сохранить
	                                                                          //4.Отмена
	                                                                          //кнопка 3 сохраняет также сохранение при разнице занчений текущего и предидущего
	                                                       //2.Период жаркий    2.3.1.2 уровень меню
	                                                                               //1.Текущий до редактировани
	  	                                                                          //2.Редактируемый
	  	                                                                          //3.кн1 ++    кн2--   кн3 сохранить
	  	                                                                          //4.Отмена
	  	                                                                          //кнопка 3 сохраняет также сохранение при разнице занчений текущего и предидущего
	                                                       //3.                 2.3.1.3 уровень меню
	                                                       //4.Отмена           2.3.1.4 уровень меню
	                     //2.Утсановка кол-ва период        2.3.2
	                                                //1.Текущий до редактировани
	  	                                            //2.Редактируемый
	  	                                            //3.кн1 ++    кн2--   кн3 сохранить
	  	                                            //4.Отмена
	  	                                            //кнопка 3 сохраняет также сохранение при разнице занчений текущего и предидущего
	                     //3.Установка времени              2.3.3
	                                          //1.Время  Часы:Мин
	                                          //2.Время  Часы:Мин  Редактируемый
	  	                                      //3.кн1 ++    кн2--   кн3 переключает установку Часы -Мин
	  	                                      //4.Отмена или Подвердить
	  	                                   //Сохранение при выходе по кн4
	                     //4.Отмена                        2.3.4
	                  //По отмене или через таймер возврат в меню по умолчанию

	  //описание меню
	       //structura со свойствами
	       //уровни и под уровни меню

	  //фиксация нажатия клавиш
//--------------------------------------------------------------------------------
	  //Начало программы
//Опрос кнопок
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


     //Если была нажата кнопка то вычисляем под уровень меню и требуется ли действие
	  if(state_button!=0){
		  //вызываем функцию которая скажет нужно ли менять под уровень
		  level_menu=nextLevel(level_menu,state_button);
		  state_button=0;
	  }





	  if(level_menu==10){  //отобразить меню по default
//Меню по умолчанию 1.0
//1.порядок собрать структуру mainmenu
//2.Обновить экран
	  //для структуры необходимо: время часы-мин-сек   1я строчка
	                        //    время суток утро-день-вечер-ночь    2я строка
	                        //    показания датчика температуры       3я сторока
	                        //    статус выхода на клапан   ON-OFF   или код ошибки 4я строка

	  //получение значения времени для конфигурирования первой строчки
	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); // RTC_FORMAT_BIN , RTC_FORMAT_BCD
	  snprintf(trans_str, 63, " %d:%d:%d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
	  mainmenu.line1=trans_str;
	  //update_LCD(mainmenu);
	  //получение второй строки день ночь
	   //время  суток  утро с 6-00 до 11-00
	                  //день с 11-00 до 17-00
	                  //вечер с 17-00 до 23-00
	                  //ночь  с 23-00 до 6-00
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

	  //получение показаний датчика температуры

	  mainmenu.line3="T'C_23.8C";
/*	  код тестовый удалить
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
	  HAL_ADC_Start(&hadc1);      //запускаем измерения аналогового входа
	  HAL_ADC_PollForConversion(&hadc1, 100);    //дожидаемся окончания имерения
	  adcResult = HAL_ADC_GetValue(&hadc1);     //забираем реультат
	  HAL_ADC_Stop(&hadc1);                   //останавливаем измерения
	  udatchika_temp=adcResult*Upitanija/razradnostAcp;
	  Udispint=(int)(udatchika_temp*1000-500);
	  temp=Udispint/ktmp36gz;
	  Udispint=temp*100;
	  snprintf(data_str,63,"%d\n",Udispint);
	  // snprintf(data_str,63,"%d\n",adcResult);
	  mainmenu.line3=data_str;
*/

	  //получение статуса полива
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
	  //Организация полива в автомате
	  //текущий режим при достижении 18-00 включить на 8 мин

	  sostoyaniepoliva=autoRegim(interval_autoregMin,pereriv_intervalMinforauto,polivcomlete_flag,hoursstartpolivavto,minutstartpolivavto); //заходим в функцию даем сколько мин поливать и флаг поливали сегодня или нет
	  if(sostoyaniepoliva==1){  //
		  polivcomlete_flag=1;      //ставим флаг что полив произведен поливать больше не надо
		   }else if(sostoyaniepoliva==3){
		  polivcomlete_flag=1;      //ставим флаг что полив окончен вручную больше не поливать
	  }
	  //функция проверки времени 11-59 для сброса  флага полива на завтра
	  polivcomlete_flag=getNewflagforpoliv(polivcomlete_flag);
	  //HAL_Delay(500);
//---------------------------------------------------------------------------------------------------------------
	  //старая функция времени
   //blinktext();
/*
            HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); // RTC_FORMAT_BIN , RTC_FORMAT_BCD
          //  snprintf(trans_str, 63, "Time %d:%d:%d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
         //   HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
            snprintf(trans_str, 63, " %d:%d:%d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
            		ssd1306_Fill(Black);                             //пишем
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
	  }//от if level_menu==10 дефолтное меню
//--------------------------------------------------------
	  //вход в меню 2.0
	  // 1.ручной запуск на время t
	  // 2.ручной запуск по нажатию
	  // 3.Установки
	  // 4.Возврат в меню по умолчанию
	  if(level_menu==20){ //вход в меню
		  metka=&mainmenu;
		  soberiMenu20(metka);
		  update_LCD(metka);
	  }



	  //вход в  2.1.1 Ручной запуск
	  if(level_menu==211){
		  soberiMenu211(metka);
		  update_LCD(metka);
		  action_Valvetotime(time_poliv);  //активация клапан на на указанноое время аргумент в мин
		  level_menu=10;
	  }

	  //вход в 2.2.1
	  if(level_menu==221){
		  	  	  soberiMenu221(metka);
		  		  update_LCD(metka);
		  		  action_Valve();   //
		  		  level_menu=20;
	  	  }

	  //вход в 2.3.1
	   if(level_menu==231){
		   	   soberiMenu231(metka);
		   	   update_LCD(metka);
	  	  	  }
     //вход в 2.3.1.1     //Установка мин для полива в Холодный день
	   if(level_menu==2311){
		   holodny_periodmin=setHolod2311(holodny_periodmin);   //заходим в функцию устанвоки мин холодного периода полива
		   level_menu=231;  // возврращаемся в меню утсанвоков
	   }
	   //вход в 2.3.1.2  //установка мин для полива в Жаркий день
	   if(level_menu==2312){
		   garky_periodmin=setHolod2311(garky_periodmin);   //заходим в функцию устанвоки мин холодного периода полива
		   level_menu=231;  // возврращаемся в меню утсанвоков
	   }
	   //вход в 2.3.2.0  //Установка сколько мин поливать в атоматическом режиме без температуры
	   if(level_menu==2320){
	   		   	   soberiMenu2320(metka);
	   		   	   update_LCD(metka);
	   	  	  	  }
	   //Вход в 2.3.2.1.   //установка мин для полива в автоматическом режиме без температуры
	   if(level_menu==2321){
		   interval_autoregMin=setHolod2311(interval_autoregMin);   //заходим в функцию устанвоки мин холодного периода полива
	   		   level_menu=231;  // возврращаемся в меню утсанвоков
	   	   }
	   //Вход в 2.3.2.1.   //установка мин интервала затишья в автоматическом режиме без температуры
	   	   if(level_menu==2322){
	   		pereriv_intervalMinforauto=setHolod2311(pereriv_intervalMinforauto);   //заходим в функцию устанвоки мин холодного периода полива
	   	   		   level_menu=231;  // возврращаемся в меню утсанвоков
	   	   	   }
	   //Вход в 2.3.2.3	 //Установка часов установка времени старта общее меню
	   	if(level_menu==2323){
	   				soberiMenu2323(metka);
	   		  	    update_LCD(metka);
	   	}
	   	//Вход в 2.3.2.3.1.   //Установка времени системы
	   	if(level_menu==23231){
	   			soberiMenu23231(metka);
	   			update_LCD(metka);
	   	}
		//Вход в 2.3.2.3.2.   //Установка времени старта полива
	   	if(level_menu==23232){
	   		   			soberiMenu23232(metka);
	   		   			update_LCD(metka);
	   		   	}
        //Вход в 23236  время старта полива //измениение текущего часа
	   	if(level_menu==23236){
	   		hoursstartpolivavto=setHoursmethod(hoursstartpolivavto);
	   		level_menu=2323;
	   	}
	   	////Вход в 23237  время старта полива //измениение текущего значения минут
	   	if(level_menu==23237){
	   		minutstartpolivavto=setMinutsmethod(minutstartpolivavto);
	   		level_menu=2323;
	   	}
	   	//Вход в 23234 для установки часов системного времени
	   	if(level_menu==23234){
	   				hours_systemtime=sTime.Hours;  // сохраняем в переменную  текущее значение часа
	   		   		hours_systemtime=setHoursmethod(hours_systemtime); //заходим в функцию изменения количества часов ручками ставим сколько надо
	   		   		minut_systemtime=sTime.Minutes; //обнолявляем минуты в переменной
	   		   		MX_RTC_Init2(hours_systemtime,minut_systemtime);//инициализируем системное время
	   		   		level_menu=2323;
	   		   	}
	   	//Вход в 23235 для установки минут системного времени
	   	if(level_menu==23235){
	   						minut_systemtime=sTime.Minutes; // сохраняем в переменную  текущее значение минут
	   						minut_systemtime=setMinutsmethod(minut_systemtime); //заходим в функцию изменения количества минут ручками ставим сколько надо
	   		   		   		hours_systemtime=sTime.Hours; //обнолявляем минуты в переменной
	   		   		   		MX_RTC_Init2(hours_systemtime,minut_systemtime);//инициализируем системное время
	   		   		   		level_menu=2323;
	   		   		   	}
	   	//Вход в 23233 Просмотр параметров автополива
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
	//После генерации проекта куб затирает функцию RTC
	//нужно 1. Вынести в голобальные переменные RTC_TimeTypeDef sTime = {0};
	  	  	  	  	  	  	  	  	  	  	  //RTC_DateTypeDef DateToUpdate = {0};

	//        2.Закомментить   -if (HAL_RTC_SetTime    и
	//        				 -if (HAL_RTC_SetDate      для того чтобы после перзагрузки время не инициализировалось

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

#ifndef UART_COMMANDS_CONSTANTS_H
#define UART_COMMANDS_CONSTANTS_H

#include <inttypes.h>
#include "string.h"
// using namespace std;

// Массив начала команды 
// 0x10 - начало передачи 0x01 - адрес устройства на шине
static const unsigned char StartCMD[] = {0x10, 0x01}; //start of packet
static const unsigned char endOfCMD[] = {0x10, 0xFE, 0x00}; //tail of packet for clock station


// КОМАНДА ЧТЕНИЯ ОСНОВНЫХ ПАРАМЕТРОВ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 01 55 10 FE)
// ADR – адрес ЧС; CMD – 0x01
// 1. Один байт напряжения во всех каналах. Число в десятых долях вольта (255 = 25,5В).
// 2. Четыре байта значения тока в каналах на момент последнего срабатывания. Число для
// каждого канала в сотых долях ампера (125 = 1,25А).
// 3. Один байт состояния каналов в данный момент. Каждому каналу соответствует два бита
// в байте. Если оба бита сброшены значит канал выключен. Если в паре более старший бит
// установлен значит импульс положительный. Если в паре более младший бит установлен
// значит импульс отрицательный. В байте более старшая пара бит соответствует первому
// каналу, более младшая пара соответствует четвёртому каналу.
// 4. Один байт, отражающий перегрузку или КЗ в канале. Содержит число то 0 до 4. 0 – всё
// штатно. От 1 до 4 номер канала при перегрузке. При КЗ в байте с номером канала
// устанавливается старший бит.
// 5. Четыре байта от приёмника ГЛОНАСС. Первый байт – часы, второй байт – минуты, третий
// байт – секунды, поступающие от ГЛОНАСС приёмника и четвёртый байт – это видимое
// число спутников. Если число спутников равно нулю значит синхронизация ЧС не
// осуществляется.
// 6. Восемь байт. Положение стрелок вторичных часов. По два байта на канал. Первая пара –
// первый канал, последняя пара – четвёртый канал. В паре первый байт – часы (от 0 до
// 11), второй байт – минуты (от 0 до 59).
// 7. Шесть байт текущего времени и даты первичных часов ЧС. Байты идут по порядку часы,
// минуты, секунды, число, месяц, год (последние две цифры). Все значения в BCD формате
// т.е. 12 записывается как 0х12.
// 8. Один байт отражающий состояние автоматического перехода на летнее/зимнее время.
// Если байт равен нулю, то автоматический переход отключен. Если байт равен 1, то был
// осуществлён переход на летнее время. Если байт равен 2, то был осуществлён переход
// на зимнее время.
static const unsigned char ReadParametrsCMD[] = {0x01};

// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR

// CS 0x10 0xFE.


// КОМАНДА ЧТЕНИЯ ДЛИТЕЛЬНОСТИ ИМПУЛЬСА В КАНАЛАХ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 02 56 10 FE)
// ADR – адрес ЧС; CMD – 0x02
// 1. Один байт длительности импульса. Число в десятых долях секунды. Предельные
// значения от 2 до 240, что соответствует длительности импульса от 0,2 до 24,0 секунды.
static const unsigned char ReadImpulseCMD[] = {0x02};

// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR

// CS 0x10 0xFE.


// КОМАНДА ЧТЕНИЯ ВКЛЮЧЕННЫХ КАНАЛОВ ДЛЯ ВЫДАЧИ ИМПУЛЬСА ВТОРИЧНЫХ ЧАСОВ И
// СИНХРОНИЗАЦИИ ЭЛЕКТРОННЫХ ЧАСОВ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 03 57 10 FE)
// ADR – адрес ЧС; CMD – 0x03
// 1. Один байт с флагами. Младшие четыре бита отражают какой канал включен для выдачи
// импульсов вторичных часов (самый младший бит – канал 1, самый старший – канал 4).
// Старшие четыре бита отражают какой канал включен для синхронизации электронных
// часов. При нулевом значении канал выключен.
static const unsigned char ReadChannelStateCMD[] = {0x03};

// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR

// CS 0x10 0xFE.


// КОМАНДА ЧТЕНИЯ СУТОЧНОЙ КОРРЕКЦИИ И ВРЕМЕНИ КОРРЕКЦИИ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 04 50 10 FE)
// ADR – адрес ЧС; CMD – 0x04
// Ответ: 0x10 ADR
// 1. Один байт суточной коррекции. Значение в секундах. Максимальное значение 99
// секунд. Если коррекция отрицательная, то старший бит установлен.
// 2. Один байт значения часа суток в которое происходит коррекция. Коррекция происходит
// один раз в сутки в ХХ:30. ХХ имеет двоично-десятичное значение от 0х00 до 0х23.
static const unsigned char ReadDayCorrectionCMD[] = {0x04};

// CS = 0x55^ADR^CMD

// CS 0x10 0xFE.


// КОМАНДА ЧТЕНИЯ ЧАСОВОГО ПОЯСА
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 05 51 10 FE)
// ADR – адрес ЧС; CMD – 0x05
// 1. Один байт часового пояса. Значение в часах от 0 до 12. Если это значение
// отрицательное, то старший бит установлен.
static const unsigned char ReadTimeZoneCMD[] = {0x05};

// CS = 0x55^ADR^CMD.
// Ответ: 0x10 ADR

// CS 0x10 0xFE.


// КОМАНДА ЗАПИСИ ВРЕМЕНИ В КАНАЛАХ
// Запись: 0x10 ADR CMD – 0х06
// CMD Channel Hour Minute
// 1. Один байт номера канала. Значения от 1 до 4.
// 2. Два байта времени в канале. Первый байт – часы (от 0 до 11), второй байт – минуты (от 0
// до 59).
static const unsigned char WriteTimeInChannelCMD[] = {0x06,0x01,0x00,0x00};


// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.


// КОМАНДА ЗАПИСИ ДЛИТЕЛЬНОСТИ ИМПУЛЬСА В КАНАЛАХ
// Запись: 0x10 ADR CMD – 0х07
// 1. Один байт длительности импульса. Число в десятых долях секунды. Предельные
// значения от 2 до 240, что соответствует длительности импульса от 0,2 до 24,0 секунды.
static const unsigned char WriteImpulseDurationInChannelCMD[] = {0x07,0x30};


// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.


// КОМАНДА ЗАПИСИ ВКЛЮЧЕННЫХ КАНАЛОВ ДЛЯ ВЫДАЧИ ИМПУЛЬСА ВТОРИЧНЫХ ЧАСОВ И
// СИНХРОНИЗАЦИИ ЭЛЕКТРОННЫХ ЧАСОВ
// Запись: 0x10 ADR CMD – 0х08
// 1. Один байт с флагами. Младшие четыре бита отражают какой канал включен для выдачи
// импульсов вторичных часов (самый младший бит – канал 1, самый старший – канал 4).
// Старшие четыре бита отражают какой канал включен для синхронизации электронных
// часов. При нулевом значении канал выключен.
static const unsigned char WriteEnableChannelCMD[] = {0x08,0x00};


// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.


// КОМАНДА ЗАПИСИ ВРЕМЕНИ СУТОК ПЕРВИЧНЫХ ЧАСОВ
// Запись: 0x10 ADR CMD – 0х09
// 1. Семь байт текущего времени и даты первичных часов ЧС. Байты идут по порядку часы,
// минуты, секунды, день недели (от 1 - понедельник до 7 - воскресенье), число, месяц, год
// (последние две цифры). Все значения в BCD формате.
static const unsigned char WriteDateTimeCMD[] = {0x09,0x12,0x00,0x00,0x07,0x01,0x09,0x22};


// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.


// КОМАНДА ЗАПИСИ СУТОЧНОЙ КОРРЕКЦИИ И ВРЕМЕНИ КОРРЕКЦИИ
// Запись: 0x10 ADR CMD – 0х0A
// 1. Один байт суточной коррекции. Значение в секундах. Максимальное значение 99
// секунд. Если коррекция отрицательная, то старший бит установлен.
// 2. Один байт значения часа суток в которое происходит коррекция. Коррекция происходит
// один раз в сутки в ХХ:30. ХХ имеет двоично-десятичное значение от 0х00 до 0х23.
static const unsigned char WriteDayCorrectionCMD[] = {0x0A,0x00,0x03};


// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.


// КОМАНДА ЗАПИСИ ЧАСОВОГО ПОЯСА И РАЗРЕШЕНИЯ ПЕРЕХОДА ЗИМА/ЛЕТО
// Запись: 0x10 ADR CMD – 0х0B
// 1. Один байт часового пояса. Значение в часах от 0 до 12. Если это значение
// отрицательное, то старший бит установлен.
// 2. Один байт. Если 0, то автоматический переход зима/лето выключен, если не 0 –
// включен.
static const unsigned char WriteTimeZoneCMD[] = {0x0B,0x03,0x00};


// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;


//Функция подсчёта конечной длины команды с учётом удвоения 0x10 и контрольной суммы
// @param cmd указатель на мвссив байт команды
// @param cmdLen длина массива
uint8_t calculateLenOfCommand(const unsigned char * cmd, const uint8_t cmdLen);


//длина начала команды
const uint8_t startCMDLen = 2;


//длина окончания команды
const uint8_t endOfCMDLen = 3;

//Фунция сборки полной комманды с подсчётом контрольной суммы
// @param startOfCommand указатель на массив байт с началом команды
// @param sclen длина массива начала команды
// @param cmd указатель на массив байт команды
// @param cmdLen длина массива байт команды, для подсчёта использовать calculateLenOfCommand()
// @param endC указатель на массив байт окончания команды
// @param endCL длина массива окончания команды
unsigned char * buildCMD(const unsigned char *cmd = ReadParametrsCMD, uint8_t cmdLen = 1, const unsigned char *startOfCommand = StartCMD, uint8_t sclen = startCMDLen, const unsigned char *endC = endOfCMD, uint8_t endCL = endOfCMDLen);


// @todo функция switch casе выбора команды

#endif
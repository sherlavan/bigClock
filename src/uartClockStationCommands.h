//  @Author: sherlava@gmail.com
//  Описание команд часовой станции по UART
//  Каждая команда представлена в виде массива байт и имеет структуру:
//  {Число байт запроса(параметры + номер команды), Байт номера команды, Ожидаемая длина ответа}
#ifndef UART_COMMANDS_CONSTANTS_H
#define UART_COMMANDS_CONSTANTS_H

#include <inttypes.h>
#include "string.h"
#include <map>
// using namespace std;



//Команды имеют формат {длина команды; CMD; Дина ответа без начала, конца и контрольной суммы (5)}
//т.е. ответ 0 - 10 01 55 10 FE

// Массив начала команды 
// 0x10 - начало передачи 0x01 - адрес устройства на шине
static const unsigned char StartCMD[] = {0x10, 0x01}; //start of packet
static const unsigned char endOfCMD[] = {0x10, 0xFE}; //tail of packet for clock station

//длина начала команды
const uint8_t startCMDLen = 2;


//длина окончания команды
const uint8_t endOfCMDLen = 2;


// КОМАНДА ЧТЕНИЯ ОСНОВНЫХ ПАРАМЕТРОВ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 01 55 10 FE)
// ADR – адрес ЧС; CMD – 0x01
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Один байт напряжения во всех каналах. Число в десятых долях вольта (255 = 25,5В).
// 2. Четыре байта значения тока в каналах на момент последнего срабатывания. Число для
// каждого канала в сотых долях ампера (125 = 1,25А).
// 3. Один байт состояния каналов в данный момент. Каждому каналу соответствует два бита
// в байте. Если оба бита сброшены значит канал выключен. Если в паре более старший бит
// установлен значит импульс положительный. Если в паре более младший бит установлен
// значит импульс отрицательный. В байте более старшая пара бит соответствует первому
// каналу, более младшая пара соответствует четвёртому каналу.
// 4. Один байт, отражающий перегрузку или КЗ в канале. Содержит число то 0 до 4. 0 – всё
// штатно. От 1 до 4 номер канала при перегрузке. При КЗ в байте с номером канала устанавливается старший бит.
// 5. Восемь байт. Положение стрелок вторичных часов. По два байта на канал. Первая пара –
// первый канал, последняя пара – четвёртый канал. В паре первый байт – часы (от 0 до 11),
// второй байт – минуты (от 0 до 59).
// 6. Шесть байт текущего времени и даты первичных часов ЧС. Байты идут по порядку часы,
// минуты, секунды, число, месяц, год (последние две цифры). Все значения в BCD формате
// т.е. 12 записывается как 0х12.
// 7. Один байт отражающий состояние автоматического перехода на летнее/зимнее время.
// Если байт равен нулю, то автоматический переход отключен. Если байт равен 1, то был
// осуществлён переход на летнее время. Если байт равен 2, то был осуществлён переход на
// зимнее время.
// CS 0x10 0xFE.
static const unsigned char ReadParametrsCMD[] = {1, 0x01, 27};



// КОМАНДА ЧТЕНИЯ МЕНЮ «ВТОРИЧНЫЕ ЧАСЫ»
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 02 56 10 FE)
// ADR – адрес ЧС; CMD – 0x02
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Четыре байта длительности импульса в каждом канале по порядку. Число в десятых долях
// секунды. Предельные значения от 2 до 240, что соответствует длительности импульса от
// 0,2 до 24,0 секунды.
// 2. Один байт режима работы каналов. Если равен 0, то работа раздельная. Не равен 0 – совместная.
// 3. Один байт с флагами. Младшие четыре бита отражают какой канал включен для выдачи
// импульсов вторичных часов (самый младший бит – канал 1, самый старший – канал 4).
// Старшие четыре бита отражают какой канал включен для синхронизации электронных часов. При нулевом значении канал выключен.
// CS 0x10 0xFE.
static const unsigned char ReadImpulseCMD[] = {1, 0x02, 11};



// КОМАНДА ЧТЕНИЯ ДАННЫХ ГЛОНАСС ПРИЁМНИКА
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 03 57 10 FE)
// ADR – адрес ЧС; CMD – 0x03
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Один байт: признак данных. ‘A’ – данные актуальны, ‘V’ – данные на актуальны, символ
// ‘N’ – нет данных с приёмника (не подключен или вышел из строя).
// 2. Один байт направления широты: ‘N’-север / ‘S’-юг.
// 3. Шесть байт широты в формате "ГГММмм": ГГ – градусы, ММ – минуты, мм – сотые доли
// минуты.
// 4. Один байт направления долготы: 'E'-восток / 'W'-запад.
// 5. Семь байт долготы в формате "ГГГММмм": ГГГ – градусы, ММ – минуты, мм – сотые доли
// минуты.
// 6. Шесть байт время UTC в формате "ЧЧММСС": ЧЧ – часы, ММ – минуты, СС – секунды.
// CS 0x10 0xFE.
static const unsigned char ReadGlonasCMD[] = {1, 0x03, 27};



// КОМАНДА ЧТЕНИЯ МЕНЮ «ПЕРВИЧНЫЕ ЧАСЫ»
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 04 50 10 FE)
// ADR – адрес ЧС; CMD – 0x04
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Один байт суточной коррекции. Значение в секундах. Максимальное значение 99 секунд.
// Если коррекция отрицательная, то старший бит установлен.
// 2. Один байт значения часа суток в которое происходит коррекция. Коррекция происходит
// один раз в сутки в ХХ:30. ХХ имеет двоично-десятичное значение от 0х00 до 0х23.
// 3. Один байт часового пояса. Значение в часах от 0 до 12. Если это значение отрицательное,
// то старший бит установлен.
// 4. Один байт перехода на летнее время. Равен 0 – переход запрещён, не равен 0 – переход
// разрешён.
// CS 0x10 0xFE
static const unsigned char ReadDayCorrectionCMD[] = {1, 0x04, 9};



// КОМАНДА ЗАПИСИ ВРЕМЕНИ В КАНАЛАХ ВТОРИЧНЫХ ЧАСОВ
// Запись: 0x10 ADR CMD – 0х05
// 1. Один байт номера канала. Значения от 1 до 4.
// 2. Два байта времени в канале. Первый байт – часы (от 0 до 11), второй байт – минуты (от 0
// до 59).
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteTimeInChannelCMD[] = {4, 0x05, 5};



// КОМАНДА ЗАПИСИ ДЛИТЕЛЬНОСТИ ИМПУЛЬСА В КАНАЛАХ
// Запись: 0x10 ADR CMD – 0х06
// 1. Один байт длительности импульса канала 1. Число в десятых долях секунды. Предельные
// значения от 2 до 240, что соответствует длительности импульса от 0,2 до 24,0 секунды.
// 2. Один байт длительности импульса канала 2. Число в десятых долях секунды. Предельные
// значения от 2 до 240, что соответствует длительности импульса от 0,2 до 24,0 секунды.
// 3. Один байт длительности импульса канала 3. Число в десятых долях секунды. Предельные
// значения от 2 до 240, что соответствует длительности импульса от 0,2 до 24,0 секунды.
// 4. Один байт длительности импульса канала 4. Число в десятых долях секунды. Предельные
// значения от 2 до 240, что соответствует длительности импульса от 0,2 до 24,0 секунды.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteImpulseCMD[] = {5, 0x06, 5};


// КОМАНДА ЗАПИСИ ВЫБОРА КАНАЛОВ ВТОРИЧНЫХ ЧАСОВ
// Запись: 0x10 ADR CMD – 0х07
// 1. Один байт режима работы каналов. Если равен 0, то работа раздельная. Не равен 0 – совместная.
// 2. Один байт с флагами. Младшие четыре бита отражают какой канал включен для выдачи
// импульсов вторичных часов (самый младший бит – канал 1, самый старший – канал 4).
// Старшие четыре бита отражают какой канал включен для синхронизации электронных часов. При нулевом значении канал выключен.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteImpulseDurationInChannelCMD[] = {3, 0x07, 5};


// КОМАНДА ЗАПИСИ ВРЕМЕНИ СУТОК ПЕРВИЧНЫХ ЧАСОВ
// Запись: 0x10 ADR CMD – 0х08
// 1. Семь байт текущего времени и даты первичных часов ЧС. Байты идут по порядку часы,
// минуты, секунды, день недели (от 1 - понедельник до 7 - воскресенье), число, месяц, год
// (последние две цифры). Все значения в BCD формате (12 это 0х12).
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteDateTimeCMD[] = {8, 0x08, 5};


// КОМАНДА ЗАПИСИ СУТОЧНОЙ КОРРЕКЦИИ И ВРЕМЕНИ КОРРЕКЦИИ
// Запись: 0x10 ADR CMD – 0х09
// 1. Один байт суточной коррекции. Значение в секундах. Максимальное значение 99 секунд.
// Если коррекция отрицательная, то старший бит установлен.
// 2. Один байт значения часа суток в которое происходит коррекция. Коррекция происходит
// один раз в сутки в ХХ:30. ХХ имеет двоично-десятичное значение от 0х00 до 0х23.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteDayCorrectionCMD[] = {3, 0x09, 5};



// КОМАНДА ЗАПИСИ ЧАСОВОГО ПОЯСА И РАЗРЕШЕНИЯ ПЕРЕХОДА ЗИМА/ЛЕТО
// Запись: 0x10 ADR CMD – 0х0A
// 1. Один байт часового пояса. Значение в часах от 0 до 12. Если это значение отрицательное,
// то старший бит установлен.
// 2. Один байт. Если 0, то автоматический переход зима/лето выключен, если не 0 – включен.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteTimeZoneCMD[] = {3, 0x0A, 5};


// КОМАНДА ЧТЕНИЯ МЕНЮ «РЕЛЕ ПО ВРЕМЕНИ»
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 0B 5F 10 FE)
// ADR – адрес ЧС; CMD – 0x0B
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Один байт включения реле. 0 – реле выключено, 1 – реле подключено к Р1, 2 – реле подключено к Р2.
// 2. Два байта времени включения реле. Первый байт – часы то 0 до 23, второй – минуты от 0
// до 59.
// 3. Два байта времени выключения реле. Первый байт – часы то 0 до 23, второй – минуты от
// 0 до 59.
// 4. Один байт признака включенного реле. Равен 0 – реле выключено, не равен 0 – включено.
// CS 0x10 0xFE.
static const unsigned char ReadReleyStateCMD[] = {1, 0x0B, 11};


// КОМАНДА ЧТЕНИЯ МЕНЮ «АСТРОНОМИЧЕСКОЕ РЕЛЕ»
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 0C 58 10 FE)
// ADR – адрес ЧС; CMD – 0x0C
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Один байт включения реле. 0 – реле выключено, 1 – реле подключено к Р1, 2 – реле подключено к Р2.
// 2. Один байт источника координат. Равен 0 – координаты от ГЛОНАСС приёмника, не равен
// 0 – координаты от ручного ввода.
// 3. Один байт направления широты: ‘N’ север / ‘S’ юг.
// 4. Семь байт широты в формате "ГГГгггг": ГГГ – градусы, гггг – десятитысячные доли градуса.
// 5. Один байт направления долготы: 'E' восток / 'W' запад.
// 6. Семь байт долготы в формате "ГГГгггг": ГГГ – градусы, гггг – десятитысячные доли градуса.
// 7. Один байт признака включенного реле. Равен 0 – реле выключено, не равен 0 – включено.
// CS 0x10 0xFE.
static const unsigned char ReadAstronomicReleyCMD[] = {1, 0x0C, 24};


// КОМАНДА ЗАПИСИ МЕНЮ «РЕЛЕ ПО ВРЕМЕНИ»
// Запись: 0x10 ADR CMD – 0х0D
// 1. Один байт включения реле. 0 – реле выключено, 1 – реле подключено к Р1, 2 – реле подключено к Р2.
// 2. Два байта времени включения реле. Первый байт – часы то 0 до 23, второй – минуты от 0
// до 59.
// 3. Два байта времени выключения реле. Первый байт – часы то 0 до 23, второй – минуты от
// 0 до 59.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteReleyCMD[] = {6, 0x0D, 5};


// КОМАНДА ЗАПИСИ ВКЛЮЧЕНИЯ АСТРОНОМИЧЕСКОГО РЕЛЕ
// Запись: 0x10 ADR CMD – 0х0E
// 1. Один байт включения реле. 0 – реле выключено, 1 – реле подключено к Р1, 2 – реле подключено к Р2.
// 2. Один байт источника координат. Равен 0 – координаты от ГЛОНАСС приёмника, не равен
// 0 – координаты от ручного ввода.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteAstronomicReleyCMD[] = {3, 0x0E, 5};


// КОМАНДА ЗАПИСИ КООРДИНАТ АСТРОНОМИЧЕСКОГО РЕЛЕ
// Запись: 0x10 ADR CMD – 0х0F
// 1. Один байт направления широты: ‘+’ север / ‘-’ юг.
// 2. Семь байт широты в формате "ГГГгггг": ГГГ – градусы, гггг – десятитысячные доли градуса.
// 3. Один байт направления долготы: '+' восток / '-' запад.
// 4. Семь байт долготы в формате "ГГГгггг": ГГГ – градусы, гггг – десятитысячные доли градуса.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteCoordinatesCMD[] = {17, 0x0F, 5};


// КОМАНДА ЧТЕНИЯ МОДЕЛИ ЧАСОВОЙ СТАНЦИИ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 11 45 10 FE)
// ADR – адрес ЧС; CMD – 0x11
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR Строка с моделью станции CS 0x10 0xFE.
static const unsigned char ReadModelCMD[] = {1, 0x11, 0};


// КОМАНДА ЧТЕНИЯ ВЕРСИИ ПРОШИВКИ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 12 46 10 FE)
// ADR – адрес ЧС; CMD – 0x12
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR Строка с версией прошивки CS 0x10 0xFE
static const unsigned char ReadVersionCMD[] = {1, 0x12, 0};


// КОМАНДА ЧТЕНИЯ СЕРИЙНОГО НОМЕРА
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 13 47 10 FE)
// ADR – адрес ЧС; CMD – 0x13
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR Строка с серийным номером CS 0x10 0xFE
static const unsigned char ReadSerialNCMD[] = {1, 0x13, 10};//10 - checkout


// КОМАНДА ЧТЕНИЯ НОМЕРА ЧАСОВОЙ СИСТЕМЫ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 14 40 10 FE)
// ADR – адрес ЧС; CMD – 0x14
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR Строка с номером часовой системы CS 0x10 0xFE
static const unsigned char ReadCSNCMD[] = {1, 0x14, 0};


// КОМАНДА ЧТЕНИЯ ВРЕМЕНИ ДО ТО И ГАРАНТИИ
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 15 41 10 FE)
// ADR – адрес ЧС; CMD – 0x15
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Два байта числа дней до ТО. Первый байт старший.
// 2. Два байта числа дней до конца гарантии. Первый байт старший.
// CS 0x10 0xFE.
static const unsigned char ReadWarrantyCMD[] = {1, 0x15, 9};


// КОМАНДА ЗАПИСИ СЕРИЙНОГО НОМЕРА
// Запись: 0x10 ADR CMD – 0х16
// Строка серийного номера в формате Rцццц: цццц – коды цифр от ‘0’ до ‘9’.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteSerialNCMD[] = {6, 0x16, 5};


// КОМАНДА ЗАПИСИ НОМЕРА ЧАСОВОЙ СИСТЕМЫ
// Запись: 0x10 ADR CMD – 0х17
// Строка номера часовой системы в формате цццццц: цццццц – коды цифр от ‘0’ до ‘9’.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteCSNCMD[] = {7, 0x17, 5};


// КОМАНДА ЗАПИСИ ВРЕМЕНИ ДО ТО И ГАРАНТИИ
// Запись: 0x10 ADR CMD – 0х18
// 1. Два байта числа дней до ТО. Первый байт старший. Максимальное значение 999.
// 2. Два байта числа дней до конца гарантии. Первый байт старший. Максимальное значение
// 9999.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteWarrantyCMD[] = {5, 0x18, 5};


// КОМАНДА ЧТЕНИЯ ДЕМО РЕЖИМА
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 19 4D 10 FE)
// ADR – адрес ЧС; CMD – 0x19
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Два байта числа дней демо режима. Первый байт старший.
// 2. Два байта пин кода.
// 3. Один байт числа попыток ввода пин кода.
// CS 0x10 0xFE.
static const unsigned char ReadDemoCMD[] = {1, 0x19, 10};


// КОМАНДА ЗАПИСИ ДЕМО РЕЖИМА
// Запись: 0x10 ADR CMD – 0х1A
// 1. Два байта числа дней демо режима. Первый байт старший. Максимальное значение 999.
// 2. Два байта пин кода. Значения выражены в BCD формате, т.е. пин код 1234 записывается
// как 0x12 0x34.
// 3. Один байт числа попыток ввода пин кода. Максимальное значение 9.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteDemoCMD[] = {6, 0x1A, 5};


// КОМАНДА ЧТЕНИЯ МЕНЮ «БОЙ ЧАСОВ»
// Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 1B 4F 10 FE)
// ADR – адрес ЧС; CMD – 0x1B
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Один байт часа начала боя 1. Выражен в BCD формате. Значение от 0x00 до 0x23.
// 2. Один байт часа конца боя 1. Выражен в BCD формате. Значение от 0x00 до 0x23.
// 3. Один байт часа начала боя 2. Выражен в BCD формате. Значение от 0x00 до 0x23.
// 4. Один байт часа конца боя 2. Выражен в BCD формате. Значение от 0x00 до 0x23.
// 5. Один байт режима боя. Значения от 0 до 6. 0 – боя нет, 1 – только в 12, 2 – только в 12 и
// 18, 3 – каждый час, 4 – каждые полчаса, 5 – каждые 15 минут, 6 – только бой.
// 6. Один байт схема боя. Значения от 0 до 9.
// 7. Один байт признака «Новогодний бой». Если равен 0, то новогодний бой отключен. Если
// не равен 0 – новогодний бой включен.
// 8. Один байт уровня громкости. Значения от 0 до 30.
// CS 0x10 0xFE.
static const unsigned char ReadChimesCMD[] = {1, 0x1B, 13};


// КОМАНДА ЗАПИСИ ВРЕМЕНИ БОЯ
// Запись: 0x10 ADR CMD – 0х1C
// 1. Один байт часа начала боя 1. Выражен в BCD формате. Значение от 0x00 до 0x23.
// 2. Один байт часа конца боя 1. Выражен в BCD формате. Значение от 0x00 до 0x23.
// 3. Один байт часа начала боя 2. Выражен в BCD формате. Значение от 0x00 до 0x23.
// 4. Один байт часа конца боя 2. Выражен в BCD формате. Значение от 0x00 до 0x23.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteChimesTimeCMD[] = {5, 0x1C, 5};


// КОМАНДА ЗАПИСИ РЕЖИМА И СХЕМЫ БОЯ
// Запись: 0x10 ADR CMD – 0х1D
// 1. Один байт режима боя. Значения от 0 до 6. 0 – боя нет, 1 – только в 12, 2 – только в 12 и
// 18, 3 – каждый час, 4 – каждые полчаса, 5 – каждые 15 минут, 6 – только бой.
// 2. Один байт схема боя. Значения от 0 до 9.
// 3. Один байт признака «Новогодний бой». Если равен 0, то новогодний бой отключен. Если
// не равен 0 – новогодний бой включен.
// CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteChimesCMD[] = {4, 0x1D, 5};


// КОМАНДА ЗАПИСИ УРОВНЯ ГРОМКОСТИ БОЯ
// Запись: 0x10 ADR CMD – 0х1E Один байт уровня громкости. Значения от 0 до 30. CS 0x10 0xFE.
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char WriteChimesVolumeCMD[] = {2, 0x1E, 5};


// КОМАНДА ТЕСТА БОЯ
// Запись: 0x10 ADR CMD – 0х1F CS 0x10 0xFE. (10 01 1F 4B 10 FE)
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char ChimesTestCMD[] = {1, 0x1F, 5};


// КОМАНДА ТЕСТА ПРОИГРЫША
// Запись: 0x10 ADR CMD – 0х20 CS 0x10 0xFE. (10 01 20 74 10 FE)
// Ответ: 0x10 ADR CS 0x10 0xFE Нет ответа если значения не в заданных рамках.
// ADR – адрес ЧС;
// CS = 0x55^ADR.
static const unsigned char MelodyTestCMD[] = {1, 0x20, 5};


//Функция подсчёта конечной длины команды с учётом удвоения 0x10 и контрольной суммы
// @param cmd указатель на мвссив байт команды
// @param cmdLen длина массива
uint8_t calculateLenOfParams(const unsigned char * params, const uint8_t cmdLen);

static unsigned char ParametrsCMD[10]={0};


//Фунция сборки полной комманды с подсчётом контрольной суммы
// @param startOfCommand указатель на массив байт с началом команды
// длина массива startOfCommand передаётся первым элементом
// @param cmd указатель на массив байт команды
// @param cmdLen длина массива байт команды, для подсчёта использовать calculateLenOfCommand()
// @param endC указатель на массив байт окончания команды
// @param endCL длина массива окончания команды
unsigned char * buildCMD(const unsigned char *cmd = ReadParametrsCMD, const unsigned char *startOfCommand = StartCMD, unsigned char *parametrs = ParametrsCMD,  uint8_t sclen = startCMDLen, const unsigned char *endC = endOfCMD, uint8_t endCL = endOfCMDLen);


// функция switch casе выбора команды

std::map<String, const unsigned char* > commandArray = {
    {"MelodyTest", MelodyTestCMD},
    {"ChimesTest", ChimesTestCMD},
    {"ReadParametrs", ReadParametrsCMD},
    {"ReadImpulse", ReadImpulseCMD},
    {"ReadGlonas", ReadGlonasCMD},
    {"ReadDayCorrection", ReadDayCorrectionCMD},
    {"WriteTimeInChannel", WriteTimeInChannelCMD},
    {"WriteImpulse",WriteImpulseCMD},
    {"WriteImpulseDurationInChannel", WriteImpulseDurationInChannelCMD},
    {"WriteDateTime", WriteDateTimeCMD},
    {"WriteDayCorrection", WriteDayCorrectionCMD},
    {"WriteTimeZone", WriteTimeZoneCMD},
    {"ReadReleyState", ReadReleyStateCMD},
    {"ReadAstronomicReley", ReadAstronomicReleyCMD},
    {"WriteReley", WriteReleyCMD},
    {"WriteAstronomicReley", WriteAstronomicReleyCMD},
    {"WriteCoordinates", WriteCoordinatesCMD},
    {"ReadModel", ReadModelCMD},
    {"ReadVersion", ReadVersionCMD},
    {"ReadSerialN", ReadSerialNCMD},
    {"ReadCSN", ReadCSNCMD},
    {"ReadWarranty", ReadWarrantyCMD},
    {"WriteSerialN", WriteSerialNCMD},
    {"WriteCSN", WriteCSNCMD},
    {"WriteWarranty", WriteWarrantyCMD},
    {"ReadDemo", ReadDemoCMD},
    {"WriteDemo", WriteDemoCMD},
    {"ReadChimes", ReadChimesCMD},
    {"WriteChimesTime", WriteChimesTimeCMD},
    {"WriteChimes", WriteChimesCMD},
    {"WriteChimesVolume", WriteChimesVolumeCMD}

};

#endif
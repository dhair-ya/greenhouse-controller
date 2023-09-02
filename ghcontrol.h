/** @brief Gh control constants, structures, function prototypes
 *  @file ghcontrol.h
 */

//Includes
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include "sensehat.h"

//Constants
#ifndef GHCONTROL_H
#define GHCONTROL_H
#define SEARCHSTR "serial\t\t:"
#define SYSINFOBUFSS 512
#define SENSORS 3
#define TEMPERATURE 0
#define HUMIDITY 1
#define PRESSURE 2
#define SIMULATE 0
#define USTEMP 50
#define LSTEMP -10
#define USHUMID 100
#define LSHUMID 0
#define USPRESS 1016
#define LSPRESS 975
#define STEMP 25.0
#define SHUMID 55.0
#define ON 1
#define OFF 0
#define GHUPDATE 2000
#define SIMTEMPERATURE 0
#define SIMHUMIDITY 0
#define SIMPRESSURE 1
#define CTIMESTRSZ 25
#define NUMBARS 8
#define NUMPTS 8.0
#define TBAR 7
#define HBAR 5
#define PBAR 3
#define SENSEHAT 1
#define NALARMS 7
#define UPPERATEMP 30
#define LOWERATEMP 10
#define UPPERAPRESS 1016
#define LOWERAPRESS 985
#define UPPERAHUMID 70
#define LOWERAHUMID 25
#define ALARMNMSZ 18

//Enumerated Types
typedef enum {NOALARM, HTEMP, LTEMP, HHUMID, LHUMID, HPRESS, LPRESS} alarm_e;

//Structures
typedef struct readings
{
    time_t rtime;
    float temperature;
    float humidity;
    float pressure;
}reading_s;

typedef struct setpoints
{
    float temperature;
    float humidity;
}setpoint_s;

typedef struct controls
{
    int heater;
    int humidifier;
}control_s;

typedef struct alarmlimits
{
    float hight;
    float lowt;
    float highp;
    float lowp;
    float highh;
    float lowh;
} alarmlimit_s;

typedef struct alarms
{
    alarm_e code;
    time_t atime;
    float value;
    struct alarms * next;
} alarm_s;

//Function Prototypes
///@cond INTERNAL
void GhDisplayHeader(const char *sname);
u_int64_t GhGetSerial(void);
int GhGetRandom(int range);
void GhDelay(int milliseconds);
void GhControllerInit(void);
void GhDisplayControls(control_s ctrl);
void GhDisplayReadings(reading_s rdata);
void GhDisplayTargets(setpoint_s spts);
float GhGetHumidity(void);
float GhGetPresssure(void);
float GhGetTemperature(void);
reading_s GhGetReadings(void);
setpoint_s GhSetTargets(void);
control_s GhSetControls(setpoint_s target, reading_s data);
int GhLogData(const char *fname, reading_s ghdata);
int GhSaveSetpoints(const char *fname, setpoint_s spts);
setpoint_s GhRetrieveSetpoints(const char *fname);
void GhDisplayAll(readings rd, setpoint_s sd);
int GhSetVerticalBar(int bar, COLOR_SENSEHAT pxc, uint8_t value);
alarmlimit_s GhSetAlarmLimits(void);
alarm_s* GhSetAlarms(alarm_s* head, alarmlimit_s alarmpt, reading_s rdata);
void GhDisplayAlarms(alarm_s* head);
int GhSetOneAlarm(alarm_e code, time_t atime, float value, alarm_s* head);
alarm_s* GhClearOneAlarm(alarm_e code, alarm_s* head);

///@endcond
#endif

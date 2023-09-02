/** @brief Gh control functions
 *  @file ghcontrol.c
 */

#include "ghcontrol.h"
#include <string.h>

//Alarm Message Array
const char alarmnames[NALARMS][ALARMNMSZ] = { "No Alarms", "High Temperature", "Low Temperature", "High Humidity", "Low Humidity", "High Pressure", "Low Pressure"};

//Global SenseHat object
#if SENSEHAT
SenseHat Sh;
#endif

/** Prints Gh Controller Title
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param sname string with Operator's name
* @return void
*/
void GhDisplayHeader(const char * sname)
{
	fprintf(stdout, "%s's CENG153 Greenhouse Controller\n",sname);
}

/** Prints Readings
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param current readings as a readings structure
* @return void
*/
void GhDisplayReadings(reading_s rdata)
{
	fprintf(stdout,"\nUnit: %LX %sReadings\tT: %5.1fC\tH: %5.1f%\tP: %6.1fmb",GhGetSerial(), ctime(&rdata.rtime),rdata.temperature,rdata.humidity,rdata.pressure);
}

/** Sets targets and calls GhDisplayHeader
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param void
* @return void
*/
void GhControllerInit(void)
{
	srand((unsigned) time(NULL));
	GhDisplayHeader("Dhairya Pal");
}



/** Gets the serial number
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param void
* @return serial number
*/
u_int64_t GhGetSerial(void)
{
	static u_int64_t serial = 0;
	FILE * fp;
	char buf[SYSINFOBUFSS];
	char searchstring[] = SEARCHSTR;
	fp = fopen("/proc/cpuinfo", "r");
	if (fp != NULL)
	{
		while (fgets(buf, sizeof(buf), fp) !=NULL)
		{
			if (!strncasecmp(searchstring, buf, strlen(searchstring)))
			{
				sscanf(buf+strlen(searchstring), "%Lx", &serial);
			}
		}
		fclose(fp);
	}
	if (serial==0)
	{
		system("uname -a");
		system("ls --fu /usr/lib/codeblocks | grep -Po '\\.\\K[^ ]+'>stamp.txt");
		fp = fopen("stamp.txt", "r");
		if (fp != NULL)
		{
			while (fgets(buf, sizeof(buf), fp) !=NULL)
			{
				sscanf(buf, "%Lx", &serial);
			}
			fclose(fp);
		}
	}
	return serial;
}

/** Returns a random number in the specified range
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param range as an integer
* @return Random integer
*/
int GhGetRandom(int range)
{
	return rand() % range;
}

/** Holds the program execution until wait time is up
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param milliseconds in integer
* @return void
*/
void GhDelay(int milliseconds)
{
	long wait;
	clock_t now, start;
	wait = milliseconds*(CLOCKS_PER_SEC/1000);
	start = clock();
	now = start;
	while ((now-start) < wait)
	{
		now = clock();
	}
}

/** Sets heater and humidifier on or off
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param setpoints as struct setpoints and current readings as struct readings
* @return controls status (ON or OFF) as struct controls
*/
control_s GhSetControls(setpoint_s target, reading_s rdata)
{
    control_s cset;
	if (rdata.temperature<STEMP)
	{
		cset.heater=ON;
	}
	else
	{
		cset.heater=OFF;
	}
	if (rdata.humidity<SHUMID)
	{
		cset.humidifier=ON;
	}
	else
	{
		cset.humidifier=OFF;
	}
	return cset;
}

/** Displays Standard temperature and humidity
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param setpoints as struct setpoints
* @return void
*/
void GhDisplayTargets(setpoint_s spts)
{
	fprintf(stdout, "\nSetpoints\tT: %.1lfC\tH: %.1lf%\n", spts.temperature, spts.humidity);
}

/** Prints Controls
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param controls status (ON or OFF) as struct controls
* @return void
*/
void GhDisplayControls(control_s ctrl)
{
	fprintf(stdout, "Controls\tHeater: %d\tHumidifer: %d\n",ctrl.heater, ctrl.humidifier);
}


/** Sets standard setpoints temperature and humidity
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param void
* @return targets as struct setpoints
*/
setpoint_s GhSetTargets(void)
{
	setpoint_s cpoints;
	cpoints = GhRetrieveSetpoints("setpoints.dat");
	if (cpoints.temperature == 0)
	{
		cpoints.temperature = STEMP;
		cpoints.humidity = SHUMID;
		GhSaveSetpoints("setpoints.dat", cpoints);
	}
	return cpoints;
}

/** Sets humidity
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param void
* @return humidity as float number
*/
float GhGetHumidity(void)
{
#if SIMHUMIDITY
	return GhGetRandom(USHUMID-LSHUMID)+LSHUMID;
#else
	return Sh.GetHumidity();
#endif
}

/** Sets Pressure
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param void
* @return pressure as float
*/
float GhGetPressure(void)
{
#if SIMPRESSURE
	return GhGetRandom(USPRESS-LSPRESS)+LSPRESS;
#else
	return Sh.GetPressure();
#endif
}

/** Sets temperature
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param void
* @return temperature as float
*/
float GhGetTemperature(void)
{
#if SIMTEMPERATURE
	return GhGetRandom(USTEMP-LSTEMP)+LSTEMP;
#else
	return Sh.GetTemperature();
#endif
}

/** Sets values of sensors(temperature, pressure and humidity)
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param void
* @return current readings as struct readings
*/
reading_s GhGetReadings(void)
{
	reading_s now = {0};

	now.rtime = time(NULL);
	now.temperature = GhGetTemperature();
	now.humidity = GhGetHumidity();
	now.pressure = GhGetPressure();
	return now;
}

/** Prints Controls
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-06-06
* @param log file name and current readings as struct readings
* @return log status (1 for successful, 0 for failed)
*/
int GhLogData(const char *fname, reading_s ghdata)
{
    FILE *fp;
    char ltime[CTIMESTRSZ];
    fp = fopen(fname, "a");
    if (fp == NULL)
    {
        return 0;
    }
    strcpy(ltime, ctime(&ghdata.rtime));
    ltime[3] = ',';
    ltime[7] = ',';
    ltime[10] = ',';
    ltime[19] = ',';
    fprintf(fp, "\n%.24s,%5.1lf,%5.1lf,%6.1lf", ltime, ghdata.temperature, ghdata.humidity, ghdata.pressure);
    fclose(fp);
    return 1;
}

/** Writes setpoints to the file
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-07-04
* @param file name and struct setpoints structure
* @return operation status (1 if successful, 0 if failed)
*/
int GhSaveSetpoints(const char *fname, setpoint_s spts)
{
	FILE *fp;
	fp = fopen(fname, "w");
	if (fp == NULL)
	{
		return 0;
	}
	fwrite(&spts, sizeof(spts), 1, fp);
	fclose(fp);
	return 1;
}

/** Retrieves setpoints from file
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-07-04
* @param log file name
* @return retrieved data as struct sdtpoints
*/
setpoint_s GhRetrieveSetpoints(const char *fname)
{
	setpoint_s spts = {0.0};
	FILE *fp;
	fp = fopen(fname, "r");
	if (fp == NULL)
	{
		return spts;
	}
	fread(&spts, sizeof(spts), 1, fp);
	fclose(fp);
	return spts;
}


/** Displays a green vertical bar for each quantity, and a magenta pixel to indicate the target value
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-07-13
* @param current readings as struct readings and target values as struct setpoints
* @return void
*/
void GhDisplayAll(reading_s rd, setpoint_s sd)
{
	int rv, sv, avh, avl;

	Sh.WipeScreen();

	rv = (NUMPTS * (((rd.temperature - LSTEMP)/(USTEMP - LSTEMP))+0.05))-1.0;
	sv = (NUMPTS * (((sd.temperature - LSTEMP)/(USTEMP - LSTEMP))+0.05))-1.0;

	GhSetVerticalBar(TBAR,GREEN,rv);
	Sh.LightPixel(sv, TBAR, MAGENTA);

	rv = (NUMPTS * (((rd.humidity - LSHUMID)/(USHUMID - LSHUMID))+0.05))-1.0;
	sv = (NUMPTS * (((sd.humidity - LSHUMID)/(USHUMID - LSHUMID))+0.05))-1.0;

	GhSetVerticalBar(HBAR, GREEN, rv);
	Sh.LightPixel(sv, HBAR, MAGENTA);

	rv = (NUMPTS * (((rd.pressure - LSPRESS)/(USPRESS - LSPRESS))+0.05))-1.0;

	GhSetVerticalBar(PBAR, GREEN, rv);
}


/** Sets a vertical bar on the SenseHat
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-07-13
* @param int bar to light
* @param fbpixel_s pixel colour data
* @param uint8_t how many pixels to light
* @return exit status
*/
int GhSetVerticalBar(int bar, COLOR_SENSEHAT pxc, uint8_t value)
{
	int i;

	if (value>7)
	{	value = 7; }
	if (bar >= 0 && bar < 8 && value >=0 && value < 8)
	{
		for (i = 0; i<= value; i++)
		{
			Sh.LightPixel(i, bar, pxc);
		}
		for (i = value+1; i < 8; i++)
		{
			Sh.LightPixel(i, bar, BLACK);
		}
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
}


/** Sets alarm limits
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-07-25
* @param void
* @return alarmlimit_s object that has set alarm limits
*/
alarmlimit_s GhSetAlarmLimits(void)
{
    alarmlimit_s calarm;

    calarm.hight = UPPERATEMP;
    calarm.lowt = LOWERATEMP;
    calarm.highp = UPPERAPRESS;
    calarm.lowp = LOWERAPRESS;
    calarm.highh = UPPERAHUMID;
    calarm.lowh = LOWERAHUMID;

    return calarm;
}


/** Sets alarm
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-08-07
* @param alarm_s* object
* @param alarmlimit_s object
* @param reading_s object
* @return alarm_s* object
*/
alarm_s* GhSetAlarms(alarm_s * head, alarmlimit_s alarmpt, reading_s rdata)
{
    if (rdata.temperature >= alarmpt.hight)
    { GhSetOneAlarm(HTEMP, rdata.rtime, rdata.temperature, head); }
    else
    { head = GhClearOneAlarm(HTEMP, head); }


    if (rdata.temperature <= alarmpt.lowt)
    { GhSetOneAlarm(LTEMP, rdata.rtime, rdata.temperature, head); }
    else
    { head = GhClearOneAlarm(LTEMP, head); }


    if (rdata.humidity >= alarmpt.highh)
    { GhSetOneAlarm(HHUMID, rdata.rtime, rdata.humidity, head); }
    else
    { head = GhClearOneAlarm(HHUMID, head); }


    if (rdata.humidity <= alarmpt.lowh)
    { GhSetOneAlarm(LHUMID, rdata.rtime, rdata.humidity, head); }
    else
    { head = GhClearOneAlarm(LHUMID, head); }


    if (rdata.pressure >= alarmpt.highp)
    { GhSetOneAlarm(HPRESS, rdata.rtime, rdata.pressure, head); }
    else
    { head = GhClearOneAlarm(HPRESS, head); }


    if (rdata.pressure <= alarmpt.lowp)
    { GhSetOneAlarm(LPRESS, rdata.rtime, rdata.pressure, head); }
    else
    { head = GhClearOneAlarm(LPRESS, head); }
    return head;
}


/** Displays alarm
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-08-07
* @param alarm_s* pointer
* @return void
*/
void GhDisplayAlarms(alarm_s* head)
{
    alarm_s* cur;

    cur = head;

	fprintf(stdout, "\nAlarms\n");

    while (cur != NULL)
	{
		if(cur->code == HTEMP) { fprintf(stdout, "%s %s", alarmnames[HTEMP], ctime(&cur->atime)); }
        else if(cur->code == LTEMP) { fprintf(stdout, "%s %s", alarmnames[LTEMP], ctime(&cur->atime)); }
        else if(cur->code == HHUMID) { fprintf(stdout, "%s %s", alarmnames[HHUMID], ctime(&cur->atime)); }
        else if(cur->code == LHUMID) { fprintf(stdout, "%s %s", alarmnames[LHUMID], ctime(&cur->atime)); }
        else if(cur->code == HPRESS) { fprintf(stdout, "%s %s", alarmnames[HPRESS], ctime(&cur->atime)); }
        else if(cur->code == LPRESS) { fprintf(stdout, "%s %s", alarmnames[LPRESS], ctime(&cur->atime)); }
        cur = cur->next;
	}
}

/** Sets one alarm
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-08-01
* @param alarm_e object
* @param time_t object
* @param float object
* @param alarm_s* object
* @return int status code
*/
int GhSetOneAlarm(alarm_e code, time_t atime, float value, alarm_s* head)
{
    alarm_s* cur;
    alarm_s* last;

    cur = head;

    if(cur->code != NULL)
    {
        while(cur != NULL)
        {
            if(cur->code == code)
            {
                cur->atime = atime;
                cur->value = value;
                return 0;
            }
            last = cur;
            cur = cur->next;
        }
        cur = (alarm_s*) calloc(1, sizeof(alarm_s));
        if(cur == NULL) { return 0; }
        last->next = cur;
    }
    cur->code = code;
    cur->atime = atime;
    cur->value = value;
    cur->next = NULL;
    return 1;
}

/** Clears one alarm
* @version CENG153, serial: 1AE1741E
* @author Paul Moggach
* @author Dhairya Pal
* @since 2023-08-01
* @param alarm_e object
* @param alarm_s* object
* @return int status code
*/
alarm_s* GhClearOneAlarm(alarm_e code, alarm_s* head)
{
    alarm_s* cur;
    alarm_s* last;

    cur = head;
    last = head;

    //Clear one if a match for the alarm code, and only one item in the list
    if((cur->code == code) && (cur->next == NULL))
    {
        cur->code = NOALARM;
        return head;
    }

    //Clear one if a match for the alarm code for the first item in a multi-item list
    if((cur->code == code) && (cur->next != NULL))
    {
        head = cur->next;
        free(cur);
        return head;
    }

    //CHECK IF THIS IS CORRECT
    while(cur != NULL)
    {
        if(cur->code == code)
        {
            last->next = cur->next;
            free(cur);
            return head;
        }
        last = cur;
        cur = cur->next;
    }
    return head;
}

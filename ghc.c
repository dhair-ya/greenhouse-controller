/** @brief Defines the entry point for the console application.
 *  @file ghc.c
 */
#include "ghcontrol.h"

int main(void)
{
	int logged;
	reading_s creadings={0};
	control_s ctrl = {0};
	setpoint_s sets = {0};
	alarmlimit_s alimits = {0};

    alarm_s* arecord;
    arecord = (alarm_s*) calloc(1, sizeof(alarm_s));

	GhControllerInit();

	if(arecord == NULL)
    {
        fprintf(stdout, "\nCannot allocate memory\n");
        return EXIT_FAILURE;
    }

	sets = GhSetTargets();
	alimits = GhSetAlarmLimits();

	while (1>0)
	{
		creadings = GhGetReadings();
		logged = GhLogData("ghdata.txt", creadings);
		ctrl = GhSetControls(sets, creadings);
		arecord = GhSetAlarms(arecord, alimits, creadings);
		GhDisplayAll(creadings, sets);
		GhDisplayReadings(creadings);
		GhDisplayTargets(sets);
		GhDisplayControls(ctrl);
		GhDisplayAlarms(arecord);
		GhDelay(GHUPDATE);
	}
	fprintf(stdout, "Press ENTER to continue...");
	fgetc(stdin);
	return EXIT_FAILURE;
}

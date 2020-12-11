
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "stdsoap2.h"
#include "soapH.h"
#include "onvif_param.h"

/** time zone information */
typedef struct tag_time_zone_info {
	const char *tz_name; /* time zone name */
	int tz_value; /* time differential value of the UTC */

} time_zone_info_t;

static time_zone_info_t s_tz_list[] = {
	{ "DatelineStandardTime12", -43200 }, /* UTC-12:00 */
	{ "<UTC-11>11", -39600 }, /* UTC-11:00 */
	{ "AleutianStandardTime10DaylightTime,M3.2.0,M11.1.0", -36000 }, /* UTC-10:00 */
	{ "HawaiianStandardTime10", -36000 }, /* UTC-10:00 */
	{ "MarquesasStandardTime9:30", -34200 }, /* UTC-9:30 */
	{ "AlaskanStandardTime9DaylightTime,M3.2.0,M11.1.0", -32400 }, /* UTC-09:00 */
	{ "<UTC-09>9", -32400 }, /* UTC-09:00 */
	{ "PacificStandardTime8DaylightTime,M3.2.0,M11.1.0", -28800 }, /* UTC-08:00 */
	{ "PacificStandardTimeMexico8DaylightTime,M3.2.0,M11.1.0", -28800 }, /* UTC-08:00 */
	{ "<UTC-08>8", -28800 }, /* UTC-08:00 */
	{ "MountainStandardTimeMexico7DaylightTime,M4.1.0,M10.5.0",-25200 }, /* UTC-07:00 */
	{ "MountainStandardTime7DaylightTime,M3.2.0,M11.1.0", -25200 }, /* UTC-07:00 */
	{ "USMountainStandardTime7", -25200 }, /* UTC-07:00 */
	{ "EasterIslandStandardTime6DaylightTime,M8.2.6/22,M5.2.6/22", -21600 }, /* UTC-06:00 */
	{ "CentralStandardTimeMexico6DaylightTime,M4.1.0,M10.5.0", -21600 }, /* UTC-06:00 */
	{ "CanadaCentralStandardTime6", -21600 }, /* UTC-06:00 */
	{ "CentralStandardTime6DaylightTime,M3.2.0,M11.1.0", -21600 }, /* UTC-06:00 */
	{ "CentralAmericaStandardTime6", -21600 }, /* UTC-06:00 */
	{ "SAPacificStandardTime5", -18000 }, /* UTC-05:00 */
	{ "EasternStandardTime5DaylightTime,M3.2.0,M11.1.0", -18000 }, /* UTC-05:00 */
	{ "CubaStandardTime5DaylightTime,M3.2.0/0,M11.1.0/1", -18000 }, /* UTC-05:00 */
	{ "HaitiStandardTime5DaylightTime,M3.2.0,M11.1.0", -18000 }, /* UTC-05:00 */
	{ "EasternStandardTimeMexico5", -18000 }, /* UTC-05:00 */
	{ "TurksAndCaicosStandardTime5DaylightTime,M3.2.0,M11.1.0", -18000 }, /* UTC-05:00 */
	{ "USEasternStandardTime5DaylightTime,M3.2.0,M11.1.0", -18000 }, /* UTC-05:00 */
	{ "AtlanticStandardTime4DaylightTime,M3.2.0,M11.1.0", -14400 }, /* UTC-04:00 */
	{ "VenezuelaStandardTime4", -14400 }, /* UTC-04:00 */
	{ "CentralBrazilianStandardTime4DaylightTime,M10.5.6/23:59:59,M2.3.6/23:59:59", -14400 }, /* UTC-04:00 */
	{ "SAWesternStandardTime4", -14400 }, /* UTC-04:00 */
	{ "PacificSAStandardTime4DaylightTime,M8.2.6/23:59:59,M5.2.6/23:59:59", -14400 }, /* UTC-04:00 */
	{ "ParaguayStandardTime4DaylightTime,M10.1.6/23:59:59,M3.3.6/23:59:59", -14400 }, /* UTC-04:00 */
	{ "NewfoundlandStandardTime3:30DaylightTime,M3.2.0,M11.1.0", -12600 }, /* UTC-03:30 */
	{ "TocantinsStandardTime3", -10800 }, /* UTC-03:00 */
	{ "ESouthAmericaStandardTime3DaylightTime,M10.5.6/23:59:59,M2.3.6/23:59:59", -10800 }, /* UTC-03:00 */
	{ "ArgentinaStandardTime3", -10800 }, /* UTC-03:00 */
	{ "GreenlandStandardTime3DaylightTime,M3.5.6/22,M10.4.6/23", -10800 }, /* UTC-03:00 */
	{ "SAEasternStandardTime3", -10800 }, /* UTC-03:00 */
	{ "MontevideoStandardTime3", -10800 }, /* UTC-03:00 */
	{ "MagallanesStandardTime3", -10800 }, /* UTC-03:00 */
	{ "BahiaStandardTime3", -10800 }, /* UTC-03:00 */
	{ "SaintPierreStandardTime3DaylightTime,M3.2.0,M11.1.0", -10800 }, /* UTC-03:00 */
	{ "<UTC-02>2", -7200 }, /* UTC-02:00 */
	{ "<Mid-AtlanticStandardTime>2DaylightTime,M3.5.0,M9.5.0", -7200 }, /* UTC-02:00 */
	{ "CapeVerdeStandardTime1", -3600 }, /* UTC-01:00 */
	{ "UTC0", 0 }, /* UTC */
	{ "GMTStandardTime0DaylightTime,M3.5.0/1,M10.5.0", 0 }, /* UTC+00:00 */
	{ "MoroccoStandardTime0DaylightTime,M3.5.0,M10.4.0/3", 0 }, /* UTC+00:00 */
	{ "GreenwichStandardTime0", 0 }, /* UTC+00:00 */
	{ "WEuropeStandardTime-1DaylightTime,M3.5.0,M10.5.0/3", 3600 }, /* UTC+01:00 */
	{ "CentralEuropeStandardTime-1DaylightTime,M3.5.0,M10.5.0/3", 3600 }, /* UTC+01:00 */
	{ "RomanceStandardTime-1DaylightTime,M3.5.0,M10.5.0/3", 3600 }, /* UTC+01:00 */
	{ "CentralEuropeanStandardTime-1DaylightTime,M3.5.0,M10.5.0/3", 3600 }, /* UTC+01:00 */
	{ "SaoTomeStandardTime-1", 3600 }, /* UTC+01:00 */
	{ "WCentralAfricaStandardTime-1", 3600 }, /* UTC+01:00 */
	{ "JordanStandardTime-2DaylightTime,M3.5.4/23:59:59,M10.5.5/1", 7200 }, /* UTC+02:00 */
	{ "MiddleEastStandardTime-2DaylightTime,M3.5.6/23:59:59,M10.4.6/23:59:59", 7200 }, /* UTC+02:00 */
	{ "SyriaStandardTime-2DaylightTime,M3.5.5/0,M10.5.4/23:59:59", 7200 }, /* UTC+02:00 */
	{ "LibyaStandardTime-2", 7200 }, /* UTC+02:00 */
	{ "SouthAfricaStandardTime-2", 7200 }, /* UTC+02:00 */
	{ "FLEStandardTime-2DaylightTime,M3.5.0/3,M10.5.0/4", 7200 }, /* UTC+02:00 */
	{ "EEuropeStandardTime-2DaylightTime,M3.5.0,M10.5.0/3", 7200 }, /* UTC+02:00 */
	{ "KaliningradStandardTime-2", 7200 }, /* UTC+02:00 */
	{ "WestBankStandardTime-2DaylightTime,M3.4.6/1,M10.5.6/1", 7200 }, /* UTC+02:00 */
	{ "SudanStandardTime-2", 7200 }, /* UTC+02:00 */
	{ "EgyptStandardTime-2", 7200 }, /* UTC+02:00 */
	{ "NamibiaStandardTime-2", 7200 }, /* UTC+02:00 */
	{ "GTBStandardTime-2DaylightTime,M3.5.0/3,M10.5.0/4", 7200 }, /* UTC+02:00 */
	{ "IsraelStandardTime-2DaylightTime,M3.5.5,M10.5.0", 7200 }, /* UTC+02:00 */
	{ "ArabicStandardTime-3", 10800 }, /* UTC+03:00 */
	{ "ArabStandardTime-3", 10800 }, /* UTC+03:00 */
	{ "BelarusStandardTime-3", 10800 }, /* UTC+03:00 */
	{ "RussianStandardTime-3", 10800 }, /* UTC+03:00 */
	{ "EAfricaStandardTime-3", 10800 }, /* UTC+03:00 */
	{ "TurkeyStandardTime-3", 10800 }, /* UTC+03:00 */
	{ "IranStandardTime-3:30DaylightTime,M3.3.6/0,M9.3.0/23:59:59", 12600 }, /* UTC+03:30 */
	{ "ArabianStandardTime-4", 14400 }, /* UTC+04:00 */
	{ "AstrakhanStandardTime-4", 14400 }, /* UTC+04:00 */
	{ "CaucasusStandardTime-4", 14400 }, /* UTC+04:00 */
	{ "AzerbaijanStandardTime-4", 14400 }, /* UTC+04:00 */
	{ "GeorgianStandardTime-4", 14400 }, /* UTC+04:00 */
	{ "MauritiusStandardTime-4", 14400 }, /* UTC+04:00 */
	{ "SaratovStandardTime-4", 14400 }, /* UTC+04:00 */
	{ "<RussiaTimeZone3>-4", 14400 }, /* UTC+04:00 */
	{ "AfghanistanStandardTime-4:30", 16200 }, /* UTC+04:30 */
	{ "WestAsiaStandardTime-5", 18000 }, /* UTC+05:00 */
	{ "EkaterinburgStandardTime-5", 18000 }, /* UTC+05:00 */
	{ "PakistanStandardTime-5", 18000 }, /* UTC+05:00 */
	{ "IndiaStandardTime-5:30", 19800 }, /* UTC+05:30 */
	{ "SriLankaStandardTime-5:30", 19800 }, /* UTC+05:30 */
	{ "NepalStandardTime-5:45", 20700 }, /* UTC+05:45 */
	{ "CentralAsiaStandardTime-6", 21600 }, /* UTC+06:00 */
	{ "BangladeshStandardTime-6", 21600 }, /* UTC+06:00 */
	{ "OmskStandardTime-6", 21600 }, /* UTC+06:00 */
	{ "MyanmarStandardTime-6:30", 23400 }, /* UTC+06:30 */
	{ "AltaiStandardTime-7", 25200 }, /* UTC+07:00 */
	{ "WMongoliaStandardTime-7", 25200 }, /* UTC+07:00 */
	{ "NorthAsiaStandardTime-7", 25200 }, /* UTC+07:00 */
	{ "SEAsiaStandardTime-7", 25200 }, /* UTC+07:00 */
	{ "TomskStandardTime-7", 25200 }, /* UTC+07:00 */
	{ "NCentralAsiaStandardTime-7", 25200 }, /* UTC+07:00 */
	{ "ChinaStandardTime-8", 28800 }, /* UTC+08:00 */
	{ "SingaporeStandardTime-8", 28800 }, /* UTC+08:00 */
	{ "WAustraliaStandardTime-8", 28800 }, /* UTC+08:00 */
	{ "TaipeiStandardTime-8", 28800 }, /* UTC+08:00 */
	{ "UlaanbaatarStandardTime-8", 28800 }, /* UTC+08:00 */
	{ "NorthAsiaEastStandardTime-8", 28800 }, /* UTC+08:00 */
	{ "AusCentralWStandardTime-8:45", 31500 }, /* UTC+08:45 */
	{ "TransbaikalStandardTime-9", 32400 }, /* UTC+09:00 */
	{ "TokyoStandardTime-9", 32400 }, /* UTC+09:00 */
	{ "NorthKoreaStandardTime-9", 32400 }, /* UTC+09:00 */
	{ "KoreaStandardTime-9", 32400 }, /* UTC+09:00 */
	{ "YakutskStandardTime-9", 32400 }, /* UTC+09:00 */
	{ "CenAustraliaStandardTime-9:30DaylightTime,M10.1.0,M4.1.0/3", 34200 }, /* UTC+09:30 */
	{ "AUSCentralStandardTime-9:30", 34200 }, /* UTC+09:30 */
	{ "EAustraliaStandardTime-10", 36000 }, /* UTC+10:00 */
	{ "VladivostokStandardTime-10", 36000 }, /* UTC+10:00 */
	{ "WestPacificStandardTime-10", 36000 }, /* UTC+10:00 */
	{ "TasmaniaStandardTime-10DaylightTime,M10.1.0,M4.1.0/3", 36000 }, /* UTC+10:00 */
	{ "AUSEasternStandardTime-10DaylightTime,M10.1.0,M4.1.0/3", 36000 }, /* UTC+10:00 */
	{ "LordHoweStandardTime-10:30DaylightTime-11,M10.1.0,M4.1.0", 37800 }, /* UTC+10:30 */
	{ "BougainvilleStandardTime-11", 39600 }, /* UTC+11:00 */
	{ "MagadanStandardTime-11", 39600 }, /* UTC+11:00 */
	{ "NorfolkStandardTime-11", 39600 }, /* UTC+11:00 */
	{ "<RussiaTimeZone10>-11", 39600 }, /* UTC+11:00 */
	{ "SakhalinStandardTime-11", 39600 }, /* UTC+11:00 */
	{ "CentralPacificStandardTime-11", 39600 }, /* UTC+11:00 */
	{ "<RussiaTimeZone11>-12", 43200 }, /* UTC+12:00 */
	{ "NewZealandStandardTime-12DaylightTime,M9.5.0,M4.1.0/3", 43200 }, /* UTC+12:00 */
	{ "KamchatkaStandardTime-12DaylightTime,M3.5.0,M10.5.0/3", 43200 }, /* UTC+12:00 */
	{ "FijiStandardTime-12DaylightTime,M11.1.0,M1.3.0/3", 43200 }, /* UTC+12:00 */
	{ "<UTC+12>-12", 43200 }, /* UTC+12:00 */
	{ "ChathamIslandsStandardTime-12:45DaylightTime,M9.5.0/2:45,M4.1.0/3:45", 45900 }, /* UTC+12:45 */
	{ "TongaStandardTime-13", 46800 }, /* UTC+13:00 */
	{ "SamoaStandardTime-13DaylightTime,M9.5.0/3,M4.1.0/4", 46800 }, /* UTC+13:00 */
	{ "<UTC+13>-13", 46800 }, /* UTC+13:00 */
	{ "LineIslandsStandardTime-14", 50400 }, /* UTC+14:00 */

};

int onvif_set_timezone(char *TZ)
{
	int i;

	if (!TZ)
		return -1;

	for (i = 0; i< sizeof(s_tz_list) / sizeof(time_zone_info_t); i++) {
		if (!strcmp(s_tz_list[i].tz_name, TZ)) {
			onvif_set_timezone_index(i);
			break;
		}
	}

	return 0;
}

const char *onvif_get_timezone(int index)
{
	int count;

	count = sizeof(s_tz_list) / sizeof(time_zone_info_t);

	if ((index < 0) || (index >= count))
		return NULL;

	return s_tz_list[index].tz_name;
}

int onvif_get_time_value(char *TZ)
{
	int i;

	if (!TZ)
		return 0;

	for (i = 0; i< sizeof(s_tz_list) / sizeof(time_zone_info_t); i++) {
		if (!strcmp(s_tz_list[i].tz_name, TZ)) {
			return s_tz_list[i].tz_value;
		}
	}

	return 0;
}

int onvif_fault(struct soap *soap, char *value1, char *value2)
{
	soap->fault = (struct SOAP_ENV__Fault*)soap_malloc(soap,
		(sizeof(struct SOAP_ENV__Fault)));

	if (soap->fault == NULL) {
		printf("Failed to malloc for fault.\n");
		return SOAP_FAULT;
	}

	soap->fault->SOAP_ENV__Code =
		(struct SOAP_ENV__Code*)soap_malloc(soap,
		(sizeof(struct SOAP_ENV__Code)));

	if (soap->fault->SOAP_ENV__Code == NULL) {
		printf("Failed to malloc for SOAP_ENV__Code.\n");
		return SOAP_FAULT;
	}

	soap->fault->SOAP_ENV__Code->SOAP_ENV__Value = "SOAP-ENV:Sender";
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode =
		(struct SOAP_ENV__Code*)soap_malloc(soap,
		(sizeof(struct SOAP_ENV__Code)));;

	if (soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode == NULL) {
		printf("Failed to malloc for SOAP_ENV__Subcode.\n");
		return SOAP_FAULT;
	}

	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Value =
		value1;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode =
		(struct SOAP_ENV__Code*)soap_malloc(soap,
		(sizeof(struct SOAP_ENV__Code)));

	if (soap->fault->SOAP_ENV__Code->
			SOAP_ENV__Subcode->SOAP_ENV__Subcode == NULL) {
		printf("Failed to malloc for SOAP_ENV__Code.\n");
		return SOAP_FAULT;
	}

	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->
		SOAP_ENV__Subcode->SOAP_ENV__Value = value2;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->
		SOAP_ENV__Subcode->SOAP_ENV__Subcode = NULL;
	soap->fault->faultcode = NULL;
	soap->fault->faultstring = NULL;
	soap->fault->faultactor = NULL;
	soap->fault->detail = NULL;
	soap->fault->SOAP_ENV__Reason = NULL;
	soap->fault->SOAP_ENV__Node = NULL;
	soap->fault->SOAP_ENV__Role = NULL;
	soap->fault->SOAP_ENV__Detail = NULL;

	return SOAP_OK;
}

/* @brief Check if a hostname is valid */
int isValidHostname(char *str)
{
	/* Catch NULL pointer. */
	if (str == NULL)
		return 0;

	/* Process every character in string. */
	while (*str != '\0')
	{
		if ((*str >= 'a' && *str <= 'z')
			|| (*str >= 'A' && *str <= 'Z')
			|| (*str >= '0' && *str <= '9')
			|| (*str == '.') || (*str == '-'))
			str++;
		else
			return 0;
	}

	return 1;
}


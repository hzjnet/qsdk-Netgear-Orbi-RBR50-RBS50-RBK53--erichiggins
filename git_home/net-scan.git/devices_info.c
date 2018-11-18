#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netscan.h"

#define DEVICEFILE	"/tmp/device_tables/local_device_table"
#define MAXDEVICE	512
#define BUFSIZE		512
#define MACSIZE		32
#define TYPESIZE	32
#define MODELSIZE	32
#define NAMESIZE	255

typedef struct device_info {
	char mac[MACSIZE];
	char type[TYPESIZE];
	char model[MODELSIZE];
	char name[NAMESIZE];
	char type_s[TYPESIZE];
	char model_s[MODELSIZE];
	char name_s[NAMESIZE];
	unsigned int flag;
} DeviceInfo;

enum {
	CUSTOMIZE,
	UPDATE
};

int main(int argc, char ** argv) {
	FILE *fp;
	DeviceInfo newInfo, matchInfo;
	DeviceInfo Info[MAXDEVICE];
	char line[BUFSIZE], *newline, *tmpmac;
	int i, count, match = 0, type;
	strncpy(matchInfo.type_s, "NULL", TYPESIZE);
	strncpy(matchInfo.model_s, "NULL", MODELSIZE);
	strncpy(matchInfo.name_s, "NULL", NAMESIZE);

	if (argc == 6 && (strcmp(argv[1], "customizename") == 0 || strcmp(argv[1], "customize") == 0)) {
		if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL || argv[5] == NULL){
			DEBUGP("Usage:%s {customizename mac type model name}\n", argv[0]);
			return -1;
		}
		type = CUSTOMIZE;
		strlcpy(newInfo.mac, argv[2], MACSIZE);
		strlcpy(newInfo.type, argv[3], TYPESIZE);
		strlcpy(newInfo.model, argv[4], MODELSIZE);
		strlcpy(newInfo.name, argv[5], NAMESIZE);
		newInfo.flag = 1;
	} else if(argc == 6 && strcmp(argv[1], "update") == 0) {
		if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL){
			DEBUGP("Usage:%s {update mac type model}\n", argv[0]);
			return -1;
		}
		type = UPDATE;
		strlcpy(newInfo.mac, argv[2], MACSIZE);
		strlcpy(newInfo.type, argv[3], TYPESIZE);
		strlcpy(newInfo.model, argv[4], MODELSIZE);
		strlcpy(newInfo.name, argv[5], NAMESIZE);
		newInfo.flag = 0;
	} else {
		DEBUGP("Usage:%s {customizename mac type model name|update mac type model}\n", argv[0]);
		return -1;
	}

	fp = fopen(DEVICEFILE, "r");
	if (fp == NULL) { DEBUGP("Failed to open %s\n", DEVICEFILE); return -1;}

	for (i = 0; fgets(line, sizeof(line), fp) && i < MAXDEVICE; i++) {
		newline = strtok(line, "\n");
		tmpmac = strtok(newline, ",");
		if (match == 1 || strncmp(tmpmac, newInfo.mac, MACSIZE) != 0) {
			strncpy(Info[i].mac, tmpmac, MACSIZE);
			strncpy(Info[i].type, strtok(NULL, ","), TYPESIZE);
			strncpy(Info[i].model, strtok(NULL, ","), MODELSIZE);
			strncpy(Info[i].name, strtok(NULL, ","), NAMESIZE);
			Info[i].flag = atoi(strtok(NULL, ","));
			if (Info[i].flag == 0) {
				strncpy(Info[i].type_s, Info[i].type, TYPESIZE);
				strncpy(Info[i].model_s, Info[i].model, MODELSIZE);
				strncpy(Info[i].name_s, Info[i].name, NAMESIZE);
			} else {
				strncpy(Info[i].type_s, strtok(NULL, ","), TYPESIZE);
				strncpy(Info[i].model_s, strtok(NULL, ","), MODELSIZE);
				strncpy(Info[i].name_s, strtok(NULL, ","), NAMESIZE);
			}
		} else {
			i--;
			match = 1;
			strlcpy(matchInfo.mac, tmpmac, MACSIZE);
			strlcpy(matchInfo.type, strtok(NULL, ","), TYPESIZE);
			strlcpy(matchInfo.model, strtok(NULL, ","), MODELSIZE);
			strlcpy(matchInfo.name, strtok(NULL, ","), NAMESIZE);
			matchInfo.flag = atoi(strtok(NULL, ","));
			if (matchInfo.flag == 0) {
				strlcpy(matchInfo.type_s, matchInfo.type, TYPESIZE);
				strlcpy(matchInfo.model_s, matchInfo.model, MODELSIZE);
				strlcpy(matchInfo.name_s, matchInfo.name, NAMESIZE);
			} else {
				strlcpy(matchInfo.type_s, strtok(NULL, ","), TYPESIZE);
				strlcpy(matchInfo.model_s, strtok(NULL, ","), MODELSIZE);
				strlcpy(matchInfo.name_s, strtok(NULL, ","), NAMESIZE);
				if ((type == UPDATE) && (strncmp(matchInfo.type_s, "NULL", 4) == 0 || strncmp(matchInfo.model_s, "NULL", 4) == 0)) {
					strlcpy(matchInfo.type_s, newInfo.type, TYPESIZE);
					strlcpy(matchInfo.model_s, newInfo.model, MODELSIZE);
					strlcpy(matchInfo.name_s, newInfo.name, NAMESIZE);
				}
			}
		}
	}
	fclose(fp);

	if (i == MAXDEVICE) {
		count = i;
		strncpy(Info[0].mac, newInfo.mac, MACSIZE);
		strncpy(Info[0].type, newInfo.type, TYPESIZE);
		strncpy(Info[0].model, newInfo.model, MODELSIZE);
		strncpy(Info[0].name, newInfo.name, NAMESIZE);
		Info[0].flag = newInfo.flag;
		strncpy(Info[0].type_s, newInfo.type, TYPESIZE);
		strncpy(Info[0].model_s, newInfo.model, MODELSIZE);
		strncpy(Info[0].name_s, newInfo.name, NAMESIZE);
	} else {
		count = i + 1;
		if (type == CUSTOMIZE || match == 0 || (match == 1 && type == UPDATE && matchInfo.flag == 0)) {
			strncpy(Info[i].mac, newInfo.mac, MACSIZE);
			strncpy(Info[i].type, newInfo.type, TYPESIZE);
			strncpy(Info[i].model, newInfo.model, MODELSIZE);
			strncpy(Info[i].name, newInfo.name, NAMESIZE);
			strncpy(Info[i].type_s, matchInfo.type_s, TYPESIZE);
			strncpy(Info[i].model_s, matchInfo.model_s, MODELSIZE);
			strncpy(Info[i].name_s, matchInfo.name_s, NAMESIZE);
			Info[i].flag = newInfo.flag;
			if (strncmp(Info[i].type, "0", 1) == 0) {
				strncpy(Info[i].type_s, "0", TYPESIZE);
				strncpy(Info[i].model_s, "Netgear", MODELSIZE);
				strncpy(Info[i].name_s, "Orbi Satellite", NAMESIZE);
			}
		} else {
			strncpy(Info[i].mac, matchInfo.mac, MACSIZE);
			strncpy(Info[i].type, matchInfo.type, TYPESIZE);
			strncpy(Info[i].model, matchInfo.model, MODELSIZE);
			strncpy(Info[i].name, matchInfo.name, NAMESIZE);
			strncpy(Info[i].type_s, matchInfo.type_s, TYPESIZE);
			strncpy(Info[i].model_s, matchInfo.model_s, MODELSIZE);
			strncpy(Info[i].name_s, matchInfo.name_s, NAMESIZE);
			Info[i].flag = matchInfo.flag;
		}
	}

	fp = fopen(DEVICEFILE, "w");
	if (fp == NULL) { DEBUGP("Failed to open %s\n", DEVICEFILE); return -1;}

	for (i = 0; i < count; i++) 
		fprintf(fp, "%s,%s,%s,%s,%d,%s,%s,%s\n", Info[i].mac, Info[i].type, Info[i].model, Info[i].name, Info[i].flag, Info[i].type_s, Info[i].model_s, Info[i].name_s);

	fclose(fp);

	return 0;
}

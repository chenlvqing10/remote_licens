#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lombo_system_sound.h"

int main(int argc, const char **argv)
{
	if (argc < 3) {
		printf("usage system_sound_test file volume hvolume\n");
		return -1;
	}

	printf("file %s, volume %s\n", argv[1], argv[2]);

	lombo_system_set_volume(atoi(argv[2]));
	lombo_set_master_volume(atoi(argv[3]));
	lombo_system_sound_play(argv[1]);
}

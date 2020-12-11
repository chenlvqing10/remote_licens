/* lombo_sound_test.c
**
** Copyright 2011, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/

#include <lombo_sound_play.h>
#include <lombo_sound_cap.h>
#include <lombo_sound_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc, const char **argv)
{
	const char *file_name;
	int volume;

	if (argc < 2) {
		fprintf(stderr, "input mode[-p or -r] and file name:\n");
		fprintf(stderr, "or input mode[-v 0] or mode[-v 1 value]\n");
		return EXIT_FAILURE;
	}
	
	if (argc < 3) {
		fprintf(stderr, "input file name:\n");
        	return EXIT_FAILURE;
    	}

	file_name = argv[2];	

	if (strcmp(argv[1], "-p") == 0) {
		return lombo_sound_play(file_name);
	} else if (strcmp(argv[1], "-c") == 0) {
		return lombo_sound_cap_start(file_name);
	} else if (strcmp(argv[1], "-v") == 0) {
		if (strcmp(argv[2], "0") == 0) {
			lombo_sound_mixer(LOMBO_SOUND_GET_VOLUME, &volume);
			printf("LOMBO_SOUND_GET_VOLUME:%d\n", volume);
			return 0;
		} else if (strcmp(argv[2], "1") == 0) {
			volume = atoi(argv[3]);
			printf("LOMBO_SOUND_SET_VOLUME:%d\n", volume);
			lombo_sound_mixer(LOMBO_SOUND_SET_VOLUME, &volume);
			return 0;
		}
	}

	fprintf(stderr, "para err\n");
    return -1;
}



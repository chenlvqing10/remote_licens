/*
 * rgb2nir.cpp - code for rotate component.
 *
 * Copyright (C) 2019-2020, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <random>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdio>
#include <vector>
#include <sstream>
#include <algorithm>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#define LOG_TAG "rgb2nir_util"
#include <log/log.h>

using namespace std;
using namespace cv;

#include "rgb2nir_util.hpp"

static bool cmp(std::string const &arg_a, std::string const &arg_b)
{
  return arg_a.size() < arg_b.size() ||
    (arg_a.size() == arg_b.size() && arg_a < arg_b);
}

int getFiles2(const string path, vector<string> &files, char const *key)
{
  int iFileCnt = 0;
  DIR *dirptr = NULL;
  struct dirent *dirp;

  if ((dirptr = opendir(path.c_str())) == NULL) {
    return 0;
  }

  while ((dirp = readdir(dirptr)) != NULL) {
    if (dirp->d_type == DT_REG && strcmp(dirp->d_name, ".") != 0 &&
      strcmp(dirp->d_name, "..") != 0 && strstr(dirp->d_name, key) != 0) {
        char file[256] = {0};
        sprintf(file, "%s/%s", path.c_str(), dirp->d_name);
        stringstream stream;
        stream << file;
        //cout << stream.str() << endl;
        files.push_back(stream.str());
    }
    iFileCnt++;
  }

  sort(files.begin(), files.end());
  sort(files.begin(), files.end(), cmp);
  closedir(dirptr);
  return iFileCnt;
}

long long gettime_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}


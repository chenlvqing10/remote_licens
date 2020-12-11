#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <poll.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "debug.h"
#include "wpa_ctrl.h"
#include "wifi_common.h"
#include "state.h"

static struct wpa_ctrl *ctrl_conn;
static struct wpa_ctrl *monitor_conn;

/* socket pair used to exit from a blocking read */
static int exit_sockets[2];

static const char primary_iface[]	 = "wlan0";
static const char IFACE_DIR[]		 = "/var/run/wpa_supplicant";
static const char SUPP_CONFIG_TEMPLATE[] = "/data/wifi/wpa_supplicant_src.conf";
static const char SUPP_CONFIG_FILE[]	 = "/data/wifi/wpa_supplicant.conf";

static const char IFNAME[]		 = "IFNAME=";
#define IFNAMELEN			(sizeof(IFNAME) - 1)
static const char WPA_EVENT_IGNORE[]	 = "CTRL-EVENT-IGNORE ";
static sem_t sem_exit;

static int lw_system(const char *cmd)
{
	pid_t pid;
	int status;

	if (cmd == NULL)
		return 1;

	if ((pid = vfork()) < 0) {
		loge("vfork failed for %s", cmd);
		status = -1;
	} else if (pid == 0) {
		execl("/bin/sh", "sh", "-c", cmd, (char *)0);
		loge("execl failed for %s", cmd);
		_exit(127);
	} else {
		while (waitpid(pid, &status, 0) < 0) {
			if (errno != EINTR) {
				status = -1;
				break;
			}
		}
	}

	return status;
}

int ensure_config_file_exists(const char *config_file)
{
	char buf[2048];
	int srcfd, destfd;
	int nread;
	int ret;

	ret = access(config_file, R_OK|W_OK);
	if ((ret == 0) || (errno == EACCES)) {
		if ((ret != 0) &&
		 (chmod(config_file, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) != 0)) {
			loge("Cannot set RW to \"%s\": %s\n",
				 config_file, strerror(errno));
			return -1;
		}
		return 0;
	} else if (errno != ENOENT) {
		loge("Cannot access \"%s\": %s\n",
			 config_file, strerror(errno));
		return -1;
		}

	srcfd = TEMP_FAILURE_RETRY(open(SUPP_CONFIG_TEMPLATE, O_RDONLY));
	if (srcfd < 0) {
		loge("Cannot open \"%s\": %s\n",
		 SUPP_CONFIG_TEMPLATE, strerror(errno));
		return -1;
	}

	destfd = TEMP_FAILURE_RETRY(open(config_file, O_CREAT|O_RDWR, 0660));
	if (destfd < 0) {
		close(srcfd);
		loge("Cannot create \"%s\": %s\n",
			 config_file, strerror(errno));
		return -1;
	}

	while ((nread = TEMP_FAILURE_RETRY
		(read(srcfd, buf, sizeof(buf)))) != 0) {
		if (nread < 0) {
			loge("Error reading \"%s\": %s\n",
			 SUPP_CONFIG_TEMPLATE, strerror(errno));
			close(srcfd);
			close(destfd);
			unlink(config_file);
			return -1;
		}
		TEMP_FAILURE_RETRY(write(destfd, buf, nread));
	}

	close(destfd);
	close(srcfd);

	/* chmod is needed because open() didn't set permisions properly */
	if (chmod(config_file, 0660) < 0) {
		loge("Error changing permissions of %s to 0660: %s\n",
		config_file, strerror(errno));
		unlink(config_file);
		return -1;
	}

	return 0;
}

int is_proc_run(const char *process)
{
	FILE *fp = NULL;
	int count = 0, size = 100;
	char buf[size];
	char cmd[150];

	snprintf(cmd, sizeof(cmd),
		"ps -ef | grep %s | grep -v grep | wc -l", process);

	fp = popen(cmd, "r");
	if (fp == NULL) {
		loge("popen err!\n");
		return -1;
	}

	if ((fgets(buf, size, fp)) != NULL)
		count = atoi(buf);

	pclose(fp);

	if (count == 0)
		return -1;
	else
		return 0;
}

int wifi_start_supplicant()
{
	/* Before starting the daemon, make sure its config file exists */
	if (ensure_config_file_exists(SUPP_CONFIG_FILE) < 0) {
		loge("Wi-Fi will not be enabled\n");
		return -1;
	}

	/* Reset sockets used for exiting from hung state */
	exit_sockets[0] = exit_sockets[1] = -1;

	sem_init(&sem_exit, 0, 0);

	/* start wpa_supplicant */
	lw_system("/etc/init.d/wifi_sta start > /dev/null");
	usleep(400000);

	return 0;
}

int wifi_stop_supplicant()
{
	lw_system("/etc/init.d/wifi_sta stop > /dev/null");
	while (1) {
		if (is_proc_run("wpa_supplicant"))
			break;
		usleep(20000);
	}

	sem_destroy(&sem_exit);

	return 0;
}

int wifi_connect_on_socket_path(const char *path)
{
	/* To ensure the unix domain socket is created */
	usleep(10000);

	ctrl_conn = wpa_ctrl_open(path);
	if (ctrl_conn == NULL) {
		loge("Unable to open connection to supplicant on \"%s\": %s\n",
		 path, strerror(errno));
		return -1;
	}
	monitor_conn = wpa_ctrl_open(path);
	if (monitor_conn == NULL) {
		loge("Unable to open monitor connection!\n");
		 wpa_ctrl_close(ctrl_conn);
		ctrl_conn = NULL;
		return -1;
	}
	if (wpa_ctrl_attach(monitor_conn) != 0) {
		loge("Unable to attach monitor connection!\n");
		wpa_ctrl_close(monitor_conn);
		wpa_ctrl_close(ctrl_conn);
		ctrl_conn = monitor_conn = NULL;
		return -1;
	}

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, exit_sockets) == -1) {
		loge("create socketpair failed!\n");
		wpa_ctrl_close(monitor_conn);
		wpa_ctrl_close(ctrl_conn);
		ctrl_conn = monitor_conn = NULL;
		return -1;
	}

	return 0;
}

/* Establishes the control and monitor socket connections on the interface */
int wifi_connect_to_supplicant()
{
	static char path[PATH_MAX];
	int count = 0;

	while (count < 400) {
		if (access(IFACE_DIR, F_OK) == 0) {
			snprintf(path, sizeof(path), "%s/%s",
			 IFACE_DIR, primary_iface);
			break;
		}

		logi("wait for create socket!\n");
		count++;
		usleep(10000);
	}

	if (count == 400)
		return -1;

	return wifi_connect_on_socket_path(path);
}

int wifi_send_command(const char *cmd, char *reply, size_t *reply_len)
{
	int ret;
	if (ctrl_conn == NULL) {
		loge("Not connected to"
			"wpa_supplicant - \"%s\" command dropped.\n", cmd);
		return -1;
	}

	set_send_command_flag(ON);
	ret = wpa_ctrl_request(ctrl_conn, cmd, strlen(cmd),
				 reply, reply_len, NULL);
	set_send_command_flag(OFF);
	if (ret == -2) {
		loge("'%s' command timed out.\n", cmd);
		/* unblocks the monitor receive socket for termination */
		TEMP_FAILURE_RETRY(write(exit_sockets[0], "T", 1));
		return -2;
	} else if (ret < 0 || strncmp(reply, "FAIL", 4) == 0)
		return -1;

	if (strncmp(cmd, "PING", 4) == 0)
		reply[*reply_len] = '\0';

	return 0;
}

int wifi_ctrl_recv(char *reply, size_t *reply_len)
{
	int res;
	int ctrlfd = wpa_ctrl_get_fd(monitor_conn);
	struct pollfd rfds[2];

	memset(rfds, 0, 2 * sizeof(struct pollfd));
	rfds[0].fd = ctrlfd;
	rfds[0].events |= POLLIN;
	rfds[1].fd = exit_sockets[1];
	rfds[1].events |= POLLIN;
	res = TEMP_FAILURE_RETRY(poll(rfds, 2, -1));
	if (res < 0) {
		loge("Error poll = %d\n", res);
		return res;
	}

	if (rfds[0].revents & POLLIN)
		return wpa_ctrl_recv(monitor_conn, reply, reply_len);

	/* it is not rfds[0], then it must be rfts[1] (i.e. the exit socket)
	 * or we timed out. In either case, this call has failed ..
	 */

	sem_post(&sem_exit);
	return -2;
}

int wifi_wait_on_socket(char *buf, size_t buflen)
{
	size_t nread = buflen - 1;
	int result;
	char *match, *match2;

	if (monitor_conn == NULL) {
		return snprintf(buf, buflen, "IFNAME=%s %s- connection closed",
			 primary_iface, WPA_EVENT_TERMINATING);
	}

	result = wifi_ctrl_recv(buf, &nread);

	/* Terminate reception on exit socket */
	if (result == -2) {
		return snprintf(buf, buflen, "IFNAME=%s %s- connection closed",
			 primary_iface, WPA_EVENT_TERMINATING);
	}

	if (result < 0) {
		loge("wifi_ctrl_recv failed: %s\n", strerror(errno));
		return snprintf(buf, buflen, "IFNAME=%s %s- recv error",
				 primary_iface, WPA_EVENT_TERMINATING);
	}
	buf[nread] = '\0';
	/* Check for EOF on the socket */
	if (result == 0 && nread == 0) {
		/* Fabricate an event to pass up */
		logw("Received EOF on supplicant socket\n");
		return snprintf(buf, buflen, "IFNAME=%s %s- signal 0 received",
				 primary_iface, WPA_EVENT_TERMINATING);
	}
	/*
	 * Events strings are in the format
	 *
	 *     IFNAME=iface <N>CTRL-EVENT-XXX
	 *        or
	 *     <N>CTRL-EVENT-XXX
	 *
	 * where N is the message level in numerical form (0=VERBOSE, 1=DEBUG,
	 * etc.) and XXX is the event name. The level information is not useful
	 * to us, so strip it off.
	 */

	if (strncmp(buf, IFNAME, IFNAMELEN) == 0) {
		match = strchr(buf, ' ');
		if (match != NULL) {
			if (match[1] == '<') {
				match2 = strchr(match + 2, '>');
				if (match2 != NULL) {
					nread -= (match2 - match);
					memmove(match + 1, match2 + 1,
					 nread - (match - buf) + 1);
				}
			}
		} else {
			return snprintf(buf, buflen, "%s", WPA_EVENT_IGNORE);
		}
	} else if (buf[0] == '<') {
		match = strchr(buf, '>');
		if (match != NULL) {
			nread -= (match + 1 - buf);
			memmove(buf, match + 1, nread + 1);
			/* logd("supplicant generated event without interface
			 - %s\n", buf); */
		}
	} else {
		/* let the event go as is! */
		/* logd("supplicant generated event without interfacE
			 and without message level - %s\n", buf); */
	}

	return nread;
}

int wifi_wait_for_event(char *buf, size_t buflen)
{
	return wifi_wait_on_socket(buf, buflen);
}

void wifi_close_sockets()
{
	if (exit_sockets[0] >= 0) {
		close(exit_sockets[0]);
		exit_sockets[0] = -1;
	}

	if (exit_sockets[1] >= 0) {
		close(exit_sockets[1]);
		exit_sockets[1] = -1;
	}

	sem_wait(&sem_exit);

	if (monitor_conn != NULL) {
#if 0
		wpa_ctrl_detach(monitor_conn);
#endif
		wpa_ctrl_close(monitor_conn);
		monitor_conn = NULL;
	}

	if (ctrl_conn != NULL) {
		wpa_ctrl_close(ctrl_conn);
		ctrl_conn = NULL;
	}
}

void wifi_close_supplicant_connection()
{
	/* wait at most 1 second to ensure init has stopped wpa_supplicant */
#if 0
	int count = 10;

	wifi_close_sockets();

	while (count-- > 0)
		usleep(100000);
#endif
	wifi_close_sockets();

}

int wifi_command(char const *cmd, char *reply, size_t reply_len)
{
	if (!cmd || !cmd[0])
		return -1;

	logd("send command: %s, thread_id: %lu\n", cmd, pthread_self());

	/* Ensure we have room to add NUL termination */
	reply_len--;
	if (wifi_send_command(cmd, reply, &reply_len) != 0)
		return -1;

	/* Strip off trailing newline */
	if ((reply_len > 0) && (reply[reply_len-1] == '\n'))
		reply[reply_len-1] = '\0';
	else
		reply[reply_len] = '\0';
	return 0;
}

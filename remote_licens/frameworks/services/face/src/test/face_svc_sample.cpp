#include <vector>
#include <string>
#include <algorithm>


#include <dirent.h>


using namespace std;
typedef vector<string> samples_t;

samples_t nirs;
samples_t rgbs;


#ifdef __cplusplus
extern "C" {
#endif 

const char  *face_svc_get_nir(int i)
{
	const char *p = NULL;
	if (i < nirs.size())
	{
		p = nirs[i].c_str();
	}
	return p;
}

const char  *face_svc_get_rgb(int i)
{
	const char *p = NULL;
	printf("face_svc_get_rgb i=%d sz=%d\n", i, rgbs.size());
	if (i < rgbs.size())
	{
		p = rgbs[i].c_str();
	}
	return p;
}

int check_head(string n, const char *p)
{
	int ret = -1;
	string head(n.begin(), n.begin()+3);
	if (head.find(p) != head.npos)
	{
		ret = 0;
	}
	return ret;
}

int check_tail(string n, const char *p)
{
	int ret = -1;
	string tail(n.begin() + n.size() - 4, n.end());
	if (tail.find(p) != tail.npos)
	{
		ret = 0;
	}
	return ret;	
}

void face_svc_set_path(const char *path)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;

	dir = opendir(path);
	if (dir == NULL) {
		printf("open dir err\n");
		return ;
	}

	while ((entry = readdir(dir))) {
		printf("%s d_type=%d\n", entry->d_name, entry->d_type);
		if (entry->d_type == DT_REG) {
			char full[512] = {0};
			snprintf(full, 512, "%s%s%s",
				path, "/", entry->d_name);
			string n = entry->d_name;

			if (check_head(n, "rgb") == 0  && (check_tail(n, "mp4") == 0))
			{
				rgbs.push_back(full);
			} 
			else if (check_head(n, "nir") == 0  && (check_tail(n, "mp4") == 0))
			{
				nirs.push_back(full);
			}
		}
	}
	std::sort(nirs.begin(), nirs.end());
	std::sort(rgbs.begin(), rgbs.end());
	closedir(dir);

	{
		int i = 0;
		const char *nir;
		const char *rgb;
		do {

			nir = face_svc_get_nir(i);
			rgb = face_svc_get_rgb(i);
			if ((NULL != nir) && (NULL != rgb))
			{
				//file_cnt++;
				printf("rgb=%s nir=%s\n", rgb, nir);
			}
			i++;
		}while((NULL != nir) && (NULL != rgb));
	}
}

#ifdef __cplusplus
};
#endif


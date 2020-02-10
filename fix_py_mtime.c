#pragma comment(copyright, " © Copyright Rocket Software, Inc. 2016, 2018 ")

/*
/bin/xlc -qlanglvl=extc99 -o fix_py_mtime fix_py_mtime.c
 */

#define _OPEN_SYS_FILE_EXT 1
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <utime.h>
#include <time.h>

static char *pyver = "36";

static int debug = 0;
static int verbose = 0;
static int test = 0;

static
void ct(const time_t *timep, char *buf, size_t max)
{
  memset(buf, 0, max);
  struct tm the_tm;
  strftime(buf, max, "%c", localtime_r(timep, &the_tm));
}

static 
void check_file(char *file)
{
  int result = 0;
  struct stat py_st;
  errno = 0;
  if (-1 == lstat(file, &py_st)) {
    fprintf(stderr, "lstat of %s returned %d (%s)\n", file, errno, strerror(errno));
    return;
  }
  if (S_ISDIR(py_st.st_mode)) {
    if (debug)
      fprintf(stderr, "checking directory %s\n", file);
    DIR *dir = opendir(file);
    char fullname[1024];
    if (dir == NULL) {
      fprintf(stderr, "Unable to upen the directory %s, errno=%d (%s)\n", file, errno, strerror(errno));
      return;
    }
    struct dirent *ent; 
    while (0 != (ent = readdir(dir))) {
      if (0!=strcmp(ent->d_name,".") && 0!=strcmp(ent->d_name,"..")) {
        snprintf(fullname, sizeof(fullname), "%s/%s", file, ent->d_name);
	check_file(fullname);
      }
    }    
    closedir(dir);
    return;
  }
  if (!S_ISREG(py_st.st_mode)) {
    return;
  }
  int filename_len = strlen(file);
  if (filename_len<=3 || 0!=strcmp(file+filename_len-3, ".py")) {
    return;
  }
  if (debug)
    fprintf(stderr, "checking file %s\n", file);
  char *last_slash = strrchr(file, '/');
  char directory[1024];
  snprintf(directory, sizeof(directory)-1, "%.*s", last_slash ? (last_slash-file) : 1, last_slash? file : ".");
  directory[1023]=0;
  char *name = last_slash ? last_slash+1 : file;
  char pyc_name[1024];
  snprintf(pyc_name, sizeof(pyc_name), "%s/__pycache__/%.*s.cpython-%s.pyc",
	   directory,
	   strlen(name)-3, name,
	   pyver);
  int py_mtime = py_st.st_mtime;
  struct stat pyc_st;
  if (-1 == lstat(pyc_name, &pyc_st)) {
    if (debug)
      fprintf(stderr, "lstat of %s returned %d (%s)\n", pyc_name, errno, strerror(errno));
    return;
  }
  char ctime_py[30] = {0};
  char ctime_pyc[30] = {0};
  int pyc_in = open(pyc_name, O_RDONLY, 0);
  if (pyc_in == -1) {
    if (debug)
      fprintf(stderr, "Failed to open %s, %s\n", pyc_name, strerror(errno));
    return;
  } else {
    if (debug)
      fprintf(stderr, "checking %s\n", pyc_name);
    char data[8];
    int num = read(pyc_in, data, sizeof(data));
    close(pyc_in);
    if (num < 8) {
      if (debug)
	fprintf(stderr, "Fewer than 8 characters in %s\n", pyc_name);
    } else {
      py_mtime = (data[4]<<0) + (data[5]<<8) + (data[6]<<16) + (data[7]<<24);
      if (verbose || debug){
        ct(&py_st.st_atime, ctime_py, sizeof(ctime_py));
        ct(&py_mtime, ctime_pyc, sizeof(ctime_pyc));
      }
      if (verbose && py_st.st_mtime != py_mtime) {
	fprintf(stderr, "%s mtime=%d (%s) => mtime=%d (%s)\n",
		file, py_st.st_mtime, ctime_py, py_mtime, ctime_pyc);
      }
      if (test)
	py_mtime = py_st.st_mtime;
    }
    if (debug) {
      fprintf(stderr, "restoring the atime and mtime of %s\n", pyc_name);
    }
    struct utimbuf pyc_ut;
    pyc_ut.actime = pyc_st.st_atime;
    pyc_ut.modtime = pyc_st.st_mtime;
    utime(pyc_name, &pyc_ut);
  }
  if (debug) {
    fprintf(stderr, "Changing the atime of %s to %s", file, ctime(&py_st.st_atime));
    fprintf(stderr, "Changing the mtime to %s to %s\n", file, ctime_pyc);
  }
  struct utimbuf py_ut;
  py_ut.actime = py_st.st_atime;
  py_ut.modtime = py_mtime;
  utime(file, &py_ut);
}

int main(int argc, char **argv)
{
  setenv("_BPXK_AUTOCVT", "OFF", 1);
  int process_args = 1;
  for (int i=1; i<argc; i++) {
    char *arg = argv[i];
    if (!process_args)
      check_file(arg);
    else if (0 == strcmp(arg, "--"))
      process_args = 0;
    else if (0 == strcmp(arg, "-d") || 0 == strcmp(arg, "--debug"))
      debug = 1;
    else if (0 == strcmp(arg, "-v") || 0 == strcmp(arg, "--verbose"))
      verbose = 1;
    else if (0 == strcmp(arg, "-t") || 0 == strcmp(arg, "--test"))
      test = 1;
    else if (0 == strcmp(arg, "-p") || 0 == strcmp(arg, "--pyver"))
      pyver = argv[++i];
    else if (arg[0] == '-')
      fprintf(stderr, "Usage: fix_py_mtime options py_file_or_directory\n"
	      "  Always recurses into the directories specified on the command line\n"
	      "option:\n"
              " -p --pyver    python version of pyc file to use, defaults to 36\n"
	      " -v --verbose  show the files that are changed (or would have been changed, if --test is also specified)\n"
	      " -t --test     do not actually make any changes to the mtime of the .py files\n"
	      " --            stop processing options\n"
	      " -d --debug    show some debugging information\n");
    else
      check_file(arg);
  }
  return 0;
}

/*
© 2016-2018 Rocket Software, Inc. or its affiliates. All Rights Reserved.
ROCKET SOFTWARE, INC.
*/


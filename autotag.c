#pragma comment(copyright, " © Copyright Rocket Software, Inc. 2016, 2018 ")

/*
pushd $BASE/dist/bin
/bin/xlc -qlanglvl=extc99 -o autotag $PORTS/src/autotag.c
rm autotag.o
popd
 */

#define _OPEN_SYS_FILE_EXT 1
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <_Ccsid.h>
#include <utime.h>

struct char_map {
  unsigned char min;
  unsigned char max;
  unsigned char code;
} IBM1047_to_ISO8859_1_map[] = {
  {0x00,0x03,0x00},
  {0x04,0x04,0x9C},
  {0x05,0x05,0x09},
  {0x06,0x06,0x86},
  {0x07,0x07,0x7F},
  {0x08,0x08,0x97},
  {0x09,0x0A,0x8D},
  {0x0B,0x13,0x0B},
  {0x14,0x14,0x9D},
  {0x15,0x15,0x85},
  {0x16,0x16,0x08},
  {0x17,0x17,0x87},
  {0x18,0x19,0x18},
  {0x1A,0x1A,0x92},
  {0x1B,0x1B,0x8F},
  {0x1C,0x1F,0x1C},
  {0x20,0x24,0x80},
  {0x25,0x25,0x0A},
  {0x26,0x26,0x17},
  {0x27,0x27,0x1B},
  {0x28,0x2C,0x88},
  {0x2D,0x2F,0x05},
  {0x30,0x31,0x90},
  {0x32,0x32,0x16},
  {0x33,0x36,0x93},
  {0x37,0x37,0x04},
  {0x38,0x3B,0x98},
  {0x3C,0x3D,0x14},
  {0x3E,0x3E,0x9E},
  {0x3F,0x3F,0x1A},
  {0x40,0x40,0x20},
  {0x41,0x41,0xA0},
  {0x42,0x42,0xE2},
  {0x43,0x43,0xE4},
  {0x44,0x45,0xE0},
  {0x46,0x46,0xE3},
  {0x47,0x47,0xE5},
  {0x48,0x48,0xE7},
  {0x49,0x49,0xF1},
  {0x4A,0x4A,0xA2},
  {0x4B,0x4B,0x2E},
  {0x4C,0x4C,0x3C},
  {0x4D,0x4D,0x28},
  {0x4E,0x4E,0x2B},
  {0x4F,0x4F,0x7C},
  {0x50,0x50,0x26},
  {0x51,0x53,0xE9},
  {0x54,0x54,0xE8},
  {0x55,0x57,0xED},
  {0x58,0x58,0xEC},
  {0x59,0x59,0xDF},
  {0x5A,0x5A,0x21},
  {0x5B,0x5B,0x24},
  {0x5C,0x5C,0x2A},
  {0x5D,0x5D,0x29},
  {0x5E,0x5E,0x3B},
  {0x5F,0x5F,0x5E},
  {0x60,0x60,0x2D},
  {0x61,0x61,0x2F},
  {0x62,0x62,0xC2},
  {0x63,0x63,0xC4},
  {0x64,0x65,0xC0},
  {0x66,0x66,0xC3},
  {0x67,0x67,0xC5},
  {0x68,0x68,0xC7},
  {0x69,0x69,0xD1},
  {0x6A,0x6A,0xA6},
  {0x6B,0x6B,0x2C},
  {0x6C,0x6C,0x25},
  {0x6D,0x6D,0x5F},
  {0x6E,0x6F,0x3E},
  {0x70,0x70,0xF8},
  {0x71,0x73,0xC9},
  {0x74,0x74,0xC8},
  {0x75,0x77,0xCD},
  {0x78,0x78,0xCC},
  {0x79,0x79,0x60},
  {0x7A,0x7A,0x3A},
  {0x7B,0x7B,0x23},
  {0x7C,0x7C,0x40},
  {0x7D,0x7D,0x27},
  {0x7E,0x7E,0x3D},
  {0x7F,0x7F,0x22},
  {0x80,0x80,0xD8},
  {0x81,0x89,0x61},
  {0x8A,0x8A,0xAB},
  {0x8B,0x8B,0xBB},
  {0x8C,0x8C,0xF0},
  {0x8D,0x8E,0xFD},
  {0x8F,0x8F,0xB1},
  {0x90,0x90,0xB0},
  {0x91,0x99,0x6A},
  {0x9A,0x9A,0xAA},
  {0x9B,0x9B,0xBA},
  {0x9C,0x9C,0xE6},
  {0x9D,0x9D,0xB8},
  {0x9E,0x9E,0xC6},
  {0x9F,0x9F,0xA4},
  {0xA0,0xA0,0xB5},
  {0xA1,0xA1,0x7E},
  {0xA2,0xA9,0x73},
  {0xAA,0xAA,0xA1},
  {0xAB,0xAB,0xBF},
  {0xAC,0xAC,0xD0},
  {0xAD,0xAD,0x5B},
  {0xAE,0xAE,0xDE},
  {0xAF,0xAF,0xAE},
  {0xB0,0xB0,0xAC},
  {0xB1,0xB1,0xA3},
  {0xB2,0xB2,0xA5},
  {0xB3,0xB3,0xB7},
  {0xB4,0xB4,0xA9},
  {0xB5,0xB5,0xA7},
  {0xB6,0xB6,0xB6},
  {0xB7,0xB9,0xBC},
  {0xBA,0xBA,0xDD},
  {0xBB,0xBB,0xA8},
  {0xBC,0xBC,0xAF},
  {0xBD,0xBD,0x5D},
  {0xBE,0xBE,0xB4},
  {0xBF,0xBF,0xD7},
  {0xC0,0xC0,0x7B},
  {0xC1,0xC9,0x41},
  {0xCA,0xCA,0xAD},
  {0xCB,0xCB,0xF4},
  {0xCC,0xCC,0xF6},
  {0xCD,0xCE,0xF2},
  {0xCF,0xCF,0xF5},
  {0xD0,0xD0,0x7D},
  {0xD1,0xD9,0x4A},
  {0xDA,0xDA,0xB9},
  {0xDB,0xDC,0xFB},
  {0xDD,0xDE,0xF9},
  {0xDF,0xDF,0xFF},
  {0xE0,0xE0,0x5C},
  {0xE1,0xE1,0xF7},
  {0xE2,0xE9,0x53},
  {0xEA,0xEA,0xB2},
  {0xEB,0xEB,0xD4},
  {0xEC,0xEC,0xD6},
  {0xED,0xEE,0xD2},
  {0xEF,0xEF,0xD5},
  {0xF0,0xF9,0x30},
  {0xFA,0xFA,0xB3},
  {0xFB,0xFC,0xDB},
  {0xFD,0xFE,0xD9},
  {0xFF,0xFF,0x9F},
};

unsigned char IBM1047_to_ISO8859_1[256];

void initialize_IBM1047_to_ISO8859_1(void)
{
  int i=-1;
  do {
    i++;
    for (int in=IBM1047_to_ISO8859_1_map[i].min, out=IBM1047_to_ISO8859_1_map[i].code; 
         in<=IBM1047_to_ISO8859_1_map[i].max; 
         in++, out++) {
      IBM1047_to_ISO8859_1[in] = out;
    }
  } while (0xFF != IBM1047_to_ISO8859_1_map[i].max);
}

/* 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x0085: common cc characters */
/* 0x0000-0x001F: unprintable */
/* 0x0020-0x007F: ASCII printable */
/* 0x00A0: unprintable */
/* 0x00A1-0x00FF: non-ASCII printable */
#define classify(ch) \
  (ch==0x09||ch==0x0A||ch==0x0B||ch==0x0C||ch==0x0D||ch==20||ch==0x85)?1: \
  (ch<=0x1F||ch==0xA0)?0: \
  (ch<=0x7F)?2: \
  3

int IBM1047_ccsid;
int ISO88591_ccsid;

int debug = 0;
int debug_ch = 0;
int debug_char_error = 0;
int debug_limit = 10;
int verbose = 0;
int test = 0;
int bad_char_limit = 0;
int recursive = 0;
char *default_charset = NULL;
int skip_tagged_files = 0;

int debug_count = 0;
int bad_char_count = 0;

#define SF_ERROR     0
#define SF_EMPTY     1
#define SF_BOTH      2
#define SF_NEITHER   3
#define SF_ISO8859_1 4
#define SF_IBM_1047  5

char *sf_names[] = {"ERROR", "EMPTY", "BOTH", "NEITHER", "ISO8859-1", "IBM-1047"};

int counts[2][4];

void clear_counts(void)
{
  for (int i=0; i<2; i++) {
    for (int j=0; j<4; j++) {
      counts[i][j]=0;
    }
  }
}

void show_counts(void)
{
  for (int e=0; e<2; e++) {
    printf("  %s: bad=%d whitespace=%d english=%d international=%d\n", (e == 0) ? "ISO8559-1" : "IBM-1047",
           counts[e][0], counts[e][1], counts[e][2], counts[e][3]);
  }
}

int scan_file(char *filename)
{
  debug_count = 0;
  bad_char_count = 0;
  int showed_filename = 0;
  FILE *in = fopen(filename, "rb"); /* the mode never affects the operation of _BPXK_AUTOCVT=ON */
  if (in==NULL) return SF_ERROR;
  int ch, ch_trans;
  clear_counts();
  int position = 0;
  while (0 <= (ch = getc(in))) {
    int iso8859_class = classify(ch);
    counts[0][iso8859_class]++;
    if (debug_char_error && iso8859_class == 0 && counts[0][iso8859_class] < 10 && debug_count++ < debug_limit) {
      if (!showed_filename) {printf("%s\n", filename); showed_filename=1;}
      printf("ISO8859-1 char error at position=%d, char=0x%02X\n", position, ch);
    }
    ch_trans = IBM1047_to_ISO8859_1[ch];
    int ibm1047_class = classify(ch_trans);
    counts[1][ibm1047_class]++;
    if (debug_char_error && ibm1047_class == 0 && counts[1][ibm1047_class] < 10 && debug_count++ < debug_limit) {
      if (!showed_filename) {printf("%s\n", filename); showed_filename=1;}
      printf("IBM-1047 char error at position=%d, char=0x%02X\n", position, ch);
    }
    position++;
  }
  if (debug) {
    show_counts();
  }
  fclose(in);
  if (0 == (counts[0][0] + counts[0][1] + counts[0][2] + counts[0][3])) {
    return SF_EMPTY;
  } else if (counts[0][0] <= bad_char_limit && counts[1][0] <= bad_char_limit) {
    if ((counts[0][0]==0 && counts[0][3]==0) && !((counts[1][0]==0 && counts[1][3]==0))) {
      return SF_ISO8859_1;
    } else if (!(counts[0][0]==0 && counts[0][3]==0) && ((counts[1][0]==0 && counts[1][3]==0))) {
      return SF_IBM_1047;
    } else if (counts[0][3] <= bad_char_limit && counts[1][3] <= bad_char_limit) {
      return SF_BOTH;
    } else if (counts[0][3] <= bad_char_limit) {
      return SF_ISO8859_1;
    } else if (counts[1][3] <= bad_char_limit) {
      return SF_IBM_1047;
    } else {
      return SF_BOTH;
    }
  } else if (counts[0][0] <= bad_char_limit) {
    bad_char_count = counts[0][0];
    return SF_ISO8859_1;
  } else if (counts[1][0] <= bad_char_limit) {
    bad_char_count = counts[0][0];
    return SF_IBM_1047;
  } else {
    return SF_NEITHER;
  }
}

#define decode_ccsid(ccsid, buffer) \
    (ccsid == FT_BINARY) ? "BINARY" : \
    (ccsid == FT_UNTAGGED) ? "UNTAGGED" : \
    (ccsid == IBM1047_ccsid) ? "IBM-1047" : \
    (ccsid == ISO88591_ccsid) ? "ISO8859-1" : \
    (__toCSName(ccsid, buffer), buffer)

#define decode_tag(tag, buffer) \
    (ccsid == FT_BINARY) ? "BINARY" : \
    (ccsid == FT_UNTAGGED) ? "UNTAGGED" : \
    (ccsid == IBM1047_ccsid) ? "IBM-1047" : \
    (ccsid == ISO88591_ccsid) ? "ISO8859-1" : \
    (__toCSName(ccsid, buffer), buffer)

int check_file(char *file)
{
  int result = 0;
  struct stat st;
  errno = 0;
  if (-1 == lstat(file, &st)) {
    printf("stat of %s returned %d (%s)\n", file, errno, strerror(errno));
    return -1;
  }
  if (S_ISDIR(st.st_mode)) {
    if (!recursive) return 0;
    DIR *dir = opendir(file);
    char fullname[1024];
    if (dir == NULL) {
      printf("Unable to upen the directory %s, errno=%d (%s)\n", file, errno, strerror(errno));
      return -1;
    }
    struct dirent *ent; 
    while (0 != (ent = readdir(dir))) {
      if (0!=strcmp(ent->d_name,".") && 0!=strcmp(ent->d_name,"..")) {
        snprintf(fullname, sizeof(fullname), "%s/%s", file, ent->d_name);
        int result1 = check_file(fullname);
        if (result == 0)
          result = result1;
      }
    }    
    closedir(dir);
    return result;
  }
  if (!S_ISREG(st.st_mode)) {
    return 0;
  }
  if (skip_tagged_files && (st.st_tag.ft_txtflag || st.st_tag.ft_ccsid == FT_BINARY)) {
    return 0;
  }
  int filelen = strlen(file);
  int saved_debug = debug;
  if (debug_ch && filelen>2 && (0 == strcmp(file+filelen-2, ".c") || 0 == strcmp(file+filelen-2, ".h")))
    debug = 1;
  char csname_buffer1[24]; strcpy(csname_buffer1, "?");
  char *csname = decode_ccsid(st.st_tag.ft_ccsid, csname_buffer1);
  if (debug) {
    printf("name=%s old txtflag=%d, old ccsid=%s\n", file, st.st_tag.ft_txtflag, csname);
  }
  int type = scan_file(file);
  char *type_name = sf_names[type];
  if (debug) {
    printf("  type=%s\n", type_name);
  }
  if (type == SF_ERROR) {
    result = -1;
  } else if (type == SF_EMPTY) {
    result = 0;
  } else if (type == SF_BOTH && default_charset == NULL) {
    printf("unable to classify %s\n", file);
    result = 0;
  } else if (type == SF_NEITHER && !st.st_tag.ft_txtflag && st.st_tag.ft_ccsid == FT_BINARY) {
    result = 0;
  } else if (type == SF_ISO8859_1 && st.st_tag.ft_txtflag && st.st_tag.ft_ccsid == ISO88591_ccsid) {
    result = 0;
  } else if (type == SF_IBM_1047 && st.st_tag.ft_txtflag && st.st_tag.ft_ccsid == IBM1047_ccsid) {
    result = 0;
  } else {
    if (type == SF_BOTH) {
      type = 
        (default_charset == NULL) ? -1 :
        (0 == strcmp(default_charset, "ISO8859-1")) ? SF_ISO8859_1 :
        (0 == strcmp(default_charset, "IBM-1047")) ? SF_IBM_1047 :
        -1;
      if (type == -1) {
        printf("unable to classify %s\n", file);
        result = 0;
      }      
    }
    if (type > SF_EMPTY) {
      if (debug) {
        printf("  there is a problem%s\n", !test ? ", and it will be fixed" : ""); /* all the info is already printed */
      } else if (verbose) {
        printf("name=%s old txtflag=%d, old ccsid=%s type=%s\n", file, st.st_tag.ft_txtflag, csname, type_name);
        show_counts();
      }
      if (!test) {
        attrib_t attrs;
        memset(&attrs, 0, sizeof(attrs));
        attrs.att_filetagchg = 1;
        attrs.att_filetag.ft_txtflag = (type != SF_BOTH) && (type != SF_NEITHER);
        attrs.att_filetag.ft_ccsid = 
          (!attrs.att_filetag.ft_txtflag) ? FT_BINARY :
          (type == SF_ISO8859_1) ? ISO88591_ccsid :
          (type == SF_IBM_1047) ? IBM1047_ccsid :
          0; /* not reached */
        char csname_buffer2[24]; strcpy(csname_buffer2, "?");
        char *csname2 = decode_ccsid(attrs.att_filetag.ft_ccsid, csname_buffer2);
        errno = 0;
        int r = __chattr(file, &attrs, sizeof(attrs));
        if (r) {
          int saved_errno = errno;
          int saved_strerror = strerror(errno);
          if (!debug && !verbose) {
            printf("%s\n"
                   "  old txtflag=%d, old ccsid=%d, old csname=%s\n",
                   file, 
                   st.st_tag.ft_txtflag, st.st_tag.ft_ccsid, csname);
            show_counts();
            printf("  %s\n", type_name);
          }
          printf("  new txtflag=%d, new ccsid=%d, new csname=%s\n", 
                 attrs.att_filetag.ft_txtflag, attrs.att_filetag.ft_ccsid, 
                 csname2);
          printf("  __chattr: ret=%d, errno=%d (%s)\n", r, saved_errno, saved_strerror);
          result = -1;
        } else {
          if (verbose) {
            printf("  changed the tag for %s to %s\n", file, csname2);
          }
          result = 1;
        }
      }
    }
  }
  if (debug) {
    printf("Changing the atime of %s to %s", file, ctime(&st.st_atime));
    printf("Changing the mtime of %s to %s", file, ctime(&st.st_mtime));
  }
  struct utimbuf ut;
  ut.actime = st.st_atime;
  ut.modtime = st.st_mtime;
  utime(file, &ut);
  debug = saved_debug;
  return result;
}

void print_help(void)
{
  printf("Usage: autotag options files\n"
         "  Classify and tag files based on their contents\n"
         "Options\n"
         "   --version\n"
         "-R --recursive\n"
         "-s --skip-tagged-files         (analyze and tag only files without tags)\n"
         "-L --bad-char-limit    limit   (allow up to limit binary characters in text files; the default is 0)\n"
         "-t --test                      (does not make any changes)\n"
         "-f --file              file    (-f is useful if a file argument begins with '-')\n"
         "-v --verbose\n"
         "   --changed                   (return 1 if anything changed, otherwise return 0)\n" 
         "-d --debug\n"
         "-D --default-charset   name    (tag to use if a file cannot be classified (very small); the default is not to tag)\n"
         "   --debug-ch                  (turns on -d debugging for c or h files)\n"
         "-C --debug-char-error\n"
         "   --debug-limit       limit   (if -C is used, this option controls how many char errors are shown; default is 10)\n");
}

void print_version(void)
{
  printf("Autotag 0.5\n");
}
                   
int main(int argc, char **argv)
{
  initialize_IBM1047_to_ISO8859_1();
  setenv("_BPXK_AUTOCVT", "OFF", 1);
  IBM1047_ccsid = __toCcsid("IBM-1047");
  ISO88591_ccsid = __toCcsid("ISO8859-1");
  int result = 0;
  int result1;
  int check_options = 1;
  int changed = 0;
  for (int i=1; i<argc; i++) {
    if (!check_options) {
      result1 = check_file(argv[i]);
      if (result == 0) result = result1;
    } else if (0==strcmp(argv[i], "--"))
      check_options = 0;
    else if (0==strcmp(argv[i], "-d") || 0==strcmp(argv[i], "--debug"))
      debug = 1;
    else if (0==strcmp(argv[i], "--debug-ch"))
      debug_ch = 1;
    else if (0==strcmp(argv[i], "-C") || 0==strcmp(argv[i], "--debug-char-error"))
      debug_char_error = 1;
    else if (0==strcmp(argv[i], "--debug-limit"))
      debug_limit = strtol(argv[++i], NULL, 10);
    else if (0==strcmp(argv[i], "-v") || 0==strcmp(argv[i], "--verbose"))
      verbose = 1;
    else if (0==strcmp(argv[i], "-f") || 0==strcmp(argv[i], "--file"))
      check_file(argv[++i]);
    else if (0==strcmp(argv[i], "-R") || 0==strcmp(argv[i], "--recursive"))
      recursive = 1;
    else if (0==strcmp(argv[i], "--changed"))
      changed = 1;
    else if (0==strcmp(argv[i], "-t") || 0==strcmp(argv[i], "--test"))
      test = 1;
    else if (0==strcmp(argv[i], "-s") || 0==strcmp(argv[i], "--skip-tagged-files"))
      skip_tagged_files = 1;
    else if (0==strcmp(argv[i], "-D") || 0==strcmp(argv[i], "--default-charset"))
      default_charset = argv[++i];
    else if (0==strcmp(argv[i], "-L") || 0==strcmp(argv[i], "--bad-char-limit"))
      bad_char_limit = strtol(argv[++i], NULL, 10);
    else if (0==strcmp(argv[i], "--version"))
      print_version();
    else if (0==strcmp(argv[i], "-h") || 0==strcmp(argv[i], "--help"))
      print_help();
    else {
      result1 = check_file(argv[i]);
      if (result == 0) result = result1;
    }
  }
  return changed ? result1 : 0;
}

/*
© 2016-2018 Rocket Software, Inc. or its affiliates. All Rights Reserved.
ROCKET SOFTWARE, INC.
*/


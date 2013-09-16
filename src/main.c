#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <dirent.h>

#include "fatlib-0.13/fatlib.h"

static uint8_t bootsector[512] = {
  0372, 0270, 0000, 0020, 0216, 0320, 0274, 0000, 0260, 0270, 0000, 0000, 0216, 0330, 0216, 0300,
  0373, 0276, 0000, 0174, 0277, 0000, 0006, 0271, 0000, 0002, 0363, 0244, 0352, 0041, 0006, 0000,
  0000, 0276, 0276, 0007, 0070, 0004, 0165, 0013, 0203, 0306, 0020, 0201, 0376, 0376, 0007, 0165,
  0363, 0353, 0026, 0264, 0002, 0260, 0001, 0273, 0000, 0174, 0262, 0200, 0212, 0164, 0001, 0213,
  0114, 0002, 0315, 0023, 0352, 0000, 0174, 0000, 0000, 0353, 0376, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0170, 0323, 0003, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0125, 0252
  };


/**
 *
 */
static void
usage(const char *argv0)
{
  fprintf(stderr, "RTFS\n");
  exit(0);
}

static char tmpname[PATH_MAX];

static void
cleanup_tmpfile(void)
{
  printf("Removing %s\n", tmpname);
  unlink(tmpname);
}



/**
 *
 */
int
main(int argc, char **argv)
{
  int opt;
  unsigned int size = 524288;
  const char *sourcedir = NULL;
  int no_remove_tmp_file = 0;
  const char *output = NULL;

  while((opt = getopt(argc, argv, "s:f:no:")) != -1) {

    switch(opt) {
    case 's':
      size = atoi(optarg);
      break;
    case 'f':
      sourcedir = optarg;
      break;
    case 'o':
      output = optarg;
      break;
    case 'n':
      no_remove_tmp_file = 1;
      break;
    }
  }

  if(size < 1024 || !sourcedir || !output)
    usage(argv[0]);


  snprintf(tmpname, sizeof(tmpname), "/tmp/mkimg.XXXXXX");
  int fd = mkstemp(tmpname);
  if(fd == -1) {
    perror("mkstemp");
    exit(1);
  }

  if(!no_remove_tmp_file)
    atexit(cleanup_tmpfile);

  if(ftruncate(fd, size * 512)) {
    perror("ftruncate");
    exit(1);
  }
  close(fd);
  
  char tmpbuf[1024];

  snprintf(tmpbuf, sizeof(tmpbuf), "/sbin/mkfs.vfat -F 16 -v %s", tmpname);
  if(system(tmpbuf)) {
    exit(1);
  }

  printf("\n");
  printf("Mounting file system at %s\n", tmpname);
  

  DRIVE *d = fat_open_image(tmpname, 1);
  if(d == NULL) {
    fprintf(stderr, "Failed to open image\n");
    exit(1);
  }


  printf("Copying files from %s\n", sourcedir);



  struct dirent **namelist;
  int n = scandir(sourcedir, &namelist, NULL, NULL);
  if(n < 0) {
    perror("scandir");
    exit(1);
  }

  while(n--) {
    const char *fn = namelist[n]->d_name;
    if(fn[0] == '.')
      continue;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", sourcedir, fn);


    printf("Copying %-15s ... ", fn);
    fflush(stdout);

    int src = open(path, O_RDONLY);
    if(src == -1) {
      printf("%s\n", strerror(errno));
      exit(1);
    }

    struct stat st;
    if(fstat(src, &st)) {
      printf("stat() failed -- %s\n", strerror(errno));
      exit(1);
    }
    void *mem = malloc(st.st_size);

    if(read(src, mem, st.st_size) != st.st_size) {
      printf("Read failed\n");
      exit(1);
    }
    close(src);

    FILE *dst = fat_fopen(d, fn, "w");
    if(dst == NULL) {
      printf("Unable to open %s for writing", fn);
      exit(1);
    }

    printf("%10d bytes ... ", (int)st.st_size);
    fflush(stdout);

    if(fwrite(mem, st.st_size, 1, dst) != 1) {
      printf("Write failed\n");
      exit(1);
    }

    free(mem);

    printf("written ... ");
    fflush(stdout);

    fclose(dst);
    printf("OK\n");
  }

  fat_drive_close(d);

  unlink(output);

  int fatfd = open(tmpname, O_RDONLY);
  if(fatfd == -1) {
    perror("Uanble to open temp file");
    exit(1);
  }

  int outfd = open(output, O_CREAT | O_WRONLY, 0666);
  if(outfd == -1) {
    perror("Uanble to open output file");
    exit(1);
  }

  unsigned char *x = bootsector + 0x1c0;

  *x++ = 0x01;
  *x++ = 0x40;
  *x++ = 0x0e;
  *x++ = 0x03;
  *x++ = 0xe0;
  *x++ = 0xff;


  // Start sector (8192)

  *x++ = 0x00;
  *x++ = 0x20;
  *x++ = 0x00;
  *x++ = 0x00;

  // Size

  *x++ = size;
  *x++ = size >> 8;
  *x++ = size >> 16;
  *x++ = size >> 24;

  if(write(outfd, bootsector, 512) != 512) {
    perror("Unable to write bootsector to output file");
    goto bad;
  }

  lseek(outfd, 512 * 8192, SEEK_SET);
  
  size_t imgbytes = size * 512;

  void *imgmem = malloc(imgbytes);
  if(read(fatfd, imgmem, imgbytes) != imgbytes) {
    perror("Unable to read image to RAM");
    goto bad;
  }

  if(write(outfd, imgmem, imgbytes) != imgbytes) {
    perror("Unable to write to output file");
    goto bad;
  }

  close(outfd);

  return 0;


 bad:
  unlink(output);
  exit(1);
}

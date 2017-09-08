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
  printf("Usage:  %s [OPTIONS]\n", argv0);
  printf("\nMandatory options:\n\n");
  printf("  -s SIZE         Size of FAT filesystem in sectors (512 bytes)\n");
  printf("  -f PATH         Path to files that should be coped\n");
  printf("  -o PATH         Path to output file (will be created)\n");
  printf("\nOptional options:\n\n");
  printf("  -b PATH         Path to bootloader to install (max 448 bytes)\n");
  printf("  -k PATH         Path to kernel to install\n");
  printf("  -t SECTORS      Image total size in sectors\n");
  printf("  -T GIGABYTES    Image total size in 1024*1024*1024 bytes\n");
  printf("  -n              Keep temporary filesystem image\n");
  printf("\n");
  exit(1);
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
  unsigned int fs_size = 524288;
  const char *sourcedir = NULL;
  int no_remove_tmp_file = 0;
  const char *output = NULL;
  const char *bootloaderpath = NULL;
  const char *kernelpath = NULL;
  int totalsize = 0;

  while((opt = getopt(argc, argv, "s:f:no:b:k:t:T:")) != -1) {

    switch(opt) {
    case 's':
      fs_size = atoi(optarg);
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
    case 'b':
      bootloaderpath = optarg;
      break;
    case 'k':
      kernelpath = optarg;
      break;
    case 't':
      totalsize = atoi(optarg) * 512;
      break;
    case 'T':
      totalsize = atoi(optarg) * 1024LL * 1024LL * 1024LL;
      break;
    }
  }

  if(fs_size < 1024 || !sourcedir || !output)
    usage(argv[0]);


  snprintf(tmpname, sizeof(tmpname), "/tmp/mkimg.XXXXXX");
  int fd = mkstemp(tmpname);
  if(fd == -1) {
    perror("mkstemp");
    exit(1);
  }

  if(!no_remove_tmp_file)
    atexit(cleanup_tmpfile);

  if(ftruncate(fd, fs_size * 512)) {
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


    printf("Copying %-40s ... ", fn);
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

    fflush(stdout);
    fclose(dst);
    printf("OK\n");
  }

  fat_drive_close(d);

  if(bootloaderpath != NULL) {
    const int blfd = open(bootloaderpath, O_RDONLY);
    if(blfd == -1) {
      perror("Unable to open bootloader");
      exit(1);
    }

    int r = read(blfd, bootsector, 0x1c0);
    if(r < 0) {
      perror("Unable to read bootloader");
      exit(1);
    }

    close(blfd);
    printf("Loaded bootloader from %s (%d bytes)\n", bootloaderpath, r);
  }

  void *kernel = NULL;
  size_t kernel_size = 0;
  if(kernelpath != NULL) {
    const int kfd = open(kernelpath, O_RDONLY);
    if(kfd == -1) {
      perror("Unable to open kernel");
      exit(1);
    }

    struct stat st;
    if(fstat(kfd, &st)) {
      perror("Unable to stat kernel");
      exit(1);
    }

    kernel_size = st.st_size;
    kernel = malloc(st.st_size);

    if(read(kfd, kernel, kernel_size) != kernel_size) {
      perror("Unable to read kernel");
      exit(1);
    }
    close(kfd);
  }

  int fatfd = open(tmpname, O_RDONLY);
  if(fatfd == -1) {
    perror("Uanble to open temp file");
    exit(1);
  }

  size_t imgbytes = fs_size * 512;
  void *imgmem = malloc(imgbytes);
  if(read(fatfd, imgmem, imgbytes) != imgbytes) {
    perror("Unable to read image to RAM");
    goto bad;
  }

  unlink(output);

  int outfd = open(output, O_CREAT | O_WRONLY, 0666);
  if(outfd == -1) {
    perror("Uanble to open output file");
    exit(1);
  }

  unsigned int fs_start = 1;


  if(kernel != NULL) {
    int kernel_start = 1;
    int kernel_sectors = (kernel_size + 511) / 512;

    if(pwrite(outfd, kernel, kernel_size, kernel_start * 512) !=
       kernel_size) {
      perror("Uanble to write kernel");
      exit(1);
    }

    printf("Kernel start at sector %d\n", kernel_start);
    printf("Kernel size in sectors: %d\n", kernel_sectors);
    fs_start = kernel_start + kernel_sectors;
  }

  // Round fs_start to 8192 sector alignment (4MB)
  fs_start = (fs_start + 8191) & ~8191;

  printf("Filesystem start at sector %d\n", fs_start);

  unsigned char *x = bootsector + 0x1c0;

  *x++ = 0x01;
  *x++ = 0x40;
  *x++ = 0x0e;
  *x++ = 0x03;
  *x++ = 0xe0;
  *x++ = 0xff;

  // Start sector

  *x++ = fs_start;
  *x++ = fs_start >> 8;
  *x++ = fs_start >> 16;
  *x++ = fs_start >> 24;

  // Size

  *x++ = fs_size;
  *x++ = fs_size >> 8;
  *x++ = fs_size >> 16;
  *x++ = fs_size >> 24;


  if(pwrite(outfd, imgmem, imgbytes, 512 * fs_start) != imgbytes) {
    perror("Unable to write to output file");
    goto bad;
  }

  if(pwrite(outfd, bootsector, 512, 0) != 512) {
    perror("Unable to write bootsector to output file");
    goto bad;
  }

  if(totalsize) {
    if(ftruncate(outfd, totalsize) < 0)
      perror("ftruncate");
  }

  close(outfd);

  return 0;


 bad:
  unlink(output);
  exit(1);
}

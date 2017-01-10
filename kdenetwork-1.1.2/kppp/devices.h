#ifndef _DEVICES_H_
#define _DEVICES_H_

static const char *devices[] = {
#ifdef __FreeBSD__
  "/dev/cuaa0",
  "/dev/cuaa1",
  "/dev/cuaa2",
  "/dev/cuaa3",
#else
  "/dev/modem",
  "/dev/cua0",
  "/dev/cua1",
  "/dev/cua2",
  "/dev/cua3",
  "/dev/ttyS0",
  "/dev/ttyS1",
  "/dev/ttyS2",
  "/dev/ttyS3",

#ifdef ISDNSUPPORT
  "/dev/ttyI0",
  "/dev/ttyI1",
  "/dev/ttyI2",
  "/dev/ttyI3",
#endif
#endif
  0};

// default device number from the list above
const int DEV_DEFAULT = 0;

#endif

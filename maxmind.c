#ifndef MAXMIND_H
#define MAXMIND_H
#include <string.h>
#include <maxminddb.h>
#include "maxmind.h"
#include "byte.h"
#include "error.h"
#include "strerr.h"
#include "exit.h"

#define COUNTRYLEN 2
#define FATAL "maxmind fatal: "
#define WARNING "maxmind warning: "

MMDB_s mmdb;
char country[COUNTRYLEN];

void maxmind_free();

void maxmind_init(char *geoipdb) {
  setlinebuf(stderr);
  fprintf(stderr, "maxmind v%s\n", MMDB_lib_version()); 
  byte_zero(country, COUNTRYLEN);
  int status = MMDB_open(geoipdb, MMDB_MODE_MMAP, &mmdb);
  if (status != MMDB_SUCCESS) {
    strerr_die5x(111, FATAL, "can't open ", geoipdb, " - ", MMDB_strerror(status));

    if (status == MMDB_IO_ERROR) 
        strerr_die3x(111, FATAL, "io error ", strerror(errno));
    _exit(111);
  }
}

char *maxmind_lookup(char *ipstr) {
  int gai_error, mmdb_error, status;
  MMDB_entry_data_s entry;

  byte_zero(country, COUNTRYLEN);
  MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb, ipstr, &gai_error, &mmdb_error);

  if (gai_error != 0) 
    strerr_warn5(WARNING, "error from getaddrinfo for ", ipstr, " - ", gai_strerror(gai_error), 0);

  if (mmdb_error != MMDB_SUCCESS) 
    strerr_warn3(WARNING, "got an error from libmaxminddb: ", MMDB_strerror(mmdb_error), 0);

  if (result.found_entry) {
    status = MMDB_get_value(&result.entry, &entry, "country", "iso_code", NULL);

    if (status != MMDB_SUCCESS) {
        strerr_warn3(WARNING, "got an error looking up the entry data - ", MMDB_strerror(status), 0);
    } else if (entry.has_data) {
      if (entry.type == MMDB_DATA_TYPE_UTF8_STRING) {
        byte_zero(country, COUNTRYLEN);
        byte_copy(country, 2, entry.utf8_string);
        return country;
      }
    }
  } else {
    strerr_warn4(WARNING, "no entry for this ip address ", ipstr, " was found", 0);
  }
  return NULL;
}

void maxmind_free() {
    MMDB_close(&mmdb);
}
#endif

#ifndef MAXMIND_H
#define MAXMIND_H
#include <string.h>
#include <maxminddb.h>
#include "maxmind.h"
#include "byte.h"
#include "error.h"
#include "exit.h"

#define COUNTRYLEN 3

MMDB_s mmdb;
char country[COUNTRYLEN];
int exit_code = 0;

  // TODO:
    // - optimise query.c for for IS$ files

void maxmind_free();

void maxmind_init() {
  setlinebuf(stderr);
  fprintf(stderr, "maxmind v%s\n", MMDB_lib_version()); 
  byte_zero(country, COUNTRYLEN);
  char fname[] = "GeoLite2-Country.mmdb";
  int status = MMDB_open(fname, MMDB_MODE_MMAP, &mmdb);
  if (status != MMDB_SUCCESS) {
    fprintf(stderr, "maxmind: can't open %s - %s\n", fname, MMDB_strerror(status));

    if (status == MMDB_IO_ERROR) {
        fprintf(stderr, "maxmind: IO error: %s\n", strerror(errno));
    }
    exit_code = status;
    maxmind_free();
  }
}

char *maxmind_lookup(char *ipstr) {
  int gai_error, mmdb_error;
  byte_zero(country, COUNTRYLEN);
  MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb, ipstr, &gai_error, &mmdb_error);

  if (gai_error != 0) 
    fprintf(stderr, "maxmind: error from getaddrinfo for %s - %s\n", ipstr, gai_strerror(gai_error));

  if (mmdb_error != MMDB_SUCCESS) 
    fprintf(stderr, "maxmind: got an error from libmaxminddb: %s\n", MMDB_strerror(mmdb_error));

  if (result.found_entry) {
    MMDB_entry_data_s entry;
    int status = MMDB_get_value(&result.entry, &entry, "country", "iso_code", NULL);

    if (status != MMDB_SUCCESS) {
        fprintf( stderr, "maxmind: got an error looking up the entry data - %s\n", MMDB_strerror(status));
        exit_code = 4;
        /* goto end; */
    } else if (entry.has_data) {
      /* printf("data_size: %d\n", entry.data_size); */
      if (entry.type == MMDB_DATA_TYPE_UTF8_STRING) {
        byte_zero(country, COUNTRYLEN);
        byte_copy(country, 2, entry.utf8_string);
        printf("country  %s\n", country); 
      }
    }
  } else {
    fprintf( stderr, "maxmind: no entry for this ip address (%s) was found\n", ipstr);
    exit_code = 5;
  }

  return country;
}

void maxmind_free() {
    MMDB_close(&mmdb);
    _exit(exit_code);
}
#endif

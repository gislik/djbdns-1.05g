#include "dns.h"
#include "tdlookup.h"
#include "strerr.h"

const char *fatal = "tinydns: fatal: ";
const char *starting = "starting tinydns\n";

static char seed[128];

void initialize(void)
{
  dns_random_init(seed);
  if (!tdlookup_init()) strerr_die2sys(111,fatal,"tdlookup_init failed");
}

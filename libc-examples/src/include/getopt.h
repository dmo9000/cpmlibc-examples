extern char *optarg; /* Global argument pointer. */
extern int optind; /* Global argv index. */
int getopt(int argc, char *argv[], char *optstring);
int getopt_long(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex); 


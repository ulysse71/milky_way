#ifndef UTILS_HH
#define UTILS_HH

inline double sqr(double x) { return x*x; }

/// CPU aborts
#define ABORTSLT() { abort(); }
#define ABORT() { printf("abort at file %s line %d\n", __FILE__, __LINE__); fflush(stdout); ABORTSLT(); }
#define ABORTMSG(msg) { printf("abort at file %s line %d [%s]\n", __FILE__, __LINE__, msg); fflush(stdout); ABORTSLT(); }
#define ABORTMSG2(msg, msg2) { printf("abort at file %s line %d [%s %s]\n", __FILE__, __LINE__, msg, msg2); fflush(stdout); ABORTSLT(); }

#define ASSERT(predic) if (!(predic)) { printf("check failed at file %s line %d\n", __FILE__, __LINE__); abort(); }

/// unit tests
#define CHECKPRINT() fprintf(stderr, "# check file %s line %d OK\n", __FILE__, __LINE__)
#define CHECKDOUBLE(ref, dst, eps) ASSERT( fabs((dst)-(ref)) < eps * fabs(ref) ); CHECKPRINT()
#define CHECKSTRING(ref, srf) ASSERT( strcmp(ref, srf) == 0 ); CHECKPRINT()



#endif // UTILS_HH


#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>

#include "cxxutils.hh"

char *nextField(char *str) { return str+strcspn(str, ",")+1; }

namespace dim3 {
  struct T {
    double c[3];
  };

  T zero = { 0, 0, 0};
  T vi = {1, 0, 0}, vj = {0, 1, 0}, vk = {0, 0, 1};

  T add(T x, T y) { return { x.c[0]+y.c[0], x.c[1]+y.c[1], x.c[2]+y.c[2] }; }
  T sub(T x, T y) { return { x.c[0]-y.c[0], x.c[1]-y.c[1], x.c[2]-y.c[2] }; }

  double dot(T x, T y) { return x.c[0]*y.c[0] + x.c[1]*y.c[1] + x.c[2]*y.c[2]; }
  double norm2(T x) { return dot(x, x); }

  double norm(T x) { return sqrt(norm2(x)); }

  T ediv(T x, double l) { return { x.c[0]/l, x.c[1]/l, x.c[2]/l }; }
  T emul(double l, T x) { return { x.c[0]*l, x.c[1]*l, x.c[2]*l }; }

  T normalize(T x) { return ediv(x, norm(x)); }

  T cross(T x, T y) {
    return {
      x.c[1]*y.c[2] - x.c[2]*y.c[1],
      x.c[2]*y.c[0] - x.c[0]*y.c[2],
      x.c[0]*y.c[1] - x.c[1]*y.c[0]
    };
  } 

};

namespace angles {
  struct hr2 { int u; int m; double s; };
  double toReal(hr2 h) { return h.u + h.m/60. + h.s/3600.; }
};

namespace color {
  typedef unsigned char byte;
  typedef struct { byte r, g, b; } T;
  T emul(T col, double mul) {
    return { (byte)(col.r*mul), (byte)(col.g*mul), (byte)(col.b*mul) };
  }
}


namespace spectrum {
  enum SpectrumE {
    SPECTRUM_O,
    SPECTRUM_B,
    SPECTRUM_A,
    SPECTRUM_F,
    SPECTRUM_G,
    SPECTRUM_K,
    SPECTRUM_M,
    SPECTRUM_Unknown
  };
  /*
  Mark Subbarao suggested the following RGB values based on star type:
   type    R     G     B
    O      00    66    FF
    B      33    99    FF
    A      CC    FF    FF
    F      FF    FF    FF
    G      FF    CC    33
    K      FF    66    00
    M      FF    00    00
*/
  color::T getColor(char sc) {
    switch (sc) {
      case SPECTRUM_O: return { 0x00,    0x66,    0xFF};
      case SPECTRUM_B: return { 0x33,    0x99,    0xFF};
      case SPECTRUM_A: return { 0xCC,    0xFF,    0xFF};
      case SPECTRUM_F: return { 0xFF,    0xFF,    0xFF};
      case SPECTRUM_G: return { 0xFF,    0xCC,    0x33};
      case SPECTRUM_K: return { 0xFF,    0x66,    0x00};
      case SPECTRUM_M: return { 0xFF,    0x00,    0x00};
      case SPECTRUM_Unknown: return { 0, 0, 0};
      default: ABORT();
    }
  }

  double getMagnitude(enum SpectrumE s) {
    color::T col = getColor(s);
    return sqrt(sqr(col.r)+sqr(col.g)+sqr(col.b));
  }

  enum SpectrumE getIndex(char *str) {
    switch (str[0]) {
      case 'O': return SPECTRUM_O;
      case 'B': return SPECTRUM_B;
      case 'A': return SPECTRUM_A;
      case 'F': return SPECTRUM_F;
      case 'G': return SPECTRUM_G;
      case 'K': return SPECTRUM_K;
      case 'M': return SPECTRUM_M;
      default: return SPECTRUM_Unknown;
    }
  }

} // namespace spectrum
  

struct star {
  double ra, dec, dist;
  double absmag;
  enum spectrum::SpectrumE spectrum;

  star(double ra0, double dec0, double dist0): ra(ra0), dec(dec0), dist(dist0) {}

  star(char *line) {
    char tstr[64];
    for (int i=0; i<7; i++) line = nextField(line);
    sscanf(line, " %lg ,", &ra);  line = nextField(line);
    sscanf(line, " %lg ,", &dec); line = nextField(line);
    sscanf(line, " %lg ,", &dist); line = nextField(line);
    line = nextField(line);
    sscanf(line, " %lg ,", &absmag); line = nextField(line);
    sscanf(line, " %s ,", tstr); line = nextField(line);
    spectrum = spectrum::getIndex(tstr);
    ra = ra * M_PI / 12;
    dec = dec * M_PI / 180;
  }

  dim3::T getXYZ() const { return { dist*cos(ra)*cos(dec),
  				    dist*sin(ra)*cos(dec),
				    dist*sin(dec) }; }

  double getDist() const { return dist; }
  double getSpectrum() const { return spectrum; }
  color::T getColor() const {
    double smag = spectrum::getMagnitude(spectrum);
    return color::emul(spectrum::getColor(spectrum), 128*exp((-3.-absmag)/10.)/smag);
  }

  void dump() { printf("ra %12.10g dec %12.10g dist %12.10g\n", ra, dec, dist); }

  void dumpXY() { printf("%g %g\n", dist*cos(ra)*cos(dec), dist*sin(dec)); }

};


struct cat {
  std::vector<star> stars;

  void readAll(const char *filename) {
    char *line=0;
    size_t n;
    FILE *fdi=fopen(filename, "r");
    while (getline(&line, &n, fdi)) {
      if (strlen(line)==0) break;
      if (line[0] != '#') stars.push_back(star(line));
      free(line); line=0;
    }
    fprintf(stderr, "# nb_stars %lu\n", stars.size());
  }

  std::vector<star>::iterator begin() { return stars.begin(); }
  std::vector<star>::iterator end()   { return stars.end(); }

  void dump() { std::for_each(stars.begin(), stars.end(), [] (star s) { s.dump(); }); }
  void dumpXY() { std::for_each(stars.begin(), stars.end(), [] (star s) { s.dumpXY(); }); }

};




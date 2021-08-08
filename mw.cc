#include "stars.hh"

int main(void)
{
  cat hyg;
  hyg.readAll("data/hygfull.csv");
  // hyg.readAll("data/hygdata_v3.csv");
  // hyg.dumpXY();

  star sun{0, 0, 0};
  star galacticCenter{ angles::toReal({17, 45, 6})*M_PI/12, -28.94*M_PI/180, 8.34e3 /* wiki */ };
  star galacticNorthPole{ angles::toReal({12,51,4})*M_PI/12, 27.13*M_PI/180, 1.e7 }; 

  dim3::T u = sub(sun.getXYZ(), galacticCenter.getXYZ());
  dim3::T w = sub(galacticNorthPole.getXYZ(), galacticCenter.getXYZ());

  u = dim3::ediv(u, dim3::norm(u));
  w = dim3::ediv(w, dim3::norm(w));
  dim3::T v = dim3::cross(w, u);
  fprintf(stderr, "# u.w %g\n", dim3::dot(u, w));

  std::for_each(hyg.begin(), hyg.end(), [galacticCenter, u, v, w](star s) {
    dim3::T sp = sub(s.getXYZ(), galacticCenter.getXYZ());
    double x = dot(sp, u), y = dot(sp, v), z = dot(sp, w);
    if (dim3::norm(sp)<50000) printf("%g  %g  %g\n", x, y, z);
  });

  /*
  std::for_each(hyg.begin(), hyg.end(), [](star s) {
    dim3::T p = s.getXYZ();
    if (s.getDist()<100000) printf("%g  %g  %g\n", p.c[0], p.c[1], p.c[2]);
  });
  */
}
  

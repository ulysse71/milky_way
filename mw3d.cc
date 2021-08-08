#include <vector>

#include <GL/glut.h>

#include "stars.hh"

const int debug = 2;
 
/* Global variables */
static char title[] = "Milky Way 3D";
static int refreshTime = 10;        // refresh interval in milliseconds [NEW]

static GLfloat angleDelta = 0.02f;
	
static dim3::T eye = dim3::zero, center = dim3::zero;

static std::vector<dim3::T> points;
static std::vector<color::T> colors;


/// keyboard entries
typedef unsigned char byte;
static char key;
static int kx, ky;

void keyPressedHook(byte k, int x, int y) {
  key = k; kx = x; ky = y;
  if (debug>3) fprintf(stderr, "key %c pressed\n", k);
}

inline byte getKeyPressed() { byte tkey = key; key=0; return tkey; }

/* Initialize OpenGL Graphics */
void initGL() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
  glClearDepth(1.0f);                   // Set background depth to farthest
  glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
  glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
  glShadeModel(GL_SMOOTH);   // Enable smooth shading
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
  eye = {0, 0, -40};
}
 
void display() {
  byte key=0;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

  // model matrix
  glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
  glLoadIdentity();                 // Reset the model-view matrix

  dim3::T vec = sub(eye, center);
  double dist = norm(vec);
  dim3::T u = ediv(vec, dist);
  dim3::T v = normalize(cross(dim3::vj, u)); /// CAVEAT u should not be vk
  dim3::T w = cross(u, v);

  // view matrix 
  gluLookAt(eye.c[0], eye.c[1], eye.c[2],
            center.c[0], center.c[1], center.c[2], 
            0., 1., 0.);
 
  // define model
  glBegin(GL_POINTS);
     glColor3f(1.0f, 1.0f, 1.0f);     // Green
     for (unsigned i=0; i<points.size(); i++) {
       glColor3f(colors[i].r, colors[i].g, colors[i].b); 
       glVertex3f(points[i].c[0], points[i].c[1], points[i].c[2]);
     }
  glEnd();  // End of drawing color-cube
 
  glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)

  switch (key=getKeyPressed()) {
    case 0: break;
    case 's': center = add(center, emul(0.5, vec)); break;
    case 'd': center = sub(center, emul(0.5, vec)); break;
    case 'l': eye = sub(eye, emul(angleDelta*dist, v)); break;
    case 'h': eye = add(eye, emul(angleDelta*dist, v)); break;
    case 'j': eye = add(eye, emul(angleDelta*dist, w)); break;
    case 'k': eye = sub(eye, emul(angleDelta*dist, w)); break;
    case 'i': eye = sub(eye, emul(angleDelta, vec)); break;
    case 'n': eye = add(eye, emul(angleDelta, vec)); break;
    case 'Q': exit(0); break;
    default: printf("key unknown %c\n", key);
  }
  if (key!=0) printf("dist %g cen [%g, %g, %g] eye [%g, %g, %g]\n", dist,
     		     center.c[0], center.c[1], center.c[2],
		     eye.c[0], eye.c[1], eye.c[2]);

}
 
/* Called back when timer expired */
void timer(int value) {
  glutPostRedisplay();      // Post re-paint request to activate display()
  glutTimerFunc(refreshTime, timer, 0); // next timer call milliseconds later
}


/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
  // set the new projection matrix
  // Compute aspect ratio of the new window
  if (height == 0) height = 1;                // To prevent divide by 0
  GLfloat aspect = (GLfloat)width / (GLfloat)height;

  // Set the viewport to cover the new window
  glViewport(0, 0, width, height);

  // Set the aspect ratio of the clipping volume to match the viewport
  glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
  glLoadIdentity();             // Reset
  // Enable perspective projection with fovy, aspect, zNear and zFar
  gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}
 

int main(int argc, char **argv)
{
  cat hyg;
  double galacticDiameter = 50000.;
  hyg.readAll("data/hygfull.csv");
  // hyg.readAll("data/hygdata_v3.csv");
  // hyg.dump();

  star sun{0, 0, 0};
  star galacticCenter{ angles::toReal({17, 45, 6})*M_PI/12, -28.94*M_PI/180, 8.2e3 /* wiki J2000*/ };
  // star galacticCenter{ angles::toReal({17, 42, 4})*M_PI/12, -28.92*M_PI/180, 8.34e3 /* wiki B1950 */ };
  star galacticNorthPole{ angles::toReal({12,51,4})*M_PI/12, 27.13*M_PI/180, 1.e7 }; 

  dim3::T u = sub(sun.getXYZ(), galacticCenter.getXYZ());
  dim3::T w = sub(galacticNorthPole.getXYZ(), galacticCenter.getXYZ());

  u = dim3::ediv(u, dim3::norm(u));
  w = dim3::ediv(w, dim3::norm(w));
  dim3::T v = dim3::cross(w, u);
  fprintf(stderr, "# u.w %g\n", dim3::dot(u, w));

  std::for_each(hyg.begin(), hyg.end(), [galacticCenter, galacticDiameter, u, v, w](star s) {
    dim3::T sp = sub(s.getXYZ(), galacticCenter.getXYZ());
    double x = dot(sp, u), y = dot(sp, v), z = dot(sp, w);
    if (dim3::norm(sp)<galacticDiameter) {
      // distance units are kpsc
      points.push_back(dim3::ediv({x, y, z}, 1.e3));
      colors.push_back(s.getColor());
    }
  });

  glutInit(&argc, argv);            // Initialize GLUT
  glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
  glutInitWindowSize(1200, 900);   // Set the window's initial width & height
  glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
  glutCreateWindow(title);          // Create window with the given title
  glutKeyboardFunc(keyPressedHook);// keyboard callback
  glutDisplayFunc(display);       // Register callback handler for window re-paint event
  glutReshapeFunc(reshape);       // Register callback handler for window re-size event
  initGL();                       // Our own OpenGL initialization
  glutTimerFunc(0, timer, 0);     // First timer call immediately [NEW]
  glutMainLoop();                 // Enter the infinite event-processing loop
  return 0;

}
  


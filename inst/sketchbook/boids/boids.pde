import R4P.*;

flock fl;
R4P R;

void setup () {
  size(600, 400);
  noStroke();
  background(0);
  fill(0, 102, 153, 204);
  textFont(createFont("SansSerif",10));
  smooth();
  noCursor();
  R = new R4P (this, true);
  fl = new flock (R);
}

int[] getPixels ()
{
  loadPixels();
  return ((int[])pixels);
}

void bupdate (double[] q)
{
  int N = fl.N;
  for (int j=0;j<N;j++) {
    fl.Boids[j][0] = (float)q[j];      // x
    fl.Boids[j][1] = (float)q[j+N];    // y
    fl.Boids[j][2] = (float)q[j+2*N];  // x velocity
    fl.Boids[j][3] = (float)q[j+3*N];  // y velocity
    fl.Boids[j][4] = (float)q[j+4*N];  // count of nearby boids
  }
}

void mousePressed ()
{
  if (mouseButton==RIGHT) {
    fl.dens = !fl.dens;
    fl.drawBoids = true;
    loop();
  }
  else {
    R.re.eval ("mousePressed()");
    if (blue(fl.repulsor)==255) fl.repulsor=color(255,0,0);
    else if (red(fl.repulsor)==255) fl.repulsor=color(0,255,0);
    else fl.repulsor=color(0,0,255);
  }
}

void yelp (String s) 
{
  System.out.println(s);
}

void densify ()
{
  PVector loc, vel;
  for (int j=0;j<fl.N;++j){
    loc = new PVector (fl.Boids[j][0], fl.Boids[j][1]);
    vel = new PVector (fl.Boids[j][2], fl.Boids[j][3]);
    float theta = vel.heading2D() + PI/2;
    stroke (5,0,0);
    ellipseMode (CENTER);
    ellipse (loc.x, loc.y, 200, 200);
  }
}

void draw () 
{
  background(0);
  fl.run();
}

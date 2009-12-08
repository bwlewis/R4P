
class flock 
{

  R4P R;
  int N = 55;
  float r = 2.0;
  public float Boids[][];
  public color repulsor = color(0,255,0);

  flock (R4P Re) 
  {
    R = Re;
    Boids = new float[N][5];
    R.re.eval ("cat ('Welcome to R4P!\n')");
    R.re.eval ("r=2.0");
    int [] NN={
      N    };
    R.re.assign ("N", NN);
    R.re.eval ("source ('boids.R')");
  }

  void render ()
  {
    PVector loc, vel;
    stroke (repulsor);
    fill (repulsor);
    ellipseMode (CENTER);
    ellipse (mouseX, mouseY, 5, 5);
    for (int j=0;j<N;++j){
      loc = new PVector (Boids[j][0], Boids[j][1]);
      vel = new PVector (Boids[j][2], Boids[j][3]);
      float theta = vel.heading2D() + PI/2;
      if (Boids[j][4] < 2) {
        fill (0,0,0); 
        stroke (100,0,0);
      }
      else if (Boids[j][4] < 5) {
        fill (50,0,0); 
        stroke (150,0,0);
      }
      else if (Boids[j][4] < 10) {
        fill (150,0,0); 
        stroke (200,0,0);
      }
      else {
        fill (255,0,0); 
        stroke (255,0,0);
      }
      pushMatrix ();
      translate (loc.x, loc.y);
      rotate (theta);
      beginShape (TRIANGLES);
      vertex (0,-r*2);
      vertex (-r,r*2);
      vertex (r,r*2);
      endShape ();
      popMatrix (); 
    }
  }

  void run () {
    R.re.eval ("step()");
    render ();
  }

}

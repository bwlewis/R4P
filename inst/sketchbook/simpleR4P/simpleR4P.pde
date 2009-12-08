/* R4P Interactive R and Processing Example
 *
 * R4P provides a really simple R library for Processing.
 * Processing may call R functions and vice-versa. The R engine
 * runs inside a Processing Java thread.
 *
 * Before starting up R4P you must do two things:
 * 1. Set LD_LIBRARY_PATH to include your R library path
 * 2. Set R_HOME to point to your R installation
 * These steps are performed by the R4P.sh startup script.
 */

import R4P.*;
R4P R;

void setup () {
  // Setup:
  size(500, 300);
  background(255,255,255);
  smooth();
  frameRate (30);

  // Create a default font for use by the R graphics device:
  textFont(createFont("SansSerif",11));
  
  // The R4P constructor takes two arguments, 'this'
  // and a boolean value. Set the 2nd argument to true
  // to display a (crude) interactive R console.
  R = new R4P (this, true);
  // The 'R' object is now a running R engine. We can
  // instruct it to do things, for example:
  R.re.eval ("cat ('Welcome to R4P.\n')");
  // An interactive R console is also displayed in this 
  // example. The output from the above command appears
  // in the console. You'll need to explicitly print or
  // cat output to the console to see it.

  // The running Processing engine is avaiable from inside the
  // running R engine as the 'PApplet' object in the global
  // environment.

}

void draw () 
{
}

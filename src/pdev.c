#include <stdio.h>

#include <R.h>
#include <Rversion.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>

#if R_VERSION >= R_Version(2,8,0)
#ifndef NewDevDesc
#define NewDevDesc DevDesc
#endif
#endif

#define CREDC(C) (((unsigned int)(C))&0xff)
#define CGREENC(C) ((((unsigned int)(C))&0xff00)>>8)
#define CBLUEC(C) ((((unsigned int)(C))&0xff0000)>>16)
#define CALPHA(C) ((((unsigned int)(C))&0xff000000)>>24)

static void pdevTranslate (double, double, pDevDesc);
static void pdevRotate (double, pDevDesc);
static void pdevPushMatrix (pDevDesc);
static void pdevPopMatrix (pDevDesc);
// RcallMethod is defined in rJava.h
SEXP RcallMethod (SEXP par);

typedef struct _pdevDesc
{
  /* device specific stuff */
  int col;
  int fill;
  SEXP PApplet;	// PApplet Java object reference external pointer

  /* Line characteristics */
  double lwd;
  int lty;
  R_GE_lineend lend;
  R_GE_linejoin ljoin;
  double lmitre;

  pGEDevDesc RGE;
} pdevDesc;

// Wrapper for the rJava approach to passing floats to Java functions
SEXP
jfloat (double x)
{
  SEXP y = ScalarReal (x);
  setAttrib (y, install ("class"), ScalarString (mkChar ("jfloat")));
  return y;
}

/* pdevColor type should be "stroke" "background" or "fill" */
static void
pdevColor (const char *type, unsigned int c, pdevDesc * pGD)
{
  SEXP s, t;
  SEXP red = jfloat ((double) CREDC (c));
  SEXP green = jfloat ((double) CGREENC (c));
  SEXP blue = jfloat ((double) CBLUEC (c));
  SEXP alpha = jfloat ((double) CALPHA (c));
  PROTECT (t = s = allocList (8));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar (type)));
  t = CDR (t);
  SETCAR (t, red);
  t = CDR (t);
  SETCAR (t, green);
  t = CDR (t);
  SETCAR (t, blue);
  t = CDR (t);
  SETCAR (t, alpha);
  RcallMethod (s);
  UNPROTECT (1);
}

static void
redraw (pdevDesc * pGD)
{
  SEXP s, t;
  PROTECT (t = s = allocList (4));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("redraw")));
  RcallMethod (s);
  UNPROTECT (1);
}

static void
pdevNoStroke (pdevDesc * pGD)
{
  SEXP s, t;
  PROTECT (t = s = allocList (4));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("noStroke")));
  RcallMethod (s);
  UNPROTECT (1);
}

static void
pdevNoFill (pdevDesc * pGD)
{
  SEXP s, t;
  PROTECT (t = s = allocList (4));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("noFill")));
  RcallMethod (s);
  UNPROTECT (1);
}

static void
pdevSetLineType (pdevDesc * pGD, pGEcontext gc)
{
  SEXP s, t, u;
  /* Line width */
  if (pGD->lwd != gc->lwd)
    {
      u = jfloat ((double) (gc->lwd));
      PROTECT (t = s = allocList (5));
      SET_TYPEOF (s, LANGSXP);
      SETCAR (t, ScalarString (mkChar ("RcallMethod")));
      t = CDR (t);
      SETCAR (t, pGD->PApplet);
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("V")));
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("strokeWeight")));
      t = CDR (t);
      SETCAR (t, u);
      RcallMethod (s);
      UNPROTECT (1);
      pGD->lwd = gc->lwd;
    }

  /* Line end: par lend  */
  if (pGD->lend != gc->lend)
    {
      pGD->lend = gc->lend;
      if (pGD->lend == GE_ROUND_CAP)
	{
	}
      if (pGD->lend == GE_BUTT_CAP)
	{
	}
      if (pGD->lend == GE_SQUARE_CAP)
	{
	}
    }

  /* Line join: par ljoin */
  if (pGD->ljoin != gc->ljoin)
    {
      pGD->ljoin = gc->ljoin;
      if (pGD->ljoin == GE_ROUND_JOIN)
	{
	}
      if (pGD->ljoin == GE_MITRE_JOIN)
	{
	}
      if (pGD->ljoin == GE_BEVEL_JOIN)
	{
	}
    }

  /* Miter limit */
  if (pGD->lmitre != gc->lmitre)
    {
      pGD->lmitre = gc->lmitre;
//XXX Set miterLimit 
    }
}

static void
pdevActivate (const pDevDesc RGD)
{
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
/* Switch to manual draw() mode. NB mouse and other interactive operations  
 * will not be available until deactivate is called (or loop() is 
 * explicitly re-enabled).
 */
//  SEXP s, t;
//  PROTECT (t = s = allocList (4));
//  SET_TYPEOF (s, LANGSXP);
//  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
//  t = CDR (t);
//  SETCAR (t, pGD->PApplet);
//  t = CDR (t);
//  SETCAR (t, ScalarString (mkChar ("V")));
//  t = CDR (t);
//  SETCAR (t, ScalarString (mkChar ("noLoop")));
//  RcallMethod (s);
//  UNPROTECT (1);
}

static void
pdevCircle (double x, double y, double r, const pGEcontext gc, pDevDesc RGD)
{
  SEXP s, t;
  SEXP ex = jfloat (x);
  SEXP ey = jfloat (y);
  SEXP er = jfloat (r);
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  pdevSetLineType (pGD, gc);
  if (CALPHA (gc->fill))
    pdevColor ("fill", gc->fill, pGD);
  else
    pdevNoFill (pGD);
  if (CALPHA (gc->col) && gc->lty != -1)
    pdevColor ("stroke", gc->col, pGD);
  else
    pdevNoStroke (pGD);
  PROTECT (t = s = allocList (5));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("ellipseMode")));
  t = CDR (t);
  SETCAR (t, ScalarInteger (3));
  RcallMethod (s);

  PROTECT (t = s = allocList (8));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("ellipse")));
  t = CDR (t);
  SETCAR (t, ex);
  t = CDR (t);
  SETCAR (t, ey);
  t = CDR (t);
  SETCAR (t, er);
  t = CDR (t);
  SETCAR (t, er);
  RcallMethod (s);
  UNPROTECT (2);
}

static void
pdevClip (double x0, double x1, double y0, double y1, pDevDesc RGD)
{
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
}

static void
pdevDeactivate (pDevDesc RGD)
{
/* Switch back to continuous loop draw(). */
//  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
//  SEXP s, t;
//  redraw (pGD);
//  PROTECT (t = s = allocList (4));
//  SET_TYPEOF (s, LANGSXP);
//  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
//  t = CDR (t);
//  SETCAR (t, pGD->PApplet);
//  t = CDR (t);
//  SETCAR (t, ScalarString (mkChar ("V")));
//  t = CDR (t);
//  SETCAR (t, ScalarString (mkChar ("loop")));
//  RcallMethod (s);
//  UNPROTECT (1);
}

static void
pdevClose (pDevDesc RGD)
{
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  free (pGD);
  RGD->deviceSpecific = NULL;
  pdevDeactivate (RGD);
}

static Rboolean
pdevLocator (double *x, double *y, pDevDesc RGD)
{
  return TRUE;
}

static void
pdevLine (double x1, double y1, double x2, double y2, const pGEcontext gc,
	  pDevDesc RGD)
{
  SEXP s, t;
  SEXP fx1 = jfloat (x1);
  SEXP fx2 = jfloat (x2);
  SEXP fy1 = jfloat (y1);
  SEXP fy2 = jfloat (y2);
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  if (CALPHA (gc->col) && gc->lty != -1)
    {
      pdevSetLineType (pGD, gc);
      pdevColor ("stroke", gc->col, pGD);
      PROTECT (t = s = allocList (8));
      SET_TYPEOF (s, LANGSXP);
      SETCAR (t, ScalarString (mkChar ("RcallMethod")));
      t = CDR (t);
      SETCAR (t, pGD->PApplet);
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("V")));
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("line")));
      t = CDR (t);
      SETCAR (t, fx1);
      t = CDR (t);
      SETCAR (t, fy1);
      t = CDR (t);
      SETCAR (t, fx2);
      t = CDR (t);
      SETCAR (t, fy2);
      RcallMethod (s);
      UNPROTECT (1);
    }
}

static void
pdevMetricInfo (int c, const pGEcontext gc, double *ascent, double *descent,
		double *width, pDevDesc RGD)
{
  SEXP s, t, ans;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  PROTECT (t = s = allocList (4));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("F")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("textAscent")));
  ans = RcallMethod (s);
  *ascent = *(REAL (ans));
  t = CDR (s);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("F")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("textDescent")));
  ans = RcallMethod (s);
  *descent = *(REAL (ans));
  UNPROTECT (1);
  *width = 0.0;			// XXX 
}

static void
pdevMode (int mode, pDevDesc RGD)
{
}

static void
pdevTranslate (double x, double y, pDevDesc RGD)
{
  SEXP s, t, fx, fy;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  fx = jfloat (x);
  fy = jfloat (y);
  PROTECT (t = s = allocList (6));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("translate")));
  t = CDR (t);
  SETCAR (t, fx);
  t = CDR (t);
  SETCAR (t, fy);
  RcallMethod (s);
  UNPROTECT (1);
}

static void
pdevRotate (double r, pDevDesc RGD)
{
  SEXP s, t, fr;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  fr = jfloat (r);
  PROTECT (t = s = allocList (5));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("rotate")));
  t = CDR (t);
  SETCAR (t, fr);
  RcallMethod (s);
  UNPROTECT (1);
}

static void
pdevPushMatrix (pDevDesc RGD)
{
  SEXP s, t;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  PROTECT (t = s = allocList (4));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("pushMatrix")));
  RcallMethod (s);
  UNPROTECT (1);
}

static void
pdevPopMatrix (pDevDesc RGD)
{
  SEXP s, t;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  PROTECT (t = s = allocList (4));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("popMatrix")));
  RcallMethod (s);
  UNPROTECT (1);
}

static void
pdevNewPage (const pGEcontext gc, pDevDesc RGD)
{
  SEXP s, t;
  SEXP red, green, blue, alpha;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  red = jfloat ((double) CREDC (RGD->startfill));
  green = jfloat ((double) CGREENC (RGD->startfill));
  blue = jfloat ((double) CBLUEC (RGD->startfill));
  PROTECT (t = s = allocList (7));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("background")));
  t = CDR (t);
  SETCAR (t, red);
  t = CDR (t);
  SETCAR (t, green);
  t = CDR (t);
  SETCAR (t, blue);
  RcallMethod (s);
  redraw (pGD);
  UNPROTECT (1);
}

static void
pdevPolygon (int n, double *x, double *y, const pGEcontext gc, pDevDesc RGD)
{
  SEXP s, t, u, v;
  int j = 0, p = 0;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;

  if (n < 2)
    return;

  PROTECT (t = s = allocList (4));
  p++;
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("beginShape")));
  RcallMethod (s);
  pdevSetLineType (pGD, gc);
  if (CALPHA (gc->fill))
    pdevColor ("fill", gc->fill, pGD);
  else
    pdevColor ("fill", 255, pGD);
  if (CALPHA (gc->col) && gc->lty != -1)
    pdevColor ("stroke", gc->col, pGD);
  while (j < n)
    {
      u = jfloat (x[j]);
      v = jfloat (y[j]);
      PROTECT (t = s = allocList (6));
      p++;
      SET_TYPEOF (s, LANGSXP);
      SETCAR (t, ScalarString (mkChar ("RcallMethod")));
      t = CDR (t);
      SETCAR (t, pGD->PApplet);
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("V")));
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("vertex")));
      t = CDR (t);
      SETCAR (t, u);
      t = CDR (t);
      SETCAR (t, v);
      RcallMethod (s);
      j++;
    }
  PROTECT (t = s = allocList (5));
  p++;
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("V")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("endShape")));
  t = CDR (t);
  SETCAR (t, ScalarInteger (2));
  RcallMethod (s);
  UNPROTECT (p);
}

static void
pdevPolyline (int n, double *x, double *y, const pGEcontext gc, pDevDesc RGD)
{
  int j = 1;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;

  if (n < 2)
    return;

  if (CALPHA (gc->col) && gc->lty != -1)
    {
      pdevSetLineType (pGD, gc);
      pdevColor ("stroke", gc->col, pGD);
      while (j < (n - 1))
	{
	  pdevLine (x[j], y[j], x[j + 1], y[j + 1], gc, RGD);
	  j++;
	}
    }
}

static void
pdevRect (double x0, double y0, double x1, double y1, const pGEcontext gc,
	  pDevDesc RGD)
{
  double *x = (double *) malloc (4 * sizeof (double));
  double *y = (double *) malloc (4 * sizeof (double));
  x[0] = x0;
  x[1] = x1;
  x[2] = x1;
  x[3] = x0;
  y[0] = y0;
  y[1] = y0;
  y[2] = y1;
  y[3] = y1;
  pdevPolygon (4, x, y, gc, RGD);
}

static void
pdevSize (double *left, double *right, double *bottom, double *top,
	  pDevDesc RGD)
{
}

static double
pdevStrWidth (const char *str, const pGEcontext gc, pDevDesc RGD)
{
  SEXP s, t, w;
  double x;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  PROTECT (t = s = allocList (5));
  SET_TYPEOF (s, LANGSXP);
  SETCAR (t, ScalarString (mkChar ("RcallMethod")));
  t = CDR (t);
  SETCAR (t, pGD->PApplet);
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("F")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar ("textWidth")));
  t = CDR (t);
  SETCAR (t, ScalarString (mkChar (str)));
  w = RcallMethod (s);
  UNPROTECT (1);
  x = *(REAL (w));
  return x;
}

static void
pdevText (double x, double y, const char *str, double rot, double hadj,
	  const pGEcontext gc, pDevDesc RGD)
{
  double strextent;
  SEXP s, t, u;
  int p = 0;
  rot = -(rot / 180) * PI;
  pdevDesc *pGD = (pdevDesc *) RGD->deviceSpecific;
  pdevColor ("fill", gc->col, pGD);

  strextent = pdevStrWidth (str, gc, RGD);
  pdevPushMatrix (RGD);
  if (rot != 0.)
    {
      pdevTranslate (x-strextent*cos(rot)/2, y-strextent*sin(rot)/2, RGD);
      pdevRotate (rot, RGD);
      PROTECT (t = s = allocList (7));
      p++;
      SET_TYPEOF (s, LANGSXP);
      SETCAR (t, ScalarString (mkChar ("RcallMethod")));
      t = CDR (t);
      SETCAR (t, pGD->PApplet);
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("V")));
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("text")));
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar (str)));
      t = CDR (t);
      u = jfloat (0.0);
      SETCAR (t, u);
      t = CDR (t);
      SETCAR (t, u);
      RcallMethod (s);
    }
  else
    {
      pdevTranslate (x-strextent/2-hadj, y, RGD);
      PROTECT (t = s = allocList (7));
      p++;
      SET_TYPEOF (s, LANGSXP);
      SETCAR (t, ScalarString (mkChar ("RcallMethod")));
      t = CDR (t);
      SETCAR (t, pGD->PApplet);
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("V")));
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar ("text")));
      t = CDR (t);
      SETCAR (t, ScalarString (mkChar (str)));
      t = CDR (t);
      u = jfloat (0.0);
      SETCAR (t, u);
      t = CDR (t);
      SETCAR (t, u);
      RcallMethod (s);
    }
  pdevPopMatrix (RGD);
  redraw (pGD);
  UNPROTECT (p);
}

SEXP
pdev_new_device (SEXP args)
{
  /* R Graphics Device: in GraphicsDevice.h */
  pDevDesc RGD;

  /* R Graphics Engine: in GraphicsEngine.h */
  pGEDevDesc RGE;

  /* pdev Graphics Device */
  pdevDesc *pGD;

  int width, height, bgcolor;

  SEXP v, PApplet;
  args = CDR (args);

  PApplet = CAR (args);
  args = CDR (args);

  v = CAR (args);
  args = CDR (args);
  if (!isNumeric (v))
    {
      error ("`width' must be a number");
    }
  width = asInteger (v);
  v = CAR (args);
  args = CDR (args);
  if (!isNumeric (v))
    {
      error ("`height' must be a number");
    }
  height = asInteger (v);
  v = CAR (args);
  args = CDR (args);
  if (!isString (v) && !isInteger (v) && !isLogical (v) && !isReal (v))
    error ("invalid color specification for `bg'");
  bgcolor = RGBpar (v, 0);

  R_CheckDeviceAvailable ();

  if (!(RGD = (pDevDesc) calloc (1, sizeof (NewDevDesc))))
    {
      error ("calloc failed for pdev device");
    }

  if (!(pGD = (pdevDesc *) calloc (1, sizeof (pdevDesc))))
    {
      free (RGD);
      error ("calloc failed for pdev device");
    }

  pGD->PApplet = PApplet;
  RGD->deviceSpecific = (void *) pGD;

  /* Callbacks */
  RGD->close = pdevClose;
  RGD->activate = pdevActivate;
  RGD->deactivate = pdevDeactivate;
  RGD->size = pdevSize;
  RGD->newPage = pdevNewPage;
  RGD->clip = pdevClip;
  RGD->strWidth = pdevStrWidth;
  RGD->text = pdevText;
  RGD->rect = pdevRect;
  RGD->circle = pdevCircle;
  RGD->line = pdevLine;
  RGD->polyline = pdevPolyline;
  RGD->polygon = pdevPolygon;
  RGD->locator = pdevLocator;
  RGD->mode = pdevMode;
  RGD->metricInfo = pdevMetricInfo;
  RGD->hasTextUTF8 = TRUE;
  RGD->strWidthUTF8 = pdevStrWidth;
  RGD->textUTF8 = pdevText;
  RGD->wantSymbolUTF8 = TRUE;

  /* Initialise RGD */
  RGD->left = RGD->clipLeft = 0;
  RGD->top = RGD->clipTop = 0;
  RGD->right = RGD->clipRight = width;
  RGD->bottom = RGD->clipBottom = height;
  RGD->xCharOffset = 0.4900;
  RGD->yCharOffset = 0.3333;
  RGD->yLineBias = 0.1;
  RGD->ipr[0] = 1.0 / 72.0;
  RGD->ipr[1] = 1.0 / 72.0;
  RGD->cra[0] = 0.9 * 10;
  RGD->cra[1] = 1.2 * 10;
  RGD->gamma = 1.0;
  RGD->canClip = FALSE;
  RGD->canChangeGamma = FALSE;
  RGD->canHAdj = 2;
  RGD->startps = 10.0;
  RGD->startcol = R_RGB (0, 0, 0);
  RGD->startfill = bgcolor;
  RGD->startlty = LTY_SOLID;
  RGD->startfont = 1;
  RGD->startgamma = RGD->gamma;
  RGD->displayListOn = FALSE;

  /* Add to the device list */
  RGE = GEcreateDevDesc (RGD);
  pGD->RGE = RGE;
  GEaddDevice (RGE);
  GEinitDisplayList (RGE);

  /*return ScalarInteger(1 + GEdeviceNumber(RGE)); */
  return R_NilValue;
}

R_ExternalMethodDef pdev_externals[] = {
  {"pdev_new_device", (DL_FUNC) & pdev_new_device, 3}
  ,
  {NULL, NULL, 0}
};

void
R_init_pdev (DllInfo * info)
{
  R_registerRoutines (info, NULL, NULL, NULL, pdev_externals);
}

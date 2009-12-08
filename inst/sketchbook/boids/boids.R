# Boid state is stored in an N x 4 array, the 
# columns hold loc.x,loc.y,vel.x,vel.y.

# XXX Initialize r, N, width, height from Processing
# or uncomment to run only in R
# N = 30
# width = 100
# height = 100
# r = 2.0
# par (mar=c(1,1,1,1),pch=20)
# ------------------------------------------------

width <- dims()[1]
height <- dims()[2]

Boids = matrix (0.0, N, 5)
Boids[,1] = width/2
Boids[,2] = height/2
Boids[,3] = runif(N)*2 - 1 
Boids[,4] = runif(N)*2 - 1 
Boids[,5] = N

maxspeed = 2.0
maxforce = 0.05
repulsor = 0

# Run n iterations
go = function (n)
{
  for (j in 1:n) {
    step ()
#    plot (Boids[,1:2],xlim=c(0,width),ylim=c(0,height), xlab="", ylab="")
  }
}

step = function ()
{
  acc = flock ()
  update (acc)
  .jcall (PApplet, "V", "bupdate", Boids)
}

mousePressed = function ()
{
  if (repulsor==4) repulsor <<- -repulsor
  else if (repulsor==-4) repulsor <<- 0
  else repulsor <<- 4
}

clip = function (x, l)
{
  md = Mod (x)
  s = (md>l)*l/((md>l)*md + (md<=l)*1)
  s*x
}

flock = function () 
{
  desiredseparation = 15
  neighbordist = 50

  loc = Boids[,1]+Boids[,2]*1i
  vel = Boids[,3]+Boids[,4]*1i
  dif = outer (loc, loc, '-')    # differences
  dst = Mod(dif)                 # distances
  ndif = -dif/((dst==0)*1 + dst)  # normalized differences

# Not surprisingly,  we find that most of
# the time is spent in the following apply loops.
# It's not clear how to speed this code up natively in R.

# Check for nearby boids and steer away (separation) 
  hood = (dst>0 & dst<desiredseparation)
  count = pmax(apply (hood, 2, sum),1)
  sep = apply (ndif*hood, 2, sum)/count
  sep = clip (sep, maxforce)

# Compute the average velocity for every nearby boid (alignment)
  hood = (dst>0 & dst < neighbordist)
  tcount = apply (hood, 2, sum)
  count = pmax(tcount,1)
  ali = apply (vel*hood, 2, sum)/count
  ali = clip (ali, maxforce)

# Form the average location of nearby boids, calculate steering vector 
# towards that location (cohesion)
  hood = (dst>0 & dst < neighbordist)
  tcount = apply (hood, 2, sum)
  count = pmax(tcount,1)
  avg = apply (loc*hood, 2, sum)/count
  nz = tcount>0
  desired = avg - loc 
  md = Mod (desired)
  md[md==0]=1
  desired = desired/(md * count)
  coh = (desired - vel)*nz
  coh = clip (coh, maxforce)

# Avoid the mouse
  m = mouse()
  m = m[1] + m[2]*1i
  mdst = clip(loc - m,maxforce)

  acc = 0.3*sep + 0.2*ali + 0.19*coh  + repulsor*mdst
# Convert to array form for update, returning 
# xaccel, yaccel, neighborhood count (for coloring)
  cbind (Re(acc), Im(acc), tcount)
}

update = function (acc)
{
  b = Boids
  b[,3:4] = b[,3:4] + acc[,1:2]
  b[,1:2] = b[,1:2] + b[,3:4]
  b[,3:4] = pmin(b[,3:4],maxspeed)
  b[,5] = acc[,3]
  x=b[,1]
  y=b[,2]
  x[x < -r] = width + r
  x[x > width + r] = -r
  y[y < -r] = height + r
  y[y > height + r] = -r
  b[,1] = x
  b[,2] = y
  Boids <<- b
}

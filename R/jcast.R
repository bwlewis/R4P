`jcast` <-
function (x, t)
{
  switch (t, 
    I = as.integer (x),
    V = NULL,
    J = .jlong (x),
    F = .jfloat (x),
    D = as.double (x),
    Z = as.logical (x),
    x)
}


`estsigdif` <-
function (j, f)
{
  rj <- function (x) {
    switch (class(x), 
      numeric = "D",
      logical = "Z",
      integer = "I",
      character = "Ljava/lang/String",
      "jobjRef" = paste("L", x@jclass, sep=""),
      "*")
  }
  rs <- Map (rj,f)
  js <- strsplit (j, ":")[[1]]
  if (length (js)!=length (rs)) return (length(js))
  return (sum (rs!=js))
}


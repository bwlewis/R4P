`returnType` <-
function (x)
{
  y <- gsub ('\\[\\]','',x)
  a <- ''
  if (y!=x) a <- '['
  if (nchar(y)>0) {
    s <- switch (y,
      int  = "I",
      void = "V",
      long = "J",
      float = "F",
      double = "D",
      boolean = "Z",
      char = "C",
      byte = "B",
      paste ("L",y,sep=""))
    paste (a,s,sep='')
  }
  else s <- ''
  s
}


`fnCall` <-
function (x)
{
  f <- x[2]               # function name
  r <- returnType (x[1])        # returntype
  w <- paste (Map (returnType,x[3:length(x)]), collapse=":")
  n <- ifelse (nchar (w)>0, length (x)-2, 0)
  list (n=n, f=f, r=r, sig=w)
}


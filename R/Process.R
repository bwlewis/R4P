`Process` <- function (f=NULL, ...)
{
  cache <- paste(Sys.getenv("HOME"),".processingMethods.cache",sep="/")
  pparse <- function (y) {
    z <- gsub ('processing.core.PApplet.','',y)
    z <- strsplit (gsub ('throws.*','',z), ' ')[[1]]
    m <- length (z)
    z <- paste (z [(m-1):m],collapse=' ')
    z <- strsplit (gsub ('[(),]',' ',z), ' ')[[1]]
    fnCall (z)
  }
  if (!exists ('PApplet')) stop ("Processing was not found")
  if (!exists ('.processingMethods')) {
    tryCatch(load(cache),  error = function(e) invisible(),warning=function(w) invisible())
  }
  if (!exists ('.processingMethods')) {
    cat ("Buiding an index of Processing methods, please be patient...", file=stderr())
    flush.connection (stderr())
    x <- .jmethods (get('PApplet',envir=.GlobalEnv))
    z <- lapply (x, pparse)
    .processingMethods <<- data.frame (stringsAsFactors=FALSE)
    for (j in 1:length(z)) {
      .processingMethods <<- rbind (.processingMethods, as.data.frame (z[[j]],stringsAsFactors=FALSE))
    }
    save(.processingMethods,file=cache)
  }

  fname <- as.character((substitute(f)))
  a <- list (...)
  n <- length (a)
  if (length(fname)==0) {
# No function, print a helpful list
    print (.processingMethods[,'f']) 
    return (invisible (NULL))
  }
# Look up the requested method(s) in our table
  r <- rbind (.processingMethods[.processingMethods[,'n']==n & 
                          .processingMethods[,'f']==fname, ])
  if (nrow (r)<1) stop ("No matching function call available.")
  else if (nrow (r)==1) {
# Easy case...only one option.
    jsig <- strsplit(r[1,'sig'][[1]],":")[[1]]
    rt <- r[1,'r'][[1]]
    a <- Map (jcast, a, jsig)
  }
  else {
# Hard case...try to choose among several possibilities.
    d <- unlist (lapply (r[,'sig'],estsigdif, a))
    r <- r[d==min(d),,drop=FALSE]
    r <- r[1,,drop=FALSE]
    jsig <- strsplit(r[1,'sig'][[1]],":")[[1]]
    rt <- r[1,'r'][[1]]
    a <- Map (jcast, a, jsig)
  }
  fnargs <- c(get('PApplet',envir=.GlobalEnv), rt, fname, a)
  do.call (.jcall, fnargs)
}

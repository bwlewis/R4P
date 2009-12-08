`pdev` <- function(PApplet=NULL, bg="white", ...) 
{
  if(is.null(PApplet)) PApplet <- get('PApplet',envir=.GlobalEnv)
  width <- .jcall(PApplet,"I","getWidth")
  height <- .jcall(PApplet,"I","getHeight")
  invisible(.External("pdev_new_device", PApplet@jobj, width, height, bg, ..., PACKAGE="R4P"))
}

`dims` <-
function ()
{
  .jcall (get("R4P",envir=.GlobalEnv), "[I", "dims")
}

`mouse` <-
function ()
{
  .jcall (get("R4P",envir=.GlobalEnv), "[I", "mouse")
}

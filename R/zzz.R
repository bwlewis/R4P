.First.lib <- function(libname, pkgname) {
    library.dynam("R4P", pkgname, libname)
}

### in case we decide to keep the namespace ...
.onLoad <- .First.lib

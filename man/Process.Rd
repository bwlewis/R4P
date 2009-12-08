\name{Process}
\Rdversion{1.1}
\alias{Process}
\title{ Process }
\description{
The Process function implements a simple call interface to Processing functions from R.
}
\usage{
Process(f = NULL, ...)
}
\arguments{
  \item{f}{ A Processing function name (unquoted) }
  \item{\dots}{ The corresponding function arguments }
}
\details{The first time the Process functio runs it builds a table of available Processing methods which can results in a brief delay. Subsequent calls consult the cached tables. Future versions may incude a provision for off-line caching.}
\value{
The return value varies depending on the function call. See the Processing documentation.
}
\references{http://www.processing.org }
\author{ B. W. Lewis <bwaynelewis@gmail.com}
\note{The Process function uses a simple heuristic to match function arguments.  It may not always succeed--if you encounter an unexpected error, explicitly cast arguments to the desired type.}

\seealso{ }
\examples{
# Process (rect, 10,10,50,100)
}

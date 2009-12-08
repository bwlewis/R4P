\name{pdev}
\Rdversion{1.1}
\alias{pdev}
\title{ pdev }
\description{
Activate the Processing graphics output device for R.
}
\usage{
pdev(PApplet=NULL, bg="white", ...)
}
\arguments{
  \item{PApplet}{The Processing Java object reference (optional).}
  \item{bg}{Default background color (optional).}
  \item{\dots}{Additional graphics device arguments (optional).}
}
\details{Standard R graphics output will be displayed on the Processing output ccanvas after this call. Use dev.off() to disable the graphics device. The Processing draw() method may temporarily be disabled while pdev is active.}
\value{
Nothing is returned.
}
\references{http://www.processing.org }
\author{ B. W. Lewis <bwaynelewis@gmail.com}

\seealso{ }
\examples{
# pdev()
# countour(volcano,col=4)
}

# About

This R package provides seamless integration with the [ztsdb](http://www.ztsdb.org) DBMS.

# Installation

Note that rztsdb uses Linux-only API and therefore will not compile
and install on other operating systems.

Make sure ztsdb is installed first (see
[ztsdb installation](http://ztsdb.org/docs/install.html)); then the
rztsdb package can be installed in a straightforward way:

~~~ R
library(devtools)
devtools::install_git("https://lsilvest@gitlab.com/lsilvest/rztsdb.git")
~~~

# Examples

~~~ R
library(rztsdb)

## establish a connection to a 'ztsdb' instance listening on port 17001:
c1 <- connection("127.0.0.1", 17001)

## query to this instance using the c1 connection:

## first remotely create a 10x3 time-series and assign it to 'z':
c1 ? (z <<- zts(Sys.now() + 1:10*as.duration(1e6)), 1:30, dimnames=list(NULL, c("a","b","c"))

## query the newly created time-series:
c1 ? z            # get the complete times-series 'z'
c1 ? tail(z)      # get the last 6 observations from 'z'
c1 ? z[,"a"]      # get the column 'a' from 'z'
~~~

# Additional documentation

- [ztsdb R interface](http://ztsdb.org/docs/Rintegration.html)
- [ztsdb gitlab page](https://gitlab.com/lsilvest/ztsdb)
- [ztsdb website page](http://www.ztsdb.org)

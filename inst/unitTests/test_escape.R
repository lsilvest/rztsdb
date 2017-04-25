## test the escapement of each supported type; this tests the round
## trip from R to ztsdb and back.
test_double_scalar <- function() {
  checkEquals(1, c1 ? ++1)
}
test_double_scalar_multiple_plus <- function() {
  checkEquals(6, c1 ? (1 + 2 + 3))
}
test_double_scalar_multiple_plus <- function() {
  a <- 1; b <- 2; c <- 3
  checkEquals(6, c1 ? (++a + ++b + ++c))
}
test_double <- function() {
  m <- matrix(1:9, 3, 3, dimnames=list(c("1","2","3"), c("a","b","c")))
  checkEquals(m, c1 ? ++m)
}
test_bool_scalar <- function() {
  checkEquals(TRUE, c1 ? ++TRUE)
}
test_bool <- function() {
  checkEquals(c(a=TRUE,b=FALSE), c1 ? ++c(a=TRUE,b=FALSE))
}
test_time_scalar <- function() {
  p <- as.POSIXct("1970-01-01", tz="UTC")
  checkEquals(p, c1 ? ++p)
}
test_time <- function() {
  p <- as.POSIXct("1970-01-01", tz="UTC") + 1:10
  checkEquals(p, c1 ? ++p)
}
test_string_scalar <- function() {
  checkEquals("how soon is now?", c1 ? ++"how soon is now?")
}
test_string <- function() {
  s <- matrix(as.character(1:10), 5, 2)
  checkEquals(s, c1 ? ++s)
}
test_zts <- function() {
  x <- xts(1:9, as.POSIXct("1970-01-01", tz="UTC")+1:9)
  z <- (c1 ? ++x)
  checkTrue(all(index(x) == index(z)))  # we may have tzone difference, so don't use checkEquals
  checkTrue(all(coredata(x) == coredata(z)))
}

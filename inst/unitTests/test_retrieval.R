## test the retrieval of every type we support

## scalars; 
test_double_scalar <- function() {
  checkEquals(1, c1 ? 1)
}
test_bool_scalar <- function() {
  checkEquals(TRUE, c1 ? TRUE)
}
test_time_scalar <- function() {
  checkEquals(as.POSIXct("1970-01-01", tz="UTC"), c1 ? as.time("1970-01-01 00:00:00 UTC"))
}
test_string_scalar <- function() {
  checkEquals("how soon is now?", c1 ? "how soon is now?")
}
test_duration_scalar <- function() {
  checkEquals(as.integer64(10), c1 ? as.duration(10))
}

## vectors; 
test_double_vector <- function() {
  checkEquals(c(a=1,b=2), c1 ? c(a=1,b=2))
}
test_bool_vector <- function() {
  checkEquals(c(a=TRUE,b=FALSE), c1 ? c(a=TRUE,b=FALSE))
}
test_string_vector <- function() {
  checkEquals(c(a="how soon", b="is", c="now?"), c1 ? c(a="how soon", b="is", c="now?"))
}


## arrays; these have equality
test_double <- function() {
  checkEquals(matrix(1:9, 3, 3, dimnames=list(c("1","2","3"), c("a","b","c"))),
              c1 ? matrix(1:9, 3, 3, dimnames=list(c("1","2","3"), c("a","b","c"))))
}
test_bool <- function() {
  checkEquals(matrix(c(T,T,F,F), 2, 2), c1 ? matrix(c(T,T,F,F), 2, 2))
}
test_time <- function() {
  checkEquals(as.POSIXct("1970-01-01", tz="UTC") + 1:10,
              c1 ? as.time("1970-01-01 00:00:00 UTC") + 1:10*as.duration(1e9))
}
test_string <- function() {
  checkEquals(matrix(as.character(1:10), 5, 2), c1 ? matrix(as.character(1:10), 5, 2))
}
# zts may have tzone difference, so here again don't use checkEquals
test_zts <- function() {
  x <- xts(1:9, as.POSIXct("1970-01-01", tz="UTC")+1:9)
  z <- (c1 ? zts(as.time("1970-01-01 00:00:00 UTC") + 1:9*as.duration(1e9), 1:9))
  checkTrue(all(index(x) == index(z)))
  checkTrue(all(coredata(x) == coredata(z)))
}

## list
test_list_double <- function() {
  checkEquals(list(1,2,3), c1 ? list(1,2,3))
}
test_list_nested <- function() {
  checkEquals(list(1, list(1,"2",TRUE), 3), c1 ? list(1, list(1,"2",TRUE), 3))
}
test_list_double_named <- function() {
  checkEquals(list(a=1,b=2,c=3), c1 ? list(a=1,b=2,c=3))
}
test_list_double_partially_named <- function() {
  checkEquals(list(a=1,2,c=3), c1 ? list(a=1,2,c=3))
}
test_list_nested_partially_named <- function() {
  checkEquals(list(1, list(a=1,"2",TRUE), c=3), c1 ? list(1, list(a=1,"2",TRUE), c=3))
}
  

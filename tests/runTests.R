pkg <- "rztsdb"

doTest <- function(results, path, file) {
  results[[file]] <- runTestFile(paste0(path, file))
  results
}

checkForErrors <- function(l) {
  for (nm in names(l)) {
    if (getErrors(l[[nm]])$nErr || getErrors(l[[nm]])$nFail) {
      stop(paste("error or failure in", nm, ": ", l[[nm]]))
    }
  }
}

.startZtsdb <- function(port) {
  cmd <- paste0("ztsdb -p ", port)
  system(cmd, ignore.stdout=TRUE, ignore.stderr=TRUE, wait=FALSE)    
}
.stopZtsdb <- function(port) {
  ps <- system("ps -e -o pid,command", intern=TRUE)
  z  <- ps[grepl(paste("ztsdb -p", port), ps)]
  m <- regexpr("^\\s*\\d+", z)
  pid <- regmatches(z, m)
  system(paste("kill -9", pid), ignore.stdout=TRUE, ignore.stderr=TRUE, wait=FALSE)
}
.connectZtsdb <- function(port) {
  i <- 1
  done <- FALSE
  con <- NULL
  while (is.null(con) && i <= 10) {
    done <- TRUE
    Sys.sleep(1)
    print(paste("trying connection on port: ", port))
    con <- tryCatch(connection("127.0.0.1", port), error=function(x) NULL)
    i <- i + 1
  }
  con
}


if (!require("RUnit", quietly = TRUE)) {
  cat("R package 'RUnit' cannot be loaded -- no unit tests run for package", pkg, "\n")
} else if (!require("xts", quietly = TRUE)) {
  cat("R package 'xts' cannot be loaded -- no unit tests run for package", pkg, "\n")
} else if (!require("rztsdb", quietly = TRUE)) {
  cat("R package 'rztsdb' cannot be loaded -- no unit tests run for package", pkg, "\n")
} else {
  print(paste("working dir: ", getwd()))
  
  path <- paste0("../", pkg, "/unitTests/")

  port <- 23851
  print(paste("path is", path))

  .startZtsdb(port)
  c1 <- .connectZtsdb(port);
  if (is.null(c1)) {
    .stopZtsdb(port)
    stop(paste("can't connect to 'ztsdb' instance"))
  }
  
  results <- list()
  results <- doTest(results, path, "test_retrieval.R")
  results <- doTest(results, path, "test_escape.R")

  .stopZtsdb(port)
  checkForErrors(results)
}

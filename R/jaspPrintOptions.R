.jaspPrintOptions <- list2env(list(
  showDevInfo     = TRUE,
  useUnicode      = TRUE,
  indentWithTabs  = FALSE
))

#' @rdname jaspPrintOptions
#' @export
getJaspPrintOption <- function(name) return(get(name, envir = .graphOptions))

#' @rdname jaspPrintOptions
#' @export
setJaspPrintOption <- function(name, value) {
  assign(name, value, envir = .graphOptions)
  return(invisible(value))
}

#' @rdname jaspPrintOptions
#' @title Global printing options for JASP objects
#' @param ... modify options using name = value.
#' @param name character string of the value to get or set.
#' @param value the value to change x into.
#'
#' @export
jaspPrintOptions <- function(...) {

  args <- list(...)
  if (length(args) == 1L && is.list(args[[1L]]))
    args <- args[[1L]]

  if (!(length(args) && is.null(names(args)))) {
    if (length(args)) {
      for (i in seq_along(args)) {
        setJaspPrintOption(names(args)[[i]], args[[i]])
      }

      return(invisible(args))

    } else {

      return(as.list(.jaspPrintOptions))

    }
  }

  args <- unlist(args)
  out <- as.list(.jaspPrintOptions)[args]

  if (length(out) == 1)
    out <- out[[1]]

  return(out)
}


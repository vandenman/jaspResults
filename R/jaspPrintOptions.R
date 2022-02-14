toCamelCase <- function(x) {
  substr(x, 1, 1) <- toupper(substr(x, 1, 1))
  x
}

printOptionNameRtoCPP <- function(nms, getter = TRUE) {
  paste0("jaspPrintOptions", if (getter) "Get" else "Set", toCamelCase(nms))
}

getJaspResultsEnvir <- function() {
  # TODO: won't work inside JASP yet!
  as.environment("package:jaspResults")
}

initJaspPrintOptions <- function() {
  # should only be called in .onAttach or .onLoad
  jaspResultsEnvir <- getJaspResultsEnvir()
  if (!exists(".jaspPrintOptions", envir = jaspResultsEnvir)) { # necessary because checkForJaspResultsInit call .onAttach...
    # statically list .jaspPrintOptions
    # .jaspPrintOptions <- list2env(list(
    #   printDevInfo    = jaspPrintOptionsGetPrintDevInfo(),
    #   useUnicode      = jaspPrintOptionsGetUseUnicode(),
    #   indentWithTabs  = jaspPrintOptionsGetIndentWithTabs(),
    #   indentSize      = jaspPrintOptionsGetIndentSize()
    # ))

    # dynamic generate .jaspPrintOptions
    nms <- c("printDevInfo", "useUnicode", "indentWithTabs", "indentSize")
    .jaspPrintOptions <- lapply(printOptionNameRtoCPP(nms), function(nm) {
      get(x = nm, mode = "function", envir = jaspResultsEnvir)()
    })
    names(.jaspPrintOptions) <- nms
    .jaspPrintOptions <- list2env(.jaspPrintOptions)
    assign(".jaspPrintOptions", .jaspPrintOptions, envir = jaspResultsEnvir)
  }
}

#' @rdname jaspPrintOptions
#' @export
getJaspPrintOption <- function(name) return(get(name, envir = .jaspPrintOptions))

#' @rdname jaspPrintOptions
#' @export
setJaspPrintOption <- function(name, value) {
  assign(name, value, envir = .jaspPrintOptions)
  setJaspPrintOptionCPP(name, value)
  return(invisible(value))
}

setJaspPrintOptionCPP <- function(name, value) {
  fun <- get(printOptionNameRtoCPP(name, getter = FALSE), mode = "function", envir = getJaspResultsEnvir())
  fun(value)
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


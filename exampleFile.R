rm(list = ls())
library(jaspResults)

dummyTable <- function(title = "hola") {
  tb <- createJaspTable(title = title)
  tb$addColumnInfo("model", title =  "Model",   type = "string")
  tb$addColumnInfo("lower", title =  "Lower",   type = "number", overtitle = "95% Confidence interval")
  tb$addColumnInfo("upper", title =  "Upper",   type = "number", overtitle = "95% Confidence interval")
  tb$addColumnInfo("pval",  title =  "p-value", type = "pvalue")

  set.seed(123)
  tb$setData(data.frame(
    model = paste0("H", 0:2),
    lower = rnorm(3),
    upper = rnorm(3),
    pval  = runif(3)
  ))
  tb
}

dummyPlot <- function() {
  plt <- createJaspPlot(title = "I'm a plot!")
  set.seed(42)
  plt$plotObject <- ggplot2::qplot(rnorm(10), rnorm(10))
  plt
}

dummyContainer <- function() {
  cont <- createJaspContainer(title = "containerino")
  cont[["tablio"]] <- dummyTable()
  cont[["plotio"]] <- dummyPlot()
  cont[["table2"]] <- dummyTable("Also a table")
  cont
}

tb <- dummyTable()

jaspPrintOptionsSetPrintDevInfo(FALSE)
tb
jaspPrintOptionsSetPrintDevInfo(TRUE)
tb

tb$transpose <- TRUE
jaspPrintOptionsSetPrintDevInfo(FALSE)
tb
jaspPrintOptionsSetPrintDevInfo(TRUE)
tb

cont <- dummyContainer()

jaspPrintOptionsSetPrintDevInfo(FALSE)
cont
jaspPrintOptionsSetPrintDevInfo(TRUE)
cont

rcont <- toRobject(cont)

rcont$hola
as.data.frame(rcont$hola)

rcont$`Also a table`

rcont$`I'm a plot!`$`I'm a plot!`
rcont$`I'm a plot!`[[1]]

env <- attributes(rcont)$jaspObject
names(env)
parent.env(env)
env$jaspObject$print()


rcont

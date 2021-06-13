library(jaspResults)

container <- createJaspContainer(title = "This is a container")
container$print()

container[["internal_name_1"]] <- createJaspContainer(title = "name shown to users 1")
container[["internal_name_2"]] <- createJaspContainer(title = "name shown to users 2")
container$print()

# let's reverse the position
container[["subcontainer1"]]$position <- 2
container[["subcontainer2"]]$position <- 1
container$print()

# TODO:

## to discuss with Joris/ others

- [ ] ensure there midlines always extend one char beyond the contents
currently, the amount of space in this example is ok:

hoi:
||||───────────────────────────────────────────|
||||        | 95% Confidence Interval  |       |
||||        |──────────────────────────|       |
|||| Model  |    Lower    |   Upper    |   p   |
||||───────────────────────────────────────────|
||||      H0|   9.0000e-01|  1.1000e+00|  0.040|
||||      H1|   9.5000e-01|  1.2000e+00|  0.080|
||||      H2|   7.0000e-01|  1.3000e+00|  0.005|
||||───────────────────────────────────────────|

but the formatting of all columns is incorrect, because the data is aligned right.
What we want is this:

hoi:
||||───────────────────────────────────────────|
||||        | 95% Confidence Interval  |       |
||||        |──────────────────────────|       |
|||| Model  |    Lower    |   Upper    |   p   |
||||───────────────────────────────────────────|
||||     H0 |  9.0000e-01 | 1.1000e+00 | 0.040 |
||||     H1 |  9.5000e-01 | 1.2000e+00 | 0.080 |
||||     H2 |  7.0000e-01 | 1.3000e+00 | 0.005 |
||||───────────────────────────────────────────|

we could achieve this by

1. Formatting the data on the fly.
2. modifying the column spacing so that it only happens whenever 

either way, we need to account for the colType, because strings should be aligned right but numbers not.

- [ ] come up with a consistent style for how to print titles.
  - [ ] jaspObject::dataToString does not print the title of an object (jaspObject::toString does). Why? Should we overwrite this?

- [ ] what to do about transposed tables?

- [ ] for container keys, do we want to print "\"key\":" (current) or "$key"?

- [ ] do we want to use unicode for prettyness? for example, cat("\033[3mhoi\033[23m") to show italic hoo? 
  If yes
  - [ ] how do we changes stuff like <em>Note.</em> to \033[3mNote.\033[23m?
  - [ ] does this work on windows?

- [ ] what to do with html?

## to fix myself

- [x] container indent does not work
- [x] indent size should be variable

## to discuss with Simon
- [ ] do we want `format()` on a jaspObject to return the raw `obj$toString()`?
- [ ] which table options should we deprecate (if any).
  - [ ] look at correlations for rowOverTitles

- [x] unit tests with snapshots?
  - [ ] add option to hide footnote in print function
  - [ ] sounds good but how to do tolerance?

- [x] summary method
  - [x] one line sentence what the object contains

- [x] drop tibble, only return data.frame

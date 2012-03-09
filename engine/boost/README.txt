This folder contains header-only libraries from Boost 1.49.0 (See
http://www.boost.org). To add something new, just copy it from the Boost 1.49.0
sources - they're all in the /boost folder - and then continue to copy in
dependencies that the compiler complains to you about until your code compiles.
Be sure to add the new stuff into the repository with git add.

If you need a compiled Boost library - one that doesn't work as header-only -
post to the simulationcraft-swtor Google group and we'll decide how to change
our integration strategy to work with libraries that must be built and linked.
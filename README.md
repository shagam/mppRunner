# mppRunner

###abstract

trivial running speed benchmarks written in Java and c
Many threads increment one static integer counter.

###usage

java -jar dist/mpprunner.jar</li>
java -jar dist/mpprunner.jar   [threads=32]  [iterations=1000000]  [verbose]</li>

c:


make
mppRunner

mppRunner [threads=32]  [iterations=1000000]  [verbose] [loose] [atomic]

When running with no parameters, uses default parameters.


###arguments

Unique prefix of argument name is enough (No need to type full name).


treads     - number of concurrent threads</li>
iterations - loops</li>
atomic     - c only, use hardware lock (faster)</li>
oose      - c only, no lock, faster (wrong result)</li>

###help

When running with no parameters (Or with) help is printed.
The help is created automatically.
See Args.java args.c

review  args.utest ()   Args.utest()  usage examples.

### files

<ul>
<li>Makefile -  compiles c program</li>
<li>Mpprunner.java</li>
<li>Args.java             Argumet parser for Java (MIT license)</li>
<li>mpprunner.jar</li>
<li>mpprunner.c</li>
<li>args.c                Argumet parser for c (MIT license)</li>
<li>args.h</li>
</ul>

<h3>
<a id="authors-and-contributors" class="anchor" href="#authors-and-contributors" aria-hidden="true"><span aria-hidden="true" class="octicon octicon-link"></span></a>Authors and Contributors</h3>

<p><a href="mailto:eli.shagam@gmail.com">eli.shagam@gmail.com</a></p>
~                                                                                      
~       

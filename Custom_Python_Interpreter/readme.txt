This example code contains a custom Python interpreter. The rationale behind this is the following
context:
 * You love writing Python scripts - there are a lot of great libraries that make life easier.
   Let's say you're using SQLAlchemy to manage a database.
 * You have a C/C++ Python module that contains functionality that shouldn't be "exposed" publicly.
   Let's say you're decrypting the database password which is stored on disk.

To realize this, you need a "constrained" execution environment that will accept only "your"
scripts. This can easily be done by digitally signing your scripts and having the interpreter
check for a valid signature.

This project contains a very basic sample implementation of an interpreter and a signing tool.
It works in the following way:
 (1) The interpreter's main is a stripped-down version of the "original" Python interpreter,
     removing features such as environment variable support (to secure the execution environment).
 (2) The interpreter can do 2 things:
     (a) Execute a script/file given as argument.
     (b) Start an interactive shell.
Depending on *your* scenario, you might want to disable one of these features. Also, Python by
default allows to run modules as scripts using the "-m" option. This has been removed because it
doesn't work with built-in (non-scripted) modules (at least in Python 3 ... it seemed to work in
Python 2). You might want to re-add this functionality.

Access control is implemented in 3 flavors:
 (1) Using a detached signature: When trying to execute "foo.py", the interpreter checks for a valid
     signature in "foo.py.signature".
 (2) Using a "standalone" script: Python supports executing a ZIP file by checking for a contained
     "__main__.py". Since ZIP files store their metadata at the end, the signature can be put
     in a header *before* the actual ZIP. Prepending a shebang line, this is a standalone.
 (3) For unsigned scripts or interactive access, the interpreter asks for a "secret"
     password. This is only met to be exemplary (and useful for testing), *not* secure!
     Remove/adapt as needed.

Caveats:
 * Your script is signed, so it may not be tampered with, but it's *not* encrypted.
   Anyone can still read it!
 * System modules/libraries aren't signed or checked. If the local admin manages to manipulate
   one of the used system modules, your script isn't that safe anymore...
   (This is intentional, because you can't really protect yourself against 'root' ...)

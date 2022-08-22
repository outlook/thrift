Apache Thrift
=============

Last Modified: 2017-11-10

License
=======

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements. See the NOTICE file
distributed with this work for additional information
regarding copyright ownership. The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied. See the License for the
specific language governing permissions and limitations
under the License.

Introduction
============

Thrift is a lightweight, language-independent software stack with an
associated code generation mechanism for RPC. Thrift provides clean
abstractions for data transport, data serialization, and application
level processing. The code generation system takes a simple definition
language as its input and generates code across programming languages that
uses the abstracted stack to build interoperable RPC clients and servers.

![Apache Thrift Layered Architecture](doc/images/thrift-layers.png)

Thrift makes it easy for programs written in different programming
languages to share data and call remote procedures.  With support
for [over 20 programming languages](LANGUAGES.md), chances are Thrift
supports the ones that you currently use.

Thrift is specifically designed to support non-atomic version changes
across client and server code.

For more details on Thrift's design and implementation, take a gander at
the Thrift whitepaper included in this distribution or at the README.md file
in your particular subdirectory of interest.

Project Hierarchy
=================

thrift/

  compiler/

    Contains the Thrift compiler, implemented in C++.

  lib/

    Contains the Thrift software library implementation, subdivided by
    language of implementation.

    cpp/
    go/
    java/
    php/
    py/
    rb/
    ...

  test/

    Contains sample Thrift files and test code across the target programming
    languages.

  tutorial/

    Contains a basic tutorial that will teach you how to develop software
    using Thrift.

Requirements
============

See http://thrift.apache.org/docs/install for an up-to-date list of build requirements.

Resources
=========

More information about Thrift can be obtained on the Thrift webpage at:

     http://thrift.apache.org

Acknowledgments
===============

Thrift was inspired by pillar, a lightweight RPC tool written by Adam D'Angelo,
and also by Google's protocol buffers.

Installation
============

Use Homebrew to install the dependencies:

```
brew install autoconf automake bison pkg-config boost
```

If you are building from the first time out of the source repository, you will
need to generate the configure scripts.  (This is not necessary if you
downloaded a tarball.)  From the top directory, do:

```
./bootstrap.sh
```

From the top directory, do:

```
export CXXFLAGS=-Wno-deprecated-copy-with-user-provided-copy
export PATH="/usr/local/opt/bison/bin:$PATH"
./configure --enable-libs=no
cd compiler/cpp
make
```

Note that some language packages must be installed manually using build tools
better suited to those languages (at the time of this writing, this applies
to Java, Ruby, PHP).

Look for the README.md file in the lib/<language>/ folder for more details on the
installation of each language library package.

Testing
=======

There are a large number of client library tests that can all be run
from the top-level directory.

          make -k check

This will make all of the libraries (as necessary), and run through
the unit tests defined in each of the client libraries. If a single
language fails, the make check will continue on and provide a synopsis
at the end.

To run the cross-language test suite, please run:

          make cross

This will run a set of tests that use different language clients and
servers.

Development
===========

To build the same way Travis CI builds the project you should use docker.
We have [comprehensive building instructions for docker](build/docker/README.md).

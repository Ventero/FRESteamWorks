# Steamworks API for AIR applications #

A simple Steamworks API wrapper to be used as native extension for Adobe AIR
applications (see src/FRESteamWorks). Since the AIR runtime on Linux doesn't
support native extensions, it also includes a small command line tool which
can communicate with the AIR application (src/APIWrapper).

The library to be used on the AS3 side can be found in lib/src/ (or lib/src_linux
for Linux). Scripts to create the ANE/SWC files are in lib/bin/.

It also includes a small test application (in test/src/), which tests the basic
Steamworks API functions. It uses app id 480, which is the example application from
the Steamworks SDK.

---

# License #

Copyright (c) 2012-2013, Level Up Labs, LLC  
Copyright (c) 2012, Oldes  
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

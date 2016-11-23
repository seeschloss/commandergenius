This is Enigma, a puzzle game, ported to Google Android.

This uses commandergenius, but since commandergenius itself was patched to enable/disable
some things a bit specific to Enigma (like gyroscope and screen rotation handling) I feel
it's better to make it live its life alone in its own repository.


Installation
============

Install latest Android SDK and NDK from http://developer.android.com/index.html
You'll need to install Java Ant too. Since for building apk files some java classes are needed as well,
it is recommended to install OpenJDK and its development files.
The application should run on Android 2.3 and above, but will use features from Android 6.0 if available.
The most supported environment for this port is Linux, MacOs should be okay too.


How to compile
==============

Launch the following commands:

	./changeAppSettings.sh -a
	./build.sh

The resulting compiled package will be in `project/bin`.


License information
===================

The SDL 1.2 port is licensed under LGPL, so you may use it for commercial purposes
without releasing source code, however to fullfill LGPL requirements you'll have to publish
the file AndroidAppSettings.cfg to allow linking other version of libsdl-1.2.so with the libraries
in the binary package you're distributing - typically libapplication.so and other
closed-source libraries in your .apk file.

Java source files are licensed under zlib license, which means you may modify them as you like
without releasing source code.

The libraries under project/jni have their own license, none of them are GPL or anything too
inconvenient.

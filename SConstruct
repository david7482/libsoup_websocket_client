import os

# Set our required libraries
libraries 		= []
library_paths 	= ''
cppDefines 		= {}
cFlags 		    = ['-Wall']

# Define the attributes of the build environment shared
env = Environment()
env.Append(LIBS 		= libraries)
env.Append(LIBPATH 		= library_paths)
env.Append(CPPDEFINES 	= cppDefines)
env.Append(CFLAGS 		= cFlags)

env.ParseConfig('pkg-config --cflags --libs libsoup-2.4')
env.ParseConfig('pkg-config --cflags --libs glib-2.0')

# Build test programs
env.Program('#bin/websocket_client', 'websocket_client.c')
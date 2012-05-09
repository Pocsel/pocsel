#!/bin/sh

make `cat Makefile | grep gccPCH_fc: | cut -d : -f 1`
#make client_gccPCH_fc
#make server_gccPCH_fc
#make test_database_gccPCH_fc
#make test_lua_gccPCH_fc
#make test_udp_sv_gccPCH_fc
#make test_udp_cl_gccPCH_fc
#make sdk_gccPCH_fc

#make client server base_plugin dev_world $1
make $1

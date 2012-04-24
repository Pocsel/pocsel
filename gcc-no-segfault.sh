#!/bin/sh

make client_gccPCH_fc
make server_gccPCH_fc
make test_database_gccPCH_fc
make test_lua_gccPCH_fc
make test_udp_sv_gccPCH_fc
make test_udp_cl_gccPCH_fc
#make sdk_gccPCH_fc

make $1

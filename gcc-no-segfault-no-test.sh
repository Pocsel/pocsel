#!/bin/sh

#make `cat Makefile | grep gccPCH_fc: | cut -d : -f 1`
make client_gccPCH_fc
make server_gccPCH_fc
make packager-cli_gccPCH_fc
make installer-cli_gccPCH_fc
#make test_database_gccPCH_fc
#make test_lua_gccPCH_fc
#make test_udp_sv_gccPCH_fc
#make test_udp_cl_gccPCH_fc
#make sdk_gccPCH_fc

make server client $1
# make server $1
# make packager-cli $1
# make installer-cli $1
# make base_plugin
# make dev_world
#make $1

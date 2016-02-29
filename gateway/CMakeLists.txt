cmake_minimum_required(VERSION 2.8)
PROJECT(gateway)

INSTALL(PROGRAMS "gateway.py" DESTINATION "sbin" RENAME "tinkwise_gw.py")
INSTALL(PROGRAMS "init_script" DESTINATION "/etc/init.d" RENAME "tinkwise")

SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_CONTACT "Florian Doerfler")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gateway that reads from the serial port and writes to rrd files")
SET(CPACK_PACKAGE_NAME "tinkwise_gateway")
SET(CPACK_PACKAGE_VERSION "0.1.0")
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
INCLUDE(CPack)

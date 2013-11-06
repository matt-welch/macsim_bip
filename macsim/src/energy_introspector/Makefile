# Makefile - simulator suite make file
#
# Please see copyright/licensing notices at the end of this file

##################################################################
#
# Modify the following definitions to suit your build environment,
# NOTE: most platforms should not require any changes
#
##################################################################

##
## vanilla Unix, GCC build
##
## tested hosts:
##
## Redhat Enterprise 5/Linux 2.6.18/64-bit
## Redhat Enterprise 4/Linux 2.6.9/32-bit
##
#CC = mpic++
CC = g++

##################################################################

MAKE = make
AR = ar qcv
AROPT =
RANLIB = ranlib
RM = rm -rf
LN = ln -s
LNDIR = ln -s
DIFF = diff
OEXT = o
LEXT = a
EEXT =
CS = ;
X=/

##################################################################
#
# complete flags
#
CFLAGS = -g

#
# all the sources
#
SRCS = \
test.cc \
parameters.cc \
parser.cc \
energy_introspector.cc \
ENERGYLIB_McPAT.cc \
ENERGYLIB_IntSim.cc \
THERMALLIB_HotSpot.cc \
RELIABILITYLIB_RAMP.cc \
SENSORLIB_RNG.cc

HDRS = \
parameters.h \
parser.h \
queue.h \
library.h \
energy_introspector.h \
ENERGYLIB_McPAT.h \
ENERGYLIB_IntSim.h \
THERMALLIB_HotSpot.h \
RELIABILITYLIB_RAMP.h \
SENSORLIB_RNG.h

OBJS = \
parameters.$(OEXT) \
parser.$(OEXT) \
ENERGYLIB_McPAT.$(OEXT) \
ENERGYLIB_IntSim.$(OEXT) \
THERMALLIB_HotSpot.$(OEXT) \
RELIABILITYLIB_RAMP.$(OEXT) \
SENSORLIB_RNG.$(OEXT) \
energy_introspector.$(OEXT) 


default: test

test$(EEXT):		test.$(OEXT) $(OBJS)
	$(CC) -o test$(EEXT) -pthread $(CFLAGS) test.$(OEXT) $(OBJS)

.cc.$(OEXT):
	$(CC) $(CFLAGS) -c $*.cc

libEI: $(OBJS)
	ar qcv libEI.a $(OBJS)
	ranlib libEI.a

filelist:
	@echo $(SRCS) $(HDRS) makefile

clean:
	$(RM) ENERGYLIB_IntSim/*.o ENERGYLIB_IntSim/*~
	$(RM) ENERGYLIB_McPAT/*.o ENERGYLIB_McPAT/cacti/*.o ENERGYLIB_McPAT/*~ ENERGYLIB_McPAT/cacti/*~
	$(RM) THERMALLIB_HotSpot/*.o THERMALLIB_HotSpot/*~
	$(RM) RELIABILITYLIB_RAMP/*.o RELIABILITYLIB_RAMP/*~
	$(RM) SENSORLIB_RNG/*.o SENSORLIB_RNG/*~
	$(RM) libEI.a *.o *~
	$(RM) test

parameters.o: energy_introspector.h parameters.h parser.h
parser.o: energy_introspector.h parameters.h parser.h
ENERGYLIB_McPAT.o: definitions.h parameters.h library.h queue.h
ENERGYLIB_McPAT.o: energy_introspector.h ENERGYLIB_McPAT.h
ENERGYLIB_IntSim.o: definitions.h parameters.h library.h queue.h
ENERGYLIB_IntSim.o: energy_introspector.h ENERGYLIB_IntSim.h
THERMALLIB_HotSpot.o: definitions.h parameters.h library.h queue.h
THERMALLIB_HotSpot.o: energy_introspector.h THERMALLIB_HotSpot.h
RELIABILITYLIB_RAMP.o: definitions.h parameters.h library.h queue.h
RELIABILITYLIB_RAMP.o: energy_introspector.h RELIABILITYLIB_RAMP.h
SENSORLIB_RNG.o: definitions.h parameters.h library.h queue.h
SENSORLIB_RNG.o: energy_introspector.h SENSORLIB_RNG.h
energy_introspector.o: definitions.h parameters.h parser.h library.h queue.h
energy_introspector.o: energy_introspector.h ENERGYLIB_McPAT.h ENERGYLIB_IntSim.h 
energy_introspector.o: THERMALLIB_HotSpot.h RELIABILITYLIB_RAMP.h
test.o: definitions.h parameters.h parser.h library.h queue.h
test.o: energy_introspector.h ENERGYLIB_McPAT.h ENERGYLIB_IntSim.h 
test.o: THERMALLIB_HotSpot.h RELIABILITYLIB_RAMP.h SENSORLIB_RNG.h

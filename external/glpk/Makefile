# Makefile of the library "external/glpk"
# Paul Boersma, 16 May 2026

OBJECTS = amd_1.o amd_2.o amd_aat.o amd_control.o amd_defaults.o amd_info.o \
   amd_order.o amd_post_tree.o amd_postorder.o amd_preprocess.o amd_valid.o \
   colamd.o \
   glpapi01.o glpapi02.o glpapi03.o glpapi04.o glpapi05.o glpapi06.o glpapi07.o \
   glpapi08.o glpapi09.o glpapi10.o glpapi11.o glpapi12.o glpapi13.o glpapi14.o \
   glpapi15.o glpapi16.o glpapi17.o glpapi18.o glpapi19.o \
   glpavl.o glpbfd.o glpbfx.o glpcpx.o glpdmp.o glpdmx.o \
   glpenv01.o glpenv02.o glpenv03.o glpenv04.o \
   glpenv05.o glpenv06.o glpenv07.o glpenv08.o \
   glpfhv.o glpgmp.o glphbm.o glpini01.o glpini02.o \
   glpios01.o glpios02.o glpios03.o glpios04.o glpios05.o glpios06.o \
   glpios07.o glpios08.o glpios09.o glpios10.o glpios11.o glpios12.o \
   glpipm.o glplib01.o glplib02.o glplib03.o glplpf.o \
   glplpx01.o glplpx02.o glplpx03.o \
   glpluf.o glplux.o glpmat.o \
   glpmpl01.o glpmpl02.o glpmpl03.o glpmpl04.o glpmpl05.o glpmpl06.o glpmps.o \
   glpnet01.o glpnet02.o glpnet03.o glpnet04.o glpnet05.o \
   glpnet06.o glpnet07.o glpnet08.o glpnet09.o \
   glpnpp01.o glpnpp02.o glpnpp03.o glpnpp04.o glpnpp05.o \
   glpqmd.o glprgr.o glprng01.o glprng02.o \
   glpscf.o glpscl.o glpsdf.o glpspm.o glpspx01.o glpspx02.o \
   glpsql.o glpssx01.o glpssx02.o glptsp.o

.PHONY: all clean

all: libglpk.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libglpk.a

libglpk.a: $(OBJECTS)
	ar rus libglpk.a $(OBJECTS)

$(OBJECTS): *.h

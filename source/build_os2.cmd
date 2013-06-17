del jpeglib\*.o
del jpeglib\libjpeg.a
cd jpeglib
gcc -O2 -D TEMP_DIRECTORY="\"\"" -c ansi2knr.c cdjpeg.c cjpeg.c ckconfig.c djpeg.c example.c jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c jcphuff.c jcprepct.c jcsample.c jctrans.c jdapimin.c jdapistd.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c jdphuff.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c jidctred.c jmemansi.c jmemmgr.c jmemname.c jmemnobs.c jpegtran.c jquant1.c jquant2.c jutils.c rdbmp.c rdcolmap.c rdgif.c rdjpgcom.c rdppm.c rdrle.c rdswitch.c rdtarga.c transupp.c wrbmp.c wrgif.c wrjpgcom.c wrppm.c wrrle.c wrtarga.c
ar -ru libjpeg.a *.o
cd ..
gcc -O2 -static -I ./jpeglib -D __OS2__=1 -D ALLOW_ROTATE=0 -o ../bin/d70reader_no_rotate.exe d70reader.c d70jpegtran.c d70buildweb.c d70resize.c d70utils.c ./jpeglib/cdjpeg.o ./jpeglib/rdswitch.o ./jpeglib/transupp.o ./jpeglib/libjpeg.a
gcc -O2 -static -I ./jpeglib -D __OS2__=1 -D ALLOW_ROTATE=1 -o ../bin/d70reader.exe d70reader.c d70jpegtran.c d70buildweb.c d70resize.c d70utils.c ./jpeglib/cdjpeg.o ./jpeglib/rdswitch.o ./jpeglib/transupp.o ./jpeglib/libjpeg.a
del jpeglib\*.o
del jpeglib\libjpeg.a



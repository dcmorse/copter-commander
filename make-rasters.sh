
for file in *.c; do 
  $CC $CFLAGS -c $file ;
done ;

touch built


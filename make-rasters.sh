
echo -n Compiling "\".c\"s    to" \".o\"s

for file in *.c; do 
  echo -n "."
  $CC $CFLAGS -c $file ;
done ;

echo done.

touch built


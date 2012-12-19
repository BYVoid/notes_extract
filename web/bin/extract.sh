basepath=$1
wavefile=$2
cd $basepath
../../bin/notes $wavefile 1
mono ../../bin/sheet.exe notes.mid sheet
timidity notes.mid -o notes.ogg -Ov

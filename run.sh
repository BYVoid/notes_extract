notes_track() {
  prefix=$1
  algo=$2
  surfix=$3
  wavefld="../wave"
  resfld="./res"
  ./notes "${wavefld}/${prefix}.wav" $algo
  mono sheet.exe notes.mid "${resfld}/${prefix}_${surfix}"
  mv notes.txt "${resfld}/${prefix}_${surfix}.txt"
  mv notes.mid "${resfld}/${prefix}_${surfix}.mid"
  echo $surfix
}

file=$1
notes_track $file "0" "mcomb"
notes_track $file "1" "yinfft"
notes_track $file "2" "yin"
notes_track $file "3" "schmitt"
notes_track $file "4" "fcomb"

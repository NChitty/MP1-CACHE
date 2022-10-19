validation_0() {
  ../sim_cache 16 1024 2 0 0 0 0 gcc_trace.txt > test0.txt
  diff -iw test0.txt ../debug/debug0.txt > diff0.txt
  ret=$?
  if [[ $ret -eq 0 ]]; then
    echo 'Validation 0 passed'
  else
    echo 'Validation 0 failed'
  fi
}

validation_1() {
  ../sim_cache 16 1024 1 0 0 0 0 perl_trace.txt > test1.txt
  diff -iw test1.txt ../debug/debug1.txt > diff1.txt
    ret=$?
    if [[ $ret -eq 0 ]]; then
      echo 'Validation 1 passed'
    else
      echo 'Validation 1 failed'
    fi
}

validation_2() {
  ../sim_cache 16 1024 2 0 0 1 0 gcc_trace.txt > test2.txt
  diff -iw test2.txt ../debug/debug2.txt > diff2.txt
  ret=$?
  if [[ $ret -eq 0 ]]; then
    echo 'Validation 2 passed'
  else
    echo 'Validation 2 failed'
  fi
}

validation_3() {
  ../sim_cache 16 1024 2 0 0 2 0 vortex_trace.txt > test3.txt
  diff -iw test3.txt ../debug/debug3.txt > diff3.txt
  ret=$?
  if [[ $ret -eq 0 ]]; then
    echo 'Validation 3 passed'
  else
    echo 'Validation 3 failed'
  fi
}

validation_4() {
  ../sim_cache 16 1024 2 8192 4 0 0 gcc_trace.txt > test4.txt
  diff -iw test4.txt ../debug/debug4.txt > diff4.txt
  ret=$?
  if [[ $ret -eq 0 ]]; then
    echo 'Validation 4 passed'
  else
    echo 'Validation 4 failed'
  fi
}

validation_5() {
  ../sim_cache 16 1024 1 8192 4 0 0 go_trace.txt > test5.txt
  diff -iw test5.txt ../debug/debug5.txt > diff5.txt
  ret=$?
  if [[ $ret -eq 0 ]]; then
    echo 'Validation 5 passed'
  else
    echo 'Validation 5 failed'
  fi
}

validation_6() {
  ../sim_cache 16 1024 2 8192 4 0 1 gcc_trace.txt > test6.txt
  diff -iw test6.txt ../debug/debug6.txt > diff6.txt
  ret=$?
  if [[ $ret -eq 0 ]]; then
    echo 'Validation 6 passed'
  else
    echo 'Validation 6 failed'
  fi
}

validation_7() {
  ../sim_cache 16 1024 1 8192 4 0 1 compress_trace.txt > test7.txt
  diff -iw test7.txt ../debug/debug7.txt > diff7.txt
  ret=$?
  if [[ $ret -eq 0 ]]; then
    echo 'Validation 7 passed'
  else
    echo 'Validation 7 failed'
  fi
}

make
cd traces
case $1 in
  0)
    echo "Running validation 0"
    validation_0
    ;;
  1)
    echo "Running validation 1"
    validation_1
    ;;
  2)
    echo "Running validation 2"
    validation_2
    ;;
  3)
    echo "Running validation 3"
    validation_3
    ;;
  4)
    echo "Running validation 4"
    validation_4
    ;;
  5)
    echo "Running validation 5"
    validation_5
    ;;
  6)
    echo "Running validation 6"
    validation_6
    ;;
  7)
    echo "Running validation 7"
    validation_7
    ;;
  all)
    validation_0 &
    validation_1 &
    validation_2 &
    validation_3 &
    validation_4 &
    validation_5 &
    validation_6 &
    validation_7 &
    ;;
esac
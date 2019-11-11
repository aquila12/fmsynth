lfo 0 5.0
lfo 1 1.0
op.r 3 0.998
op.r 0 2.003
op.r 1 1.002
op.r 2 1.987
op.e 1 10.0 10.0 0.0 0.7 15.0 ; Operator 1 envelope
op.e 2 10.0 10.0 0.0 0.7 15.0
prog
  lmix 0 0.05
  feed 0
  feed 3
  mix 3 7.0
  feed 1
  mix 0 3.5
  feed 2
  mix 1 0.8
  mix 2 0.8
  amod 1 0.1
  out
end

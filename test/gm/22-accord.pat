lfo 0 3.3
lfo 1 1.0
lfo 2 4.6
op.r 3 0.998
op.r 0 2.003
op.r 1 1.002
op.r 2 1.987
op.e 1 10.0 10.0 0.0 0.7 15.0 ; Operator 1 envelope
op.e 2 10.0 10.0 0.0 0.7 15.0
prog
  lmix 0 0.1
  feed 0
  lmix 2 0.05
  feed 3
  mix 3 2.1
  feed 1
  mix 0 1.1
  feed 2
  mix 1 0.8
  mix 2 0.6
  amod 1 0.1
  out
end

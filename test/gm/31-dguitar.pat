lfo 0 5.0
lfo 1 4.0
op.r 0 0.5
op.r 2 1.498
op.r 1 1.002
op.e 1 150.0 10.0 0.2 0.7 10.0 ; Operator 1 envelope
prog
  mix 2 0.25   ; FB
  feed 2
  lmix 0 0.05
  mix 2 0.18
  feed 0
  mix 0 0.4
  feed 1
  mix 1 1.0
  amod 1 0.1
  out
end

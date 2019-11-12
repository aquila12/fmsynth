op.r 0 1.002
op.r 1 0.998
op.e 1 100.0 20.0 0.5 0.6 5.0
op.r 2 1.002
op.e 2 80.0 0.5 0.1 0.7 100.0 ; Resonance
op.r 3 0.499
op.e 3 80.0 5.0 10.0 0.7 15.0  ; Pluck
prog
  feed 0
  mix 1 0.34 ; FB
  feed 1
  mix 0 0.7
  feed 2
  mix 1 0.06
  feed 3
  mix 2 2.0
  mix 3 0.4
  out
end

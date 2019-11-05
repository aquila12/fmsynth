op.r 0 1.001
op.r 1 0.998
op.e 1 100.0 20.0 0.5 0.5 5.0
op.r 2 1.001
op.e 2 80.0 0.5 0.1 0.7 100.0 ; Resonance
op.r 3 0.499
op.e 3 80.0 2.0 0.1 0.6 15.0  ; Pluck
prog
  feed 0
  mix 1 0.6 ; FB
  feed 1
  mix 0 3.0
  feed 2
  mix 1 10.0
  feed 3
  mix 2 2.0
  mix 3 1.0
  out
end

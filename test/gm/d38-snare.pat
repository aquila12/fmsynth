op.a 0 150.7
op.e 0 80 12 7 0.7 7    ; Resonance
op.a 1 85.3
op.e 1 150 2 6 0.7 6 ; FB
op.a 2 195.4
op.e 2 150 30 10 0.6 10 ; Kick
prog
  mix 1 0.57
  feed 1
  mix 1 2.15
  feed 0
  mix 1 0.2
  feed 2
  mix 0 1.0
  mix 2 1.2
  out
end

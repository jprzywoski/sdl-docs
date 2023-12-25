puts "0"
puts "0"
puts "NBLIGNE"
for { set i 0 } { $i < 80 } { incr i } {
	puts  "[ expr $i * 7] 0 -1"
	}
for { set i 0 } { $i < 20 } { incr i } {
	puts  "553  [expr $i * 5]  -1"
	}
for { set i 79 } { $i > 0 } { set i [ expr $i - 1] } {
	puts  "[ expr $i * 7] 100 -1"
	}
for { set i 0 } { $i < 20 } { incr i } {
	puts  "0  [ expr [expr $i * 5] + 100] -1 "
	}
for { set i 0 } { $i < 80 } { incr i } {
	puts  "[ expr $i * 7] 200  -1"
	}
for { set i 0 } { $i < 20 } { incr i } {
	puts  "553   [ expr [expr $i * 5] + 200] -1 "
	}
for { set i 79 } { $i > 0 } { set i [ expr $i - 1] } {
	puts  "[ expr $i * 7] 300 -1"
	}

puts "620"
puts "0"
puts "NBLIGNE"
for { set i 79 } { $i > 0 } { set i [ expr $i - 1] } {
	puts  "[ expr $i * 7] 50"
	}
for { set i 0 } { $i < 20 } { incr i } {
	puts  "0   [expr [expr $i * 5] + 50 ] "
	}
for { set i 0 } { $i < 80 } { incr i } {
	puts  "[ expr $i * 7] 150"
	}
for { set i 0 } { $i < 20 } { incr i } {
	puts  "553   [ expr [expr $i * 5] + 150] "
	}
for { set i 79 } { $i > 0 } { set i [ expr $i - 1] } {
	puts  "[ expr $i * 7] 250"
	}
